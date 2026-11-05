#pragma once

#include "../detail/constants.h"
#include "../functions/convert.h"




//
//Public API

REFLEX_NS(DSP)

template <class TYPE, bool BIPOLAR = true> class Noise;

template <bool BIPOLAR> using NoiseV4 = Noise <FloatV4,BIPOLAR>;

REFLEX_END




//
//Noise

template <class TYPE, bool BIPOLAR> 
class DSP::Noise
{
public:

	//types

	typedef Detail::Constants <TYPE> Constants;

	typedef typename Constants::Int IntType;



	//lifetime

	Noise();



	//setup

	void SetSeed(const IntType & seed);



	//process

	TYPE Process();



private:

	IntType m_state;
	

	static inline const IntType st_mult = 1103515245;

	static inline const IntType st_inc = 12345;

	static inline const TYPE st_fmult = BIPOLAR ? TYPE(1.0 / 8388608.0) : TYPE(1.0 / 16777216.0); // 2^23 / 2^24

};




//
//impl

REFLEX_NS(DSP::Detail)

REFLEX_INLINE void InitSeed(Int32 & state)
{
	UInt64 t = System::GetTime();

	auto s = UInt32(t) ^ UInt32(t >> 32) ^ UInt32(ToUIntNative(&state));

	if (!s) s = 1;

	state = (Int32)s;
}

REFLEX_INLINE void InitSeed(IntV4 & state)
{
	UInt64 t = System::GetTime();
	
	auto s = UInt32(t) ^ UInt32(t >> 32) ^ UInt32(ToUIntNative(&state));
	
	if (!s) s = 1;

	auto a = Int32(s);
	auto b = Int32(UInt32(a) * 1103515245u + 12345u);
	auto c = Int32(UInt32(b) * 1103515245u + 12345u);
	auto d = Int32(UInt32(c) * 1103515245u + 12345u);

	state.Set(a, b, c, d);
}

template <UInt32 N> REFLEX_INLINE Int32 ShiftRightLogical(Int32 v)
{
	return Reinterpret<Int32>(Reinterpret<UInt32>(v) >> N);
}

template <UInt32 N> REFLEX_INLINE IntV4 ShiftRightLogical(const IntV4 & v)
{
	return REFLEX_SHR1_I32_X4(v, (int)N);
}

REFLEX_END

template <class TYPE, bool BIPOLAR> inline DSP::Noise<TYPE,BIPOLAR>::Noise()
{
	Detail::InitSeed(m_state);
}

template <class TYPE, bool BIPOLAR> REFLEX_INLINE void DSP::Noise<TYPE,BIPOLAR>::SetSeed(const IntType & seed)
{
	m_state = seed;
}

template <class TYPE, bool BIPOLAR> REFLEX_INLINE TYPE DSP::Noise<TYPE,BIPOLAR>::Process()
{
	IntType state = m_state;	//Int32 or IntV4

	state *= st_mult;

	state += st_inc;

	m_state = state;

	if constexpr (BIPOLAR)
	{
		IntType x = DSP::Detail::ShiftRightLogical<9>(state);

		TYPE f = IntToFloat(x);

		return f * st_fmult - TYPE(1.0f);
	}
	else
	{
		IntType x = DSP::Detail::ShiftRightLogical<8>(state);

		TYPE f = IntToFloat(x);

		return f * st_fmult;
	}
}
