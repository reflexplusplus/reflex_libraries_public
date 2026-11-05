# Netx — Technical Overview

Netx is the Reflex framework for building modular, real-time audio products. It provides a common model for editable product state, compiled audio processing, parameters and properties, event handling, voice management, modulation, persistence, resources, and host integration. It is used as the engine framework for products such as SamplerV3.

Netx is headless at its core. A product can place its own interface, workflow, content model, and scripting environment above the engine, while optional Bootstrap UI, instance, and VM packages provide reusable integration where required.

## Editable state and real-time processing

Netx deliberately separates the mutable object model edited by the application from the state consumed by the audio thread.

On the main thread, changes to modules, components, properties, parameters, pins, or connections cause Netx to compile a complete processor generation. Compilation resolves the active graph, stream connectivity, processing order, network properties, component callbacks, and processor-specific state before rendering begins. The completed generation is then published to the real-time processor as a single retained snapshot.

The audio thread therefore processes one internally consistent generation and does not read components while they are being edited or rebuilt. Replaced generations remain retained until real-time processing no longer uses them, so a processor callback can safely complete even if the corresponding editable component has subsequently been detached.

This architecture supports structural and parameter-driven product changes without exposing partially updated graph state to the renderer. Processor callbacks are expected to use only their compiled processor object and bound streams, and to avoid allocation, blocking, locks, and access to mutable main-thread state.

## Modular product model

Netx structures a product as a network of modules, components, pins, streams, and events.

- **Modules** are processing and routing units within a network. They own pins and organise related product functionality.
- **Components** hold independently serialisable and updateable pieces of module state. A processing component compiles its editable state into a complete replacement processor configuration for the current generation.
- **Audio, stereo, and event streams** carry rendered signals and sample-positioned performance data between modules.
- **Events** include gates/notes, controllers, clocks, and application-defined event structures.
- **Properties and parameters** provide persistent product state and automatable controls, with continuous, discrete, and Boolean definitions.

The same model can represent a focused instrument or effect, a user-editable modular environment, or the audio-engine and routing layer of a multitrack application. Tracks, buses, instruments, effects, and nested routing can be represented in the graph; timeline editing, recording, media management, and product workflow remain application-level concerns.

## Nested graphs and extensibility

Netx includes a ready-made **Group** module for constructing nested modular graphs. A Group can contain registered module classes, including other Groups, and supports module addition/removal, dynamic pins, connection validation, connection editing, enumeration, serialisation, and restoration.

Class registration and factories allow an application to define its own module types and construction arguments. Custom modules and components participate in the same graph compilation, persistence, transactions, resources, and real-time snapshot model as the supplied building blocks. Applications can also define product-specific event types and register additional stream implementations.

This makes the graph an extensible engine model rather than a fixed catalogue of processors.

## Plug-in hosting

The supplied **Plugin** module hosts **VST2, VST3, and CLAP** plug-ins where the corresponding format and platform support are available. It covers:

- Dynamic-library loading and plug-in/shell enumeration.
- Instrument/effect identification and audio/event bus configuration.
- Parameter discovery, value conversion, edit gestures, and automation transfer.
- Plug-in state chunks and restoration.
- Enable/bypass state and latency publication.
- Optional native editor-window ownership and lifecycle.

Hosted plug-ins can therefore participate in the same Netx graph and persistence model as native modules. Format SDK availability, third-party licensing requirements, and native editor support are platform-dependent and should be confirmed in the delivery platform matrix; library availability on a platform does not by itself imply that every hosted plug-in format is available there.

## Voice, modulation, and performance support

Netx supplies reusable engine components for common instrument and effect requirements.

- **VoiceManager** provides configurable polyphony and SIMD voice-block allocation, release, choke, stealing, activity, and purge lifecycle.
- **Matrix** supports registered modulation sources and destinations, static and dynamic mappings, additive and multiplicative application, bipolar sources, and per-voice SIMD processing.
- **VoiceFx** provides a registrable, configurable chain of per-voice mono or stereo processors with per-slot parameters and modulation.
- **ParameterSet** and **PropertySet** components manage dynamic controls and persistent product data.
- Mixers, external audio/event I/O, dynamic pins, clock/transport handling, MIDI utilities, sustain processing, keyboard state, host assignments, sequencing, and peak information are supplied as reusable components and utilities.

These facilities can be used selectively; they do not require a product to expose a generic modular interface to its users.

## Persistence, resources, and editing

Netx extends the real-time model into the wider product workflow.

- Modules and components have standard serialisation, versioning, reset, import, and restoration hooks.
- Transaction-backed history interfaces provide undo/redo for recorded edits.
- Standard preset and file-format components support property-sheet import and product-defined formats.
- Resource-pool integration provides shared retrieval and lifetime management for external assets.
- Monolithic preset files can embed resources such as samples alongside the serialised module state.
- Module addresses and class/component identifiers provide structured ways to locate product state.

Optional Bootstrap UI components include parameter and structure views, matrix and sequencer editors, virtual keyboards, transport controls, metering, generic controls, and diagnostic console output. Reflex VM bindings expose Netx components, streams, events, and selected UI facilities to scriptable product layers.

## Host and product integration

The Bootstrap instance package connects a Netx network and root module to Reflex audio-plug-in/application hosting. External input/output modules, host assignments, event conversion, network properties, and the processing callback form the boundary between the product graph and its Reflex host.

Netx provides the engine-side state model, graph compiler, real-time execution model, persistence infrastructure, reusable modules, and Reflex integration. A licensee normally supplies its product-specific modules and processing, user experience, content workflow, and product policy. DSPCore supplies the lower-level processing primitives used by Netx Bootstrap components and can also be used directly by product modules.

## Delivery and platforms

Netx is supplied as C++20 static libraries divided into `NetxCore`, `NetxBootstrapCore`, `NetxBootstrapUI`, `NetxBootstrapInstance`, and `NetxBootstrapVM`. This separation allows a headless engine to avoid UI and VM dependencies.

Build definitions and packaged-library paths are provided for Windows, Linux, macOS, iOS, and Android. Exact operating-system versions, architectures, toolchains, hosted plug-in formats, optional SDK dependencies, and tested configurations should be recorded in the accompanying delivery platform matrix.
