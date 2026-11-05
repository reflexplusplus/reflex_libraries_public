#pragma once

#include "../stereo.h"




//
//(@doc:group=Audio)

REFLEX_NS(DSP)

template <class TYPE> Stereo <TYPE> Pan(const TYPE & pan);

template <class TYPE> Stereo <TYPE> Mix(const TYPE & amp, const TYPE & pan);

REFLEX_END


		

//
//implementation

template <class TYPE> REFLEX_INLINE DSP::Stereo <TYPE> DSP::Pan(const TYPE & pan)
{
	typedef Detail::Constants <TYPE> Constants;

	auto left = (Constants::f_one - pan);

	auto right = (Constants::f_one + pan);

	return { ((Constants::f_four - left) * left * Constants::f_rcp_three), (Constants::f_four - right) * right * Constants::f_rcp_three };
}

template <class TYPE> REFLEX_INLINE DSP::Stereo <TYPE> DSP::Mix(const TYPE & amp, const TYPE & pan)
{
	return Pan(pan) * amp;
}
