# DSP — Technical Overview

DSP is the Reflex signal-processing library. It provides the reusable processing layer beneath higher-level engines such as Netx: audio and SIMD data types, sample storage and playback, oscillators, envelopes, filters, delay infrastructure, effects, control-rate utilities, FFT analysis, and sample visualisation.

The library is divided into a headless `DSPCore` library and an optional `DSPUI` library. DSPCore can be used independently in a product renderer or as the processing foundation of Netx modules.

## Processing model

DSP is designed around a consistent scalar, stereo, and four-lane SIMD programming model. The four-lane types are used both for ordinary parallel sample processing and for processing four independent voices together, matching the voice-block model used by Netx instruments.

Processors are small, composable C++ types rather than opaque effect instances. Where a processor requires derived parameters or coefficients, its parameter object exposes setup operations such as sample-rate and control-value calculation separately from the sample-processing call. Processor state remains owned by the calling engine, allowing the product to choose allocation, voice layout, update rate, and rendering granularity.

The common foundation includes:

- Mono, stereo, SIMD, and SIMD-stereo sample representations.
- Four-lane floating-point, integer, mask, selection, and per-lane operations.
- Interpolation, mixing, gain, pan, conversion, logic, saturation, and mathematical helpers.
- Scalar and SIMD forms of commonly used operations where appropriate.
- Frequency-range, complex-number, table, smoothing, and crossfading utilities.
- Processor base types and lightweight module initialisation support.

This gives native product processors a shared numerical and state model instead of requiring each instrument or effect to establish its own conventions.

## Sample storage and resource integration

`DSP::Sample` stores in-memory floating-point audio in aligned, padded channel storage suitable for interpolated and SIMD access.

- Samples carry sample rate, channel count, length, identity, status, and metadata.
- Up to eight channels are supported, with direct read and write channel access.
- Standard metadata keys cover bit depth, loop start/length, and beat count; applications can attach additional `Key32`/`Value32` metadata.
- Sample decoders are extensible through file-extension and optional magic-byte loader registration.
- Samples can be retrieved by path through the Reflex resource pool, sharing identity and lifetime with the wider product resource system.
- Creation accepts a Reflex allocator, allowing the containing engine to control sample-memory allocation.

## Four-voice sample playback

`SamplePlayerV4` processes four independent playback voices in parallel. Each SIMD lane can use its own sample pointer, source sample rate, playback range, frequency, direction, position, and loop state.

The player supports:

- Mono and stereo sources.
- Independent pitch and source/output sample-rate conversion.
- Forward and reverse playback.
- Per-voice start/end ranges and position reporting.
- Regular and ping-pong loop traversal.
- Loop-boundary crossfades and end-of-range fades.
- Releasing an active loop so playback continues towards the sample boundary.
- Four-point real-time interpolation, with linear interpolation used for loop crossfades.
- A Vintage processing mode with configurable time/amplitude quantisation and optional interpolation/DAC-style reconstruction behaviour.

This is a low-level player rather than a complete sampler voice: envelopes, modulation, voice allocation, filters, and product-specific sample-selection policy are composed around it by the containing engine.

## Reusable processors

DSPCore includes the following processor families:

| Area | Included capabilities |
| --- | --- |
| Envelopes and control | AR and AHDSR envelopes, envelope following, LFOs, smoothing, smoother banks, crossfading, peak detection, and tables. |
| Oscillation and modulation | Phase/oscillator functions, noise generation, amplitude modulation, frequency modulation, and control/math helpers. |
| Filters | FIR, linear state-variable, state-variable, ladder, two-pole peak, and one-pole all-pass filters. |
| Delay and diffusion | Shared delay storage/management, delay processors and parameter helpers, unit delay, and diffusion stages. |
| Non-linear and lo-fi processing | Saturation, sample-rate reduction/resampling effects, and bit-depth reduction. |
| Analysis | FFT plans and windows, complex spectral data, amplitude/phase conversion, and peak analysis. |

Most templated processors can operate on scalar or four-lane state, and stereo composition is supplied where the processor semantics permit it. The collection is intended to be assembled into product-specific instruments and effects rather than presented as a fixed set of end-user processors.

## Oversampling

DSP includes a template wrapper that runs a compatible processor at a multiplied internal sample rate and performs lightweight interpolation/averaging at the wrapper boundary. The current wrapper provides 2x processing for the mono and stereo input-processing paths, and 2x or 4x operation for compatible generator-style processors.

This is a compositional utility, not a claim of a general-purpose, configurable-bandwidth resampling system. A product that requires a particular anti-aliasing response can provide a more specialised oversampling stage while continuing to use the wrapped DSP processor.

## FFT and spectral processing

DSP supplies reusable FFT planning and windowing for analysis and frequency-domain processing.

- Forward and inverse transforms for real and complex buffers.
- Reusable FFT plans and window objects.
- Scalar-to-complex and in-place window application.
- Complex packing/unpacking helpers for four-lane processing.
- Rectangular-to-amplitude/phase conversion.
- Scalar and SIMD phase-normalisation helpers.

These facilities provide the common analysis layer used by spectrogram generation and can also support product-specific spectral analysis or effects. They do not constitute a complete spectral editor or effect by themselves.

## Visualisation support

The optional `DSPUI` library builds reusable visualisation facilities above DSPCore data:

- Fixed-resolution peak-table generation for static samples and writable live buffers.
- Asynchronous spectrogram acquisition and amplitude queries.
- Waveform/sample display with channel selection, zoom, region display, trim behaviour, overlays, and resource-pool file selection.
- Reusable overlay and multi-graphic helpers.

Keeping this layer separate allows the same samples and analysis results to be used by headless rendering, a product-specific editor, or the supplied Reflex/GLX UI components.

## Integration boundary

DSP supplies processing primitives, numerical types, sample representation, and optional visualisation. It does not impose a graph, preset format, automation model, voice-allocation policy, or host lifecycle. Those responsibilities can be provided by Netx or by a licensee's own engine.

When used with Netx, the four-lane processors align with Netx voice blocks, samples participate in the shared Reflex resource pool, and DSP processors can be embedded directly in compiled Netx component processors and per-voice effect types.

## Delivery and platforms

DSP is supplied as C++20 static libraries: `DSPCore` for headless signal processing and `DSPUI` for Reflex/GLX visualisation support. Build definitions and packaged-library paths are provided for Windows, Linux, macOS, iOS, and Android.

Exact operating-system versions, architectures, toolchains, optional FFT back ends, and tested configurations should be recorded in the accompanying delivery platform matrix.
