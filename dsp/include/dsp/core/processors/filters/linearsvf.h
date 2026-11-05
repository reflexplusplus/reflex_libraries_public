#pragma once

#include "[require].h"




//
//(@doc:group=Filter)

REFLEX_NS(DSP::Filters)

template <class TYPE> class AbstractLinearSvf;

REFLEX_END




//
//svf

template <class TYPE>
class DSP::Filters::AbstractLinearSvf
{
public:

	DSP_PROCESSOR(TYPE);



	//lifetime

	AbstractLinearSvf();



	//reset

	void Reset();

	void Reset(UInt idx);



	//process

	Tuple <TYPE,TYPE,TYPE> Process(const Parameters & parameters, const TYPE & input);

	static Tuple < Complex <FloatV4>, Complex <FloatV4>, Complex <FloatV4> > Plot(const Parameters & params, const FloatV4 & f) //freq in, amp out
	{
		//typedef Complex <Float> Complex;

		//REFLEX_LOOP(idx, 4)
		//{
		//	Complex s = { 0.0f, f[idx] / params.m_frequency[idx] };

		//	Complex s2 = { Square(s.real) - Square(s.imag), (s.real * s.imag) * 2.0f };

		//	auto t0 = (s2 + Complex(1.0f));
		//	auto t1 = (s * params.m_2r[idx]);

		//	Complex lp = (Complex(1.0f) / (t0 + t1));
		//	Complex bp = (lp * s);
		//	Complex hp = (bp * s);

		//	rtn.a.real[idx] = lp.real;
		//	rtn.a.imag[idx] = lp.imag;
		//	rtn.b.real[idx] = bp.real;
		//	rtn.b.imag[idx] = bp.imag;
		//	rtn.c.real[idx] = hp.real;
		//	rtn.c.imag[idx] = hp.imag;

		//	//return { lp, bp, hp };
		//}

		//return rtn;

		typedef Complex <FloatV4> Complex;

		Complex s = { Constants::f_zero, f / params.m_frequency };

		Complex s2 = { Square(s.real) - Square(s.imag), (s.real * s.imag) * Constants::f_two };

		auto t0 = (s2 + kOne);
		auto t1 = (s * params.m_2r);

		auto lp = (kOne / (t0 + t1));
		auto bp = (lp * s);
		auto hp = (bp * s);

		return { lp, bp, hp };
	}


private:

	TYPE m_bp_z, m_lp_z;

	static inline const DSP::Complex <FloatV4> kOne = { Constants::f_one };

};




//
//parameters

template <class TYPE>
class DSP::Filters::AbstractLinearSvf<TYPE>::Parameters
{
public:

	//lifetime

	Parameters();



	//setup

	void SetSampleRate(Float sr);



	//values

protected:

	friend AbstractLinearSvf;

	void SetFrequency(const TYPE & frequency);

	void SetFrequency(UInt idx, Float frequency);

	void Set2r(const TYPE & _2r)
	{
		m_2r = _2r;

		auto t0 = m_wdiv2 * m_wdiv2;

		auto t1 = m_wdiv2 * _2r;

		auto t2 = t0 + t1;

		m_g = t2 * Constants::f_minus_one;

		m_1div1g = Reciprocal(Constants::f_one - m_g);
	}

	static void ResTo2r(const TYPE & resonance)
	{
		TYPE f_one = Constants::f_one;

		TYPE t0 = f_one - resonance;

		return t0 + t0;
	}

	void CalculateWdiv2();


	TYPE m_1_div_sr_pi;

	TYPE m_frequency;

	TYPE m_wdiv2, m_2r, m_g, m_1div1g;


	inline static const TYPE kMax = 21100.0f;

};




//
//parameters implementation

template <class TYPE> inline DSP::Filters::AbstractLinearSvf<TYPE>::Parameters::Parameters()
	: m_1_div_sr_pi(1.0f)
	, m_wdiv2(0.0f)
	, m_2r(0.0f)
	, m_g(0.0f)
	, m_1div1g(0.0f)
{
	SetFrequency(1.0f);
}

template <class TYPE> inline void DSP::Filters::AbstractLinearSvf<TYPE>::Parameters::SetSampleRate(Float sr)
{
	m_1_div_sr_pi = (1.0f / sr) * Constants::f_pi;

	CalculateWdiv2();
}

template <class TYPE> REFLEX_INLINE void DSP::Filters::AbstractLinearSvf<TYPE>::Parameters::SetFrequency(const TYPE & frequency)
{
	m_frequency = Clip(frequency, Constants::f_one, kMax);

	CalculateWdiv2();
}

template <class TYPE> REFLEX_INLINE void DSP::Filters::AbstractLinearSvf<TYPE>::Parameters::SetFrequency(UInt idx, Float frequency)
{
	m_frequency[idx] = Clip(frequency, 1.0f, 21100.0f);

	CalculateWdiv2();
}

template <class TYPE> REFLEX_INLINE void DSP::Filters::AbstractLinearSvf<TYPE>::Parameters::CalculateWdiv2()
{
	TYPE f = m_frequency * m_1_div_sr_pi;

	if constexpr (true)	//approx
	{
		TYPE t0 = Square(f);
		TYPE t1 = t0 * f;
		TYPE t3 = t0 * t1;
		TYPE t4 = t3 * Constants::f_rcp_sevenpointfive;
		TYPE t5 = t1 * Constants::f_rcp_three;
		TYPE t6 = t4 + t5;

		m_wdiv2 = f + t6;
	}
	else
	{
		auto t0 = (f * -0.0896638f) + 0.0388452f;

		auto t1 = (f * -0.430871f) + 0.0404318f;

		auto t2 = (t0 * f) + 1.00005f;

		auto t3 = (t1 * f) + Constants::f_one;

		m_wdiv2 = (t2 * f) / t3;
	}
}




//
//implementation

template <class TYPE> REFLEX_INLINE DSP::Filters::AbstractLinearSvf<TYPE>::AbstractLinearSvf()
{
	Reset();
}

template <class TYPE> REFLEX_INLINE void DSP::Filters::AbstractLinearSvf<TYPE>::Reset()
{
	auto zero = Constants::f_zero;

	m_bp_z = zero;
	m_lp_z = zero;
}

template <class TYPE> REFLEX_INLINE void DSP::Filters::AbstractLinearSvf<TYPE>::Reset(UInt vidx)
{
	m_bp_z[vidx] = 0.0f;
	m_lp_z[vidx] = 0.0f;
}

template <class TYPE> REFLEX_INLINE Tuple <TYPE,TYPE,TYPE> DSP::Filters::AbstractLinearSvf<TYPE>::Process(const Parameters & parameters, const TYPE & in)
{
	TYPE wdiv2 = parameters.m_wdiv2;

	auto t0 = in * parameters.m_g;

	auto t1 = m_bp_z * wdiv2;

	auto t2 = m_lp_z + t1;

	auto t3 = m_bp_z * parameters.m_2r;

	auto t4 = t2 + t3;

	auto t5 = t4 * Constants::f_minus_one;

	auto t6 = t0 + t5;

	auto t7 = t6 * parameters.m_1div1g;

	TYPE hp = in + t7;

	auto t8 = hp * wdiv2;

	auto bp = t8 + m_bp_z;


	auto t9 = bp * wdiv2;

	auto lp = t9 + m_lp_z;


	m_bp_z = Detail::DNC(t8 + bp);

	m_lp_z = Detail::DNC(t9 + lp);


	return { lp, bp, hp };
}


