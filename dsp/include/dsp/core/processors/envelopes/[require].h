#pragma once

#include "../[require].h"
#include "../../functions/convert.h"
#include "../../functions/logic.h"
#include "../detail/dnc.h"




REFLEX_NS(DSP)

template <class TYPE> REFLEX_INLINE TYPE ms2inc(TYPE sr, TYPE ms)
{
	return Reciprocal(Max(ms2samples(ms, sr), Constants<TYPE>::f_one));
}

template <Precision PRECISION = kPrecisionHigh, class TYPE> REFLEX_INLINE TYPE ms2mult(TYPE sr, TYPE ms)
{
	return Exp2<PRECISION>(Constants<TYPE>::f_ms2mult / Max(ms2samples(ms, sr) - Constants<TYPE>::f_one, Constants<TYPE>::f_one));
}

REFLEX_END