#pragma once

#include "../defines.h"




//
//(@doc:group=Math)

REFLEX_NS(DSP)

bool Or(bool a, bool b);

BoolV4 Or(const BoolV4 & a, const BoolV4 & b);


bool Not(bool a);

BoolV4 Not(const BoolV4 & a);


template <class TYPE> TYPE Select(bool test, const TYPE & t, const TYPE & f)
{
	return test ? t : f;
}

template <class TYPE> TYPE Select(bool test, const TYPE & t)
{
	return test ? t : TYPE();
}

template <class TYPE> TypeV4 <TYPE> Select(bool test, const TypeV4 <TYPE> & t)
{
	using SingleType = typename TypeV4<TYPE>::SingleType;
	return test ? t : TypeV4<TYPE>(SingleType(0));
}

REFLEX_END




//
//impl

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
