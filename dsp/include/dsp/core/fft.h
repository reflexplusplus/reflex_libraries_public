#pragma once

#include "complex.h"





//
//

REFLEX_NS(DSP::FFT)

struct Plan;

struct Window;

typedef Complex <Float> Complex;

typedef Array <Complex> Buffer;


void TimeToFrequency(const Plan & plan, const Window & window, const Float * samples, Complex * in_out);


Complex Polar(Float real, Float imag);

REFLEX_INLINE Float GetAmplitude(const Complex & polar) { return polar.real; }

REFLEX_INLINE Float GetPhase(const Complex & polar) { return polar.imag; }

Float PhaseToNormal(Float phase);

Float NormalToPhase(Float normal);


REFLEX_INLINE Pair <FloatV4> UnpackComplex(const DSP::FFT::Complex * fft, UInt block)
{
	auto pfft = Reinterpret<FloatV4>(fft + (block * 4));

	auto lower = *pfft++;		//k0.a, k0.b, k1.a, k1.b
	
	auto upper = *pfft;			//k2.a, k2.b, k3.a, k3.b

	return { Shuffle<0,2,0,2>(lower, upper), Shuffle<1,3,1,3>(lower, upper) };
}

REFLEX_INLINE void RestoreComplex(const FloatV4 & as, const FloatV4 & bs, DSP::FFT::Complex * fft, UInt block)
{
	//auto pfft = Reinterpret<FloatV4>(fft + (block * 4));

	//pfft[0] = InterleaveLo(as, bs);
	//pfft[1] = InterleaveHi(as, bs);

	auto pfft = Reinterpret<Float>(fft + (block * 4));	//performs faster on test Amd ryzen 5600

	pfft[0] = as[0];
	pfft[1] = bs[0];
	pfft[2] = as[1];
	pfft[3] = bs[1];
	pfft[4] = as[2];
	pfft[5] = bs[2];
	pfft[6] = as[3];
	pfft[7] = bs[3];
}


SIMD::FloatV4 PhaseToNormal(const SIMD::FloatV4 & phase);

SIMD::FloatV4 NormalToPhase(const SIMD::FloatV4 & phase);

REFLEX_END




//
//FFT::Plan

struct DSP::FFT::Plan : public Reflex::Object
{
public:

	Plan(UInt32 length, bool complex);

	~Plan();

	template <class TYPE> void ProcessForward(TYPE * pbuffer) const;

	template <class TYPE> void ProcessInverse(TYPE * pbuffer) const;


	const bool complex;

	const UInt length;



private:

	void * const impl;

	const FunctionPointer <long int(const void*,void*)> kComputeForward;

	const FunctionPointer <long int(const void*,void*)> kComputeBackward;

};




//
//FFT::Window

struct DSP::FFT::Window : public Reflex::Object
{
public:

	Window(const Plan & plan, bool smooth = true);


	void Apply(const Float * input, Complex * output) const;	//applies to output.a, input must be aligned

	void Apply(Float * input_output) const;

	void Apply(Complex * input_output) const;


	const Array <Float> & GetBuffer() const { return m_data; }
	

	const UInt length;

	

private:

	Array <Float> m_data;

	Array <FloatV4> m_data_scalar;

	Array <FloatV4> m_data_complex;

	Array <FloatV4> m_data_scalar2complex;

};




//
//impl

REFLEX_INLINE DSP::FFT::Complex DSP::FFT::Polar(Float real, Float imag)
{
	return {2.0f * SquareRoot(real * real + imag * imag), Atan2(imag, real)};
}

REFLEX_INLINE Float DSP::FFT::PhaseToNormal(Float phase)
{
	return (phase / k2Pif) + 0.5f;	//todo: optimise with reciprocal
}

REFLEX_INLINE Float DSP::FFT::NormalToPhase(Float phase)
{
	return (phase - 0.5f) * k2Pif;
}

REFLEX_INLINE SIMD::FloatV4 DSP::FFT::PhaseToNormal(const SIMD::FloatV4 & phase)
{
	typedef Detail::Constants <SIMD::FloatV4> Constants;

	return (phase / Constants::f_2pi) + Constants::f_rcp_two;
}

REFLEX_INLINE SIMD::FloatV4 DSP::FFT::NormalToPhase(const SIMD::FloatV4 & phase)
{
	typedef Detail::Constants <SIMD::FloatV4> Constants;

	return (phase - Constants::f_rcp_two) * Constants::f_2pi;
}

//REFLEX_NS(DSP::FFT)
//
//inline void CompareFloat(Float64 a, Float64 b)
//{
//	Float64 res = Max(Abs(a + b) / 500.0, 0.000001);
//
//	REFLEX_ASSERT(Abs(a - b) < res);
//}
//
//inline void Compare(const Complex & a, const Complex & b)
//{
//	CompareFloat(a.a, b.a);
//	CompareFloat(a.b, b.b);
//}
//
//REFLEX_END

template <class TYPE> REFLEX_INLINE void DSP::FFT::Plan::ProcessForward(TYPE * pbuffer) const
{
	REFLEX_STATIC_ASSERT((IsType<TYPE,Complex>::value) || (IsType<TYPE,Float>::value));

	if constexpr (IsType<TYPE,Complex>::value)
	{
		REFLEX_ASSERT(complex);
	}
	else if constexpr (IsType<TYPE,Float>::value)
	{
		REFLEX_ASSERT(!complex);
	}

	kComputeForward(impl, pbuffer);
}

template <class TYPE> REFLEX_INLINE void DSP::FFT::Plan::ProcessInverse(TYPE * pbuffer) const
{
	REFLEX_STATIC_ASSERT((IsType<TYPE,Complex>::value) || (IsType<TYPE,Float>::value));

	if constexpr (IsType<TYPE,Complex>::value)
	{
		REFLEX_ASSERT(complex);
	}
	else if constexpr (IsType<TYPE,Float>::value)
	{
		REFLEX_ASSERT(!complex);
	}

	kComputeBackward(impl, pbuffer);
}

REFLEX_INLINE void DSP::FFT::Window::Apply(const Float * input, Complex * output) const
{
	auto input_v4 = Reinterpret<FloatV4>(input);

	auto output_v4 = Reinterpret<FloatV4>(output);

	auto pmult_v4 = m_data_scalar2complex.GetData();

	auto end = pmult_v4 + m_data_scalar2complex.GetSize();

	while (pmult_v4 < end)
	{
		auto & value = *input_v4++;

		auto lower = Shuffle<0, 0, 1, 1>(value);

		auto upper = Shuffle<2, 2, 3, 3>(value);

		(*output_v4++) = lower * *pmult_v4++;
		(*output_v4++) = upper * *pmult_v4++;
	}

	//if (REFLEX_DEBUG) REFLEX_LOOP(idx, size) Compare(output[idx], Complex(input[idx] * m_data[idx], 0.0f));
}

REFLEX_INLINE void DSP::FFT::Window::Apply(Float * input_output) const
{
	auto pinout_v4 = Reinterpret<FloatV4>(input_output);

	auto pmult_v4 = m_data_scalar.GetData();

	auto end = pmult_v4 + m_data_scalar.GetSize();

	while (pmult_v4 < end)
	{
		(*pinout_v4) *= *pmult_v4++;

		pinout_v4++;
	}

	REFLEX_ASSERT(pinout_v4->GetData() == (input_output + (m_data_scalar.GetSize() * 4)));
}

REFLEX_INLINE void DSP::FFT::Window::Apply(Complex * input_output) const
{
	auto itr = Reinterpret<FloatV4>(input_output);

	auto pmult_v4 = m_data_complex.GetData();

	auto end = pmult_v4 + m_data_complex.GetSize();

	while (pmult_v4 < end)
	{
		*itr++ *= *pmult_v4++;
	}
}
