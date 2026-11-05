#pragma once

#include "../defines.h"




//
//(@doc:group=Math)

REFLEX_NS(DSP)

bool Not(bool a);

BoolV4 Not(const BoolV4 & a);

bool Or(bool a, bool b);

BoolV4 Or(const BoolV4 & a, const BoolV4 & b);

REFLEX_END




//
//(@doc:impl)

REFLEX_INLINE bool DSP::Not(bool a)
{
	return !a;
}

REFLEX_INLINE BoolV4 DSP::Not(const BoolV4 & a)
{
	return SIMD::Not(a);
}

REFLEX_INLINE bool DSP::Or(bool a, bool b)
{
	return a || b;
}

REFLEX_INLINE BoolV4 DSP::Or(const BoolV4 & a, const BoolV4 & b)
{
	return SIMD::Or(a, b);
}
