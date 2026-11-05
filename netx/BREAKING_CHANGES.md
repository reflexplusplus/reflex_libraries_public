# Breaking changes

## Component and module processor snapshots

`Component::OnUpdate` now returns a complete `RtConfig` instead of mutating a persistent retaining configuration:

```cpp
// Before
void OnUpdate(
	UInt8 flags,
	const Data::PropertySet & properties,
	const StreamsView & streams,
	RtConfig & config) override;

// After
RtConfig OnUpdate(
	UInt8 flags,
	const Data::PropertySet & properties,
	const StreamsView & streams,
	const RtConfig * previous) override;
```

The returned `RtConfig` contains a non-retaining processor reference. Netx immediately promotes it into the strongly retained processor snapshot owned by the Network. The previous Module-owned retaining cache has been removed.

### Migrating a Component

Return the complete processor configuration whenever `OnUpdate` is called:

```cpp
Component::RtConfig MyComponent::OnUpdate(
	UInt8 flags,
	const Data::PropertySet & properties,
	const StreamsView & streams,
	const RtConfig * previous)
{
	auto processor = New<MyProcessor>(properties, streams);

	return
	{
		.processor = processor,
		.callback = &MyComponent::ProcessRt,
		.priority = kProcessOrder
	};
}
```

Return an empty configuration when the Component has no active processing callback:

```cpp
return {};
```

`previous` is:

- `nullptr` when the Component has no previous configuration snapshot;
- otherwise a borrowed view of the previous main-thread `RtConfig`, valid for the duration of `OnUpdate`;
- safe to return by value when the previous configuration can be reused.

For example:

```cpp
if (flags & kFlagsThatRequireRebuilding)
{
	auto processor = New<MyProcessor>(properties, streams);

	return { .processor = processor, .callback = &ProcessRt };
}

REFLEX_ASSERT(previous);

return *previous;
```

Do not mutate or store `previous`. Build and return a replacement configuration when state changes. Its non-retaining `processor` remains valid during the call because the previous Network snapshot retains the processor object.

Callbacks may replace `ProcessInfo::callback` with another non-null callback at runtime. `OnUpdate` should still return the initial callback for each newly compiled processor generation.

On the first `OnUpdate`, Netx always supplies the properties and connections flags together with the Component's configured update flags, regardless of its update mask. The update mask filters subsequent calls only. The former update-group-5 bit is now reserved internally for rebuilding a Module processor after Component attachment or detachment.

### Processor ownership

The Network now owns the current Module processor snapshot and each compiled Network processor independently retains its generation. Component processor entries strongly retain both the Component and its returned processor object.

It is therefore valid, although usually unnecessary, to return the owning Component or Module as `RtConfig::processor`; this no longer creates a retaining cycle. Such callbacks must not assume that the object is still attached: retired processor generations can outlive Component or Module detachment, leaving their non-retaining `module` or `network` links null.
