# Netx processing and update model

Netx separates mutable module/component state from the state used by the real-time
processor. This is the central threading rule in Netx: the main thread builds a
complete processor snapshot, and the audio thread only consumes that snapshot.

The model prevents the audio thread from observing a Component or Module halfway
through an edit, reconnection, or rebuild.

## The two kinds of state

| State | Owner | Thread/use |
| --- | --- | --- |
| Component and Module fields | Component or Module | Main thread; mutable |
| `Component::RtConfig` cache | Component | Main thread; non-retaining, reusable description |
| Module and Network processor snapshots | Network | Retained generations used by real-time processing |
| Processor objects returned by `OnUpdate` | Current Network generation | Read by the audio thread |

A Network generation strongly retains its Module processor data. Netx module
processor data in turn strongly retains every Component and processor object that
participates in that generation. Retired generations remain alive until real-time
processing no longer uses them.

Consequently, a processor callback may outlive the attachment of the Component or
Module that created it. It must not assume that non-retaining links such as
`component.module` or `module.network` are still valid.

## Update flow

```text
main-thread edit / connection change / attach / detach
        |
        v
Module and Network receive refresh flags
        |
        v
Network compiles a new processor generation
        |
        v
Module::OnUpdate builds component processor entries
        |
        v
Network publishes the retained generation to real-time processing
```

`OnUpdate` runs while the previous Network generation is still retained. This
makes it possible to borrow and reuse its processor configuration safely during
the update call.

## Component processing

A processing Component implements:

```cpp
RtConfig OnUpdate(
    UInt8 flags,
    const Data::PropertySet& properties,
    const StreamsView& streams,
    const RtConfig* previous) override;
```

The method returns a *complete replacement* configuration. `RtConfig::processor`
is a `ConstTRef<Object>` and is deliberately non-retaining. Netx promotes the
returned processor into the strongly retained Network generation immediately
after the update completes.

The normal implementation creates a processor and returns it directly:

```cpp
auto processor = New<MyProcessor>(properties, streams);

return
{
    .processor = processor,
    .callback = &MyComponent::ProcessRt,
    .priority = kProcessOrder
};
```

Return `{}` when the Component should not process. A callback without a processor
is also valid when the callback needs only streams.

### Reusing a previous configuration

`previous` is null only for a Component's first update after attachment. Otherwise
it is a borrowed main-thread view of the Component's prior configuration, valid
only for the call. Do not store or mutate it.

When an update does not require a rebuild, return it by value:

```cpp
REFLEX_ASSERT(previous);
return *previous;
```

This is particularly useful for Components whose update mask is broader than the
set of flags that actually changes their processor.

### Rejected newly created processors

`New<T>()` returns a non-retaining `TRef`. Returning it in a successful
`RtConfig` is correct because the Network generation adopts it. If an allocated
processor is rejected before being returned, it must be released explicitly:

```cpp
auto processor = New<MyProcessor>(/* ... */);

if (!processor->valid)
{
    AutoRelease(processor);
    return {};
}

return { .processor = processor, .callback = &MyComponent::ProcessRt };
```

Do not assign a new processor into a `RtConfig` and later clear that config as a
failure path. The config is non-retaining, so that pattern leaks the rejected
object.

## Refresh flags and initialization

`SetUpdateFlags(access, flags, mask)` has two roles:

- `flags` are raised when the Component calls `Update()` or changes recorded
  state.
- `mask` chooses which subsequent Module updates call the Component's
  `OnUpdate`.

The first `OnUpdate` is special. It always receives network-properties and
module-connections flags, plus the Component's configured update flags, even if
the mask opted out of one or both common flags. This lets a Component build its
initial processor without depending on unrelated global refreshes.

Component attachment and detachment use the private
`Core::kRefreshFlagModuleUpdateInternal` flag. It rebuilds the Module processor
snapshot without calling already initialized Components merely because a sibling
was attached or removed. This bit replaced the former public update group 5.

Current convention is to reserve lower update groups for client code and use the
higher available group for bootstrap/add-on Components where a dedicated group is
needed.

## Module processing

`Core::Module` has a corresponding snapshot API:

```cpp
TRef<Object> OnInitialiseProcessor() override;

RtConfig OnUpdate(
    const Data::PropertySet& properties,
    const StreamsView& streams,
    UInt8 flags,
    ConstTRef<Object> previous_processor) override;
```

`OnAttachNetwork()` is a lifecycle callback only. Processor seeding belongs in
`OnInitialiseProcessor()`. Netx's `Module` implements this seed with its
component-process data, so ordinary Netx module subclasses do not need to expose
processor setup through their attachment override.

The Network keeps the current Module processor in its processor map. The seed
must therefore be valid even when no Component has an active callback; later
module updates can always receive an object of the expected processor type.

## Dispatch and priority

Netx stores every Component configuration in the Module processor data. Entries
with callbacks are ordered first by priority; inactive entries are placed after
all active entries. The active entry count allows the real-time loop to process
only active Components without testing for a null callback for every entry.

An active callback may replace `ProcessInfo::callback` at runtime, for example to
switch from a one-time preparation function to a steady-state function. It may
only replace it with another non-null callback. The returned `RtConfig` still
provides the initial callback for each newly compiled generation.

## Real-time callback rules

Processor callbacks should treat their `ProcessInfo` and processor object as the
complete state they need for the block. In particular, they should not read
main-thread mutable Component or Module state, allocate, block, or take locks.

If data must cross from the main thread to the audio thread, place it in a new
processor snapshot or use an explicitly designed real-time-safe mechanism. Do
not use a mutable Component field as an implicit cross-thread channel.

## Useful invariants

- A Component begins attachment with no cached `RtConfig` and clears it on
  detach.
- A Component `OnUpdate` receives null `previous` only on initialization.
- A returned processor is non-retaining until adopted by the Network snapshot.
- A retired snapshot may retain its Component and processor after detachment,
  but not their attachment links.
- Existing Components do not receive the private internal refresh flag.

See [BREAKING_CHANGES.md](BREAKING_CHANGES.md) for migration-specific examples
and compatibility notes.
