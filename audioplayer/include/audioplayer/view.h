#pragma once

#include "engine.h"
#include "netx_bootstrap/ui.h"
#include "dsp/ui/sampledisplay.h"




//
// declarations

namespace Audio
{
	class PatternView;

}




//
// PatternView

class Audio::PatternView : public Netx::Detail::ComponentView
{
public:

	REFLEX_OBJECT(PatternView, Netx::Detail::ComponentView);

	struct SourceTiming
	{
		Float32 bpm = 0.0f;
		Pair <UInt> region = {};
	};

	struct Host
	{
		// Returns a finite, positive source BPM.
		Function <SourceTiming(const DSP::Sample &)> read_timing;
		Function <void(const DSP::Sample &, const SourceTiming &)> store_timing;
		Function <Float64()> get_track_bps;
		Function <void(UInt32)> seek;
	};

	// lifetime

	[[nodiscard]] static Unretained <PatternView> Create(AudioPlayer & player, Host host);


	virtual void SetPattern(Key32 id) = 0;

	virtual Key32 GetPattern() const = 0;


	// display

	virtual void EnableTrim(bool enable) = 0;

	virtual bool IsTrimEnabled() const = 0;

	// Region-relative source sample position, supplied by the owner.
	virtual void SetPlayPosition(Float64 position) = 0;

	virtual AlreadyRetained <DSP::SampleDisplay> GetSampleDisplay() = 0;
	
	virtual AlreadyRetained <GLX::Object> GetHeaderClientArea() = 0;


	const AlreadyRetained <AudioPlayer> player;


protected:

	PatternView(AudioPlayer & player);

};
