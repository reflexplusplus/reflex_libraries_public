# AudioPlayer TODO

- [x] Add a public `Audio::Pattern` type derived from `Data::PropertySet`. Use it as the source of truth for editable pattern settings and keep streaming and analysis refresh with the pattern on the main thread.
- [x] Remove `CompiledPattern` and move the typed playback state into `PatternProcessorRt`. Compile that state when creating the processor on the main thread, before publishing it to the audio thread.
- [x] Add a custom property-set format based on Freestyle AudioDeck's pattern format. Register `DSP::Sample` and `Pair<Int32>`.
- [x] Use the custom format for the public `Pattern`.
- [ ] Add a dedicated timeline or tempo-map representation when that data needs to be persisted.

## Modes refactor

[Left-panel mockups for Original, Beat Sync, and Free](modes-panel-mockup.png)

- [ ] Give each pattern one playback mode and a separate stretch engine. Keep the engine choice when switching through Original.

| Mode | Playback | Timing control | Stretch engine |
| --- | --- | --- | --- |
| Original | Play the trimmed sample at native speed. Follow transport starts, seeks, and pattern restarts. | None | None active |
| Beat Sync | Map pattern beats to source frames and follow the transport tempo. | Fit | Classic, VTime, or future Slicer |
| Free | Advance through the source at a user-set rate relative to elapsed playback time. | Speed | Classic, VTime, or future Slicer |

- [ ] Keep pattern length, source trim, Loop, pattern gain, and pan independent of mode. Length sets the pattern boundary. Loop repeats the trimmed source if it ends before that boundary. Keep component master gain at the bottom of the fixed-width panel shared with the sequencer.
- [ ] Show Fit only in Beat Sync and Speed only in Free. With an eight-beat pattern at 50% Fit, one source pass takes four beats. Free at 0.75× advances three source seconds per four playback seconds, independent of host tempo.
- [ ] Keep engine-specific controls together. Classic uses native-rate voices and crossfaded position corrections. Extend it to follow either Fit or a free-speed target. VTime exposes pitch, quality, and phase lock.
- [ ] Explore Slicer using Vice transient positions. Schedule each attack once at its mapped time, play it at native rate, and shorten or extend the tail between attacks. Evaluate on drums first.
- [ ] Support future Beat Sync warp editing across engines. Persist ordered source-frame / clip-beat marker pairs on the pattern and compile them into the runtime source timeline. Slice markers identify candidate attacks a user can pin to beats. Apply Fit consistently with and without a map. Proposed Fit behavior scales marker beat positions together while preserving source frames.
- [ ] Migrate saved patterns. Audio with Sync off becomes Original. Audio with Sync on becomes Beat Sync / Classic. VTime with Sync off becomes Free / VTime. VTime with Sync on becomes Beat Sync / VTime. Preserve existing playback settings and account for mapped VTime clips that previously bypassed Fit.

## Playback optimizations

Ordered by expected impact. Confirm the ranking with profiling.

1. [ ] Reuse unchanged pattern processors when rebuilding the controller. Preserve their playback position and envelope state, and apply gain and pan edits to the active processor through an RT-safe update.
2. [ ] Refresh VTime analysis only when its inputs change. Reuse the analysis across playback-setting edits and poll only jobs that are still pending.
3. [ ] Remove synchronous logging from RT seek paths. Publish counters or other diagnostics for the main thread to read.
4. [ ] Render longer slices when ordinary ticks need no playback update. Keep seeks, wraps, pattern switches, and stops at their sample offsets. Render up to known tick offsets and retain the frame budget for tails where the next tick is unknown. Revisit the tail budget if the transport supplies explicit stop and clip-end events.
5. [ ] Add a `kWantsContinuousTicks` flag to pattern processors and combine the flags with OR when building the controller. Select a render callback that dispatches transport updates only on Restart or Jump when no pattern needs continuous ticks. In a mixed controller, check the active processor's flag. Keep clock handling for selection, wraps, playhead updates, and the frame budget.
6. [ ] Bind VTime's looping render callback directly when a slice needs no source-end handling. Keep the source-end limit and silence fill for one-shot playback.
7. [ ] Profile short loops in Audio mode. If voice restarts dominate, use the sample player's loop support while preserving the trim boundary and seek fades.
