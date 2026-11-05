#pragma once

#include "detail/constants.h"




//
//(@doc:group=Math)

#define DSP_SET_LANE(obj, PROPERTY, vidx, value) { auto v4 = obj.Get##PROPERTY(); v4[vidx] = value; obj.Set##PROPERTY(v4); }

REFLEX_NS(DSP)

using State = Reflex::ChangeCount <false, true>;
using StateMt = Reflex::ChangeCount <true, true>;	//TRANSITIONAL

using Value32 = Bootstrap::Value32;

template <class SAMPLE> using Constants = typename Detail::Constants <SAMPLE>;

template <class SAMPLE> using BoolType = typename Detail::Constants<SAMPLE>::Bool;

template <class SAMPLE> using IntType = typename Detail::Constants<SAMPLE>::Int;

enum Precision
{
	kPrecisionFast,
	kPrecisionHigh,
};
	
enum FilterType
{
	kFilterTypeLP,
	kFilterTypeBP,
	kFilterTypeHP,
	kFilterTypeNotch,
	kNumFilterType
};

REFLEX_END
