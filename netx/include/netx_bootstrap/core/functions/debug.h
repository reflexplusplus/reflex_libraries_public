#pragma once

#include "../[require].h"
#include <cmath>




//
//debug

namespace DSP
{

	template <class TYPE> class Stereo;

}




REFLEX_NS(Netx)

#if REFLEX_DEBUG

inline bool CheckFloat(Float value)
{
	switch (std::fpclassify(value))
	{
	case FP_NORMAL:
		return true;

	case FP_NAN:
		return false;
	case FP_INFINITE:
		return false;
	//case FP_SUBNORMAL:
	//	return false;

	default:
		return Inside(value, -10.0f, 20.0f);
	}
}

inline bool CheckFloat(const SIMD::FloatV4 & value)
{
	return CheckFloat(SIMD::Sum(value));
}

inline bool CheckFloat(const DSP::Stereo <SIMD::FloatV4> & value)
{
	return CheckFloat(reinterpret_cast<const Pair<SIMD::FloatV4>&>(value).a) && CheckFloat(reinterpret_cast<const Pair<SIMD::FloatV4>&>(value).b);
}

template <class TYPE> inline void CheckBuffer(const TYPE * buffer, UInt length)
{
	bool ok = true;

	REFLEX_LOOP_PTR(buffer, pvalue, length)
	{
		ok = ok && CheckFloat(*pvalue);
	}

	//REFLEX_ASSERT(ok);

	if (!ok) output.Error("Invalid audio buffer");
}

inline void CheckBuffer(const StereoStream & buffer)
{
	auto lr = buffer.ReadChannels();

	CheckBuffer(lr.a, buffer.length);
	CheckBuffer(lr.b, buffer.length);
}

#else

inline bool CheckFloat(Float value) { return true; }

inline bool CheckFloat(const SIMD::FloatV4 & value) { return true; }

inline bool CheckFloat(const DSP::Stereo <SIMD::FloatV4> & value) { return true; }

template <class TYPE> inline bool CheckBuffer(const TYPE * buffer, UInt length) { return true; }

#endif

REFLEX_END