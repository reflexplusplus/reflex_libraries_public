#pragma once

#include "../../[require].h"




//
//

REFLEX_NS(DSP::Detail)

Float DNC(Float value);
	
FloatV4 DNC(const FloatV4 & value);

REFLEX_END




//
//impl

REFLEX_NS(DSP::Detail)

extern const FloatV4 kVecDNC;

REFLEX_END

REFLEX_INLINE Float DSP::Detail::DNC(Float value)
{
	//return (value + 1e-024f) - 1e-024f;

	return value;	//assume REFLEX_ENABLE_FTZ
}

REFLEX_INLINE FloatV4 DSP::Detail::DNC(const FloatV4 & value)
{
	//return (value + kVecDNC) - kVecDNC;

	return value;	//assume REFLEX_ENABLE_FTZ
}
