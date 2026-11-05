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

	enum TimingLock : UInt8
	{
		kLockBpm,
		kLockBeats,
	};


	// lifetime

	[[nodiscard]] static TRef <PatternView> Create(AudioPlayer & player);


	virtual void SetPattern(Key32 id) = 0;

	virtual Key32 GetPattern() const = 0;


	// display

	virtual void EnableTrim(bool enable) = 0;

	virtual bool IsTrimEnabled() const = 0;

	virtual void SetTimingLock(TimingLock lock) = 0;

	virtual TimingLock GetTimingLock() const = 0;

	// Region-relative source sample position, supplied by the owner.
	virtual void SetPlayPosition(Float64 position) = 0;

	virtual TRef <DSP::SampleDisplay> GetSampleDisplay() = 0;


	const TRef <AudioPlayer> player;


protected:

	PatternView(AudioPlayer & player);

};
