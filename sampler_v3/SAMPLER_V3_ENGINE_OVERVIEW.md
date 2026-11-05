# SamplerV3 Engine — Technical Summary

SamplerV3 is a headless C++20, real-time sample-instrument engine. It has no UI dependency and is intended to be embedded behind a product’s own interface, workflow, and host integration. Its architecture is based on an **Instrument → Group → Zone** hierarchy: an instrument owns a configurable voice manager and a collection of groups; every group carries its own engine, dynamic properties, modulation, polyphonic effects, and routing; each zone supplies a sample and per-sample playback configuration.

## Netx and Reflex foundation

SamplerV3 is built on **Netx**, the Reflex audio framework. It follows Netx’s model of separate editable/main-thread state and compiled real-time processor state. Changes to the instrument model are compiled into processor, group, and zone references and transferred lock-free to the audio engine. This architecture provides thread-safe editing while audio is running, without exposing mutable authoring objects to the real-time voice path.

The processor-compilation step also resolves the active engine, sample and zone data, routing, parameter allocations, effects, modulation, and render-function variants ahead of processing. This keeps the audio path focused on the work required for active voices and supports efficient SIMD-oriented rendering.

SamplerV3 includes a native instrument file format and serialisation layer. It supports normal resource-pool sample references as well as monolithic instrument files with embedded samples, allowing a complete instrument and its source audio to be stored and moved as one asset. The same editable model includes transaction-backed undo/redo, so hosts can provide reversible instrument editing.

## Core architecture and capabilities

### Groups, zones, and dynamic properties

SamplerV3’s primary unit of construction is the group. Groups contain zones and can be combined freely to create layered, split, or hybrid instruments. A zone references a sample and defines its playback treatment; the group provides the shared sound engine, controls, modulation, effects, and audio routing for that layer.

Properties are dynamic rather than being fixed to one instrument design. Engine-specific properties and parameter definitions are exposed by the selected group engine, while the host can add its own custom parameters and polyphonic modulation inputs through `SamplerV3::Config`. This allows a product to present a conventional mapped sampler, such as a Nuance-style instrument, or a more specialised time-based slicer or processor, such as a Vice-style instrument, using the same core engine architecture.

### Polyphonic effects and modulation

Each group has an expandable polyphonic `Netx::VoiceFx` processing chain. The engine exposes a registration hook for an integrator’s own voice-effect map, allowing effect slots and effect types to be selected and configured by the host product.

Each group also includes a modulation matrix with no fixed connection-count limit imposed by the SamplerV3 group model. Sources can be routed to group parameters, making the same instrument definition suitable for simple performance control or complex sound design. The voice processor applies modulation during real-time rendering, including an audio-rate modulation path for continuously varying per-voice parameters.

### Selectable playback engines

Every group operates in one of three currently supported engine modes: **Standard**, **Granular**, or **VTime**. An instrument may use different modes in different groups, for example combining conventional sample layers with granular or time-domain layers.

### Built-in editing history

SamplerV3 includes transaction-backed undo/redo support for its editable instrument model. Built-in transactions cover group and zone creation/removal, zone edits, group ordering, instrument output-count changes, and property changes. This enables a host editor to provide reliable reversible editing without reconstructing instrument state manually.

## Playback engines

SamplerV3 provides the following playback engines per group.

| Engine | Functionality |
| --- | --- |
| Standard | Conventional mono/stereo sample playback using the DSP sample player. Supports pitch playback, reverse triggering, trim regions, looping, release loops, sample-end fades, and loop crossfades. |
| VTime | Time-domain playback through VTime analysis and player objects. Pitch/frequency and speed are independently controlled. The group exposes quality selection (`Draft`, `Standard`, or `High`), a selectable speed range, and spectral-coherence/phase-lock control. |
| Granular | Stereo granular playback with two grain states per voice. Group parameters include speed, grain size (1–500 ms), and stereo width; engine properties include jitter and key tracking. Granular playback supports forward/reverse playback, trim boundaries, looping, and fade-out handling. |

## Sample and zone handling

Each zone references a `DSP::Sample` and stores persistent playback data.

- Mono and stereo source samples.
- Independent trim and loop regions, represented as start/length pairs.
- Loop enable, loop direction/flip, and loop-during-release options.
- Independent fade-out and loop-crossfade lengths.
- Direct zone triggering, reverse triggering, release, and choke control.
- Playback-position reporting for active voices.
- Serialisation/deserialisation of sample paths, sample properties, and metadata through the resource pool.

Zone data is compiled into immutable real-time `ZoneRef` structures. These contain the resolved sample, engine-specific precomputed data, regions, loop state, fades, and voice-processing flags, avoiding mutable zone access from the audio path.

## Voice processing and real-time design

The processor is built on the Netx voice-manager abstraction. Voices are processed in SIMD-oriented blocks of four, represented by `VoiceBlock` and engine-specific `VoiceBlockX` instances.

- Separate mono and stereo render paths.
- Render functions are selected for the active combination of sample format, fade state, loop mode, effects, envelopes, modulation, and engine.
- Voice lifecycle operations include create, release, choke, release by packet, zone, group, or voice block, and global panic.
- Glide is provided at the voice level.
- Editable instrument state is compiled into processor/group/zone references before use on the real-time path.
- Default maximum polyphony is 256 voices; the default operating polyphony is 32, both configurable through `SamplerV3::Config`.

## Modulation and control sources

Every group has a dynamic Netx parameter set and modulation matrix. The default configuration provides the following sources and processors:

- Amplitude AHDSR envelope, with configurable sustain mode.
- Three modulation envelopes, each with optional sustain behaviour.
- Three LFOs, with free-running, tempo-synchronised, or phase-synchronised modes.
- Three table modulators, each with a retrigger option and configurable rate.
- Optional XYZ performance-pad parameters.
- Smoothed MIDI pitch bend, CC1, and channel pressure sources.
- Note-derived and host-defined polyphonic modulation inputs.
- A routable modulation matrix for connecting sources to group parameters, including an audio-rate processing path.

The number of modulation envelopes, LFOs, and tables is set by the engine configuration, rather than being hard-coded into an instrument format. The configuration also permits host-defined custom parameters and polyphonic modulation sources.

## Mixing, effects, and routing

Groups are independently mixed and routed.

- Per-group gain and pan.
- Two per-group auxiliary sends.
- Configurable main output per group.
- Optional auxiliary-output enablement and independent output assignment for each aux send.
- Per-group `Netx::VoiceFx` processing.
- Effect registration hook, allowing an integrating product to supply its own voice-effect map.

The instrument exposes configurable output count, and routing is compiled with the group for use in the processor.

## Integration API and persistence

The public API supports programmatic instrument editing and runtime control:

- Create, discard, clear, and sort groups and zones.
- Assign samples and configure each zone’s regions, loop mode, and fades.
- Set group engine, label, output routing, envelope/LFO/table options, and parameter values.
- Play/release MIDI notes or individual zones; query keyboard state; panic all voices.
- Supply client state and custom compile-time mapping callbacks.

Instrument, group, and zone data can be serialised with metadata. The property-sheet import path can restore instrument polyphony, groups, zones, parameter values, effect-slot selections, modulation-matrix connections, and sample search paths. Editing operations are transaction-backed, including group/zone addition and removal, zone edits, group ordering, output count, and property changes, enabling host undo/redo implementations.

## Build targets and dependencies

SamplerV3 is supplied as a static library, with CMake and native project definitions for Windows, Linux, macOS, iOS, and Android. It depends on the Reflex runtime and the companion Netx, DSP, and VTime libraries. The Android project includes arm64-v8a and x86_64 build support.
