#pragma once

#include "[require].h"




//
//declarations

REFLEX_NS(DSP::Filters)

template <class TYPE> class Ladder;

REFLEX_END



//
//svf

template <class TYPE>
class DSP::Filters::Ladder
{
public:

	DSP_PROCESSOR(TYPE);

	
	
	//lifetime

	Ladder();



	//reset

	void Reset(UInt idx);



	//process

	template <FilterType MODE> TYPE Process(const Parameters & params, const TYPE & in);



private:

	template <UInt STAGE> TYPE ProcessS(const Parameters & params, const TYPE & in, TYPE & invz);

	template <UInt STAGE> TYPE ProcessLP(const Parameters & params, const TYPE & invz, const TYPE & in);

	static TYPE Tanh(const TYPE & value);



	TYPE m_z[4], m_hpz;


	static const TYPE f_twenty, f_twenty_rcp, f_log_2_71828;

};




//
//params

template <class TYPE>
class DSP::Filters::Ladder<TYPE>::Parameters
{
public:

	//lifetime

	Parameters();



	//setup
	
	void SetSampleRate(Float sr);

	
	
	//values

	void Set(const TYPE & frequency, const TYPE & res);

	void Set(UInt idx, Float frequency, Float res);



private:

	friend Ladder;

	
	TYPE F2WDiv2(const TYPE & frequency);

	void Calculate();


	TYPE m_samplerate_rcp;


	TYPE m_frequency, m_resonance;


	TYPE m_wdiv2, m_inv_wdiv2, m_fbg, m_wdiv2_fbg;

	TYPE m_hp_wdiv2, m_hp_inv_wdiv2, m_hp_fbg, m_hp_g, m_hp_fbg2, m_hp_g2;

	TYPE m_inv_k, m_preamp[kNumFilterType];

};




//
//parameters

template <class TYPE> inline DSP::Filters::Ladder<TYPE>::Parameters::Parameters()
	: m_frequency(500.0f)
	, m_resonance(0.5f)
	, m_inv_k(-2.0f)
{
	REFLEX_LOOP (idx, kNumFilterType) m_preamp[idx] = 1.0f;

	SetSampleRate(44100.0f);

	Set(500.0f, 0.5f);
}

template <class TYPE> inline void DSP::Filters::Ladder<TYPE>::Parameters::SetSampleRate(Float sr)
{
	m_samplerate_rcp = 1.0f / sr;

	m_hp_wdiv2 = F2WDiv2(50.0f);

	m_hp_inv_wdiv2 = m_hp_wdiv2 * Constants::f_minus_one;

	m_hp_fbg = Reciprocal(Constants::f_one - m_hp_inv_wdiv2);

	Calculate();
}

template <class TYPE> REFLEX_INLINE void DSP::Filters::Ladder<TYPE>::Parameters::Set(const TYPE & frequency, const TYPE & res)
{
	m_frequency = Clip(frequency, Constants::f_one, TYPE(22000.0f));

	m_resonance = Max(res, Constants::f_zero);

	m_inv_k = m_resonance * Constants::f_minus_four;

	Calculate();
}

template <class TYPE> REFLEX_INLINE void DSP::Filters::Ladder<TYPE>::Parameters::Set(UInt vidx, Float frequency, Float res)
{
	m_frequency[vidx] = Clip(frequency, 1.0f, 22000.0f);

	res = Max(res, 0.0f);

	m_resonance[vidx] = res;

	m_inv_k[vidx] = res * -4.0f;

	Calculate();
}

template <class TYPE> REFLEX_INLINE TYPE DSP::Filters::Ladder<TYPE>::Parameters::F2WDiv2(const TYPE & frequency)
{
	TYPE temp0 = frequency * (Constants::f_pi * m_samplerate_rcp);

	temp0 = Min(temp0, FloatV4(1.50845f));

	TYPE temp1 = (temp0 * -0.0896638) + 0.0388452f;
	TYPE temp2 = (temp0 * temp1) + 1.00005f;
	TYPE temp3 = temp0 * temp2;

	TYPE temp4 = (temp0 * -0.430871f) + 0.0404318f;
	TYPE temp5 = (temp0 * temp4) + Constants::f_one;

	TYPE wdiv2 = temp3 / temp5;

	return wdiv2;
}

template <class TYPE> REFLEX_INLINE void DSP::Filters::Ladder<TYPE>::Parameters::Calculate()
{
	m_wdiv2 = F2WDiv2(m_frequency);

	m_inv_wdiv2 = m_wdiv2 * Constants::f_minus_one;

	m_fbg = Reciprocal(Constants::f_one - m_inv_wdiv2);

	m_wdiv2_fbg = m_wdiv2 * m_fbg;

	TYPE g = 1.0f;

	REFLEX_LOOP (idx, 4) g = m_fbg * g * m_wdiv2;

	m_hp_g = g;

	
	TYPE t0 = m_hp_fbg * m_hp_g;

	TYPE t1 = (t0 * m_hp_wdiv2) + t0;

	m_hp_g2 = t1 * m_inv_k;

	m_hp_fbg2 = Reciprocal(Constants::f_one - m_hp_g2);


	//gc

	TYPE gc = Min(m_resonance, TYPE(0.75f));

	m_preamp[kFilterTypeLP] = db2a<kPrecisionFast>(gc * TYPE(12.0f));

	m_preamp[kFilterTypeHP] = db2a<kPrecisionFast>(gc * TYPE(-3.0f));
}




//
//processor

template <class TYPE> const TYPE DSP::Filters::Ladder<TYPE>::f_twenty(20.0f);

template <class TYPE> const TYPE DSP::Filters::Ladder<TYPE>::f_twenty_rcp(1.0f / 20.0f);

template <class TYPE> const TYPE DSP::Filters::Ladder<TYPE>::f_log_2_71828(Log2(2.71828182846f));

template <class TYPE> REFLEX_INLINE DSP::Filters::Ladder<TYPE>::Ladder()
{
	m_z[0] = Constants::f_zero;
	m_z[1] = Constants::f_zero;
	m_z[2] = Constants::f_zero;
	m_z[3] = Constants::f_zero;

	m_hpz = Constants::f_zero;
}

template <class TYPE> REFLEX_INLINE void DSP::Filters::Ladder<TYPE>::Reset(UInt vidx)
{
	m_z[0][vidx] = 0.0f;
	m_z[1][vidx] = 0.0f;
	m_z[2][vidx] = 0.0f;
	m_z[3][vidx] = 0.0f;

	m_hpz[vidx] = 0.0f;
}

template <class TYPE> template <DSP::FilterType FILTERTYPE> REFLEX_INLINE TYPE DSP::Filters::Ladder<TYPE>::Process(const Parameters & params, const TYPE & value)
{
	TYPE in = value * params.m_preamp[FILTERTYPE];


	TYPE invz[4];

	TYPE s1 = ProcessS<0>(params, Constants::f_zero, invz[0]);

	TYPE s2 = ProcessS<1>(params, s1, invz[1]);

	TYPE s3 = ProcessS<2>(params, s2, invz[2]);

	TYPE s4 = ProcessS<3>(params, s3, invz[3]);



	//fb hp filter 
	
	TYPE t0 = m_hpz * Constants::f_minus_one;

	TYPE t1 = (t0 + s4) * params.m_hp_fbg;

	TYPE t2 = m_hpz + (t1 * params.m_hp_wdiv2);

	TYPE t3 = (t1 + t2) * params.m_inv_k;

	TYPE t4 = (((in * params.m_hp_g2) + t3) * params.m_hp_fbg2) + in;

	TYPE lp0 = Tanh(t4 * Constants::f_rcp_four) * Constants::f_four;


	
	//lp

	TYPE lp1 = ProcessLP<0>(params, invz[0], lp0);

	TYPE lp2 = ProcessLP<1>(params, invz[1], lp1);

	TYPE lp3 = ProcessLP<2>(params, invz[2], lp2);

	TYPE lp4 = ProcessLP<3>(params, invz[3], lp3);


	
	//fb 
	
	TYPE t6 = (((lp4 * params.m_hp_inv_wdiv2) + t0) * params.m_hp_fbg) + lp4;

	TYPE t7 = t6 * params.m_hp_wdiv2;

	m_hpz = m_hpz + t7 + t7;


	if constexpr (FILTERTYPE == kFilterTypeLP)
	{
		return lp4;
	}
	else
	{
		lp3 *= Constants::f_minus_four;
		lp2 *= Constants::f_six;
		lp1 *= Constants::f_minus_four;

		return lp4 + lp3 + lp2 + lp1 + lp0;
	}
}

template <class TYPE> template <UInt STAGE> REFLEX_INLINE TYPE DSP::Filters::Ladder<TYPE>::ProcessS(const Parameters & params, const TYPE & in, TYPE & invz)
{
	const TYPE & z = m_z[STAGE];

	invz = z * Constants::f_minus_one;

	return ((invz + in) * params.m_wdiv2_fbg) + z;
}

template <class TYPE> template <UInt STAGE> REFLEX_INLINE TYPE DSP::Filters::Ladder<TYPE>::ProcessLP(const Parameters & params, const TYPE & invz, const TYPE & in)
{
	TYPE & z = m_z[STAGE];

	TYPE t0 = ((((in * params.m_inv_wdiv2) + invz) * params.m_fbg) + in) * params.m_wdiv2;

	TYPE t2 = t0 + z;

	z = Detail::DNC(t0 + t2);

	return t2;
}

template <class TYPE> REFLEX_INLINE TYPE DSP::Filters::Ladder<TYPE>::Tanh(const TYPE & value)
{
	TYPE c = Sign(value);


	TYPE b0 = value + value;

	TYPE b1 = Exp2(f_log_2_71828 * b0);	//2.71828182846

	TYPE b2 = b1 - Constants::f_one;

	TYPE b3 = b1 + Constants::f_one;

	TYPE b = b2 / b3;


	TYPE a0 = Cube(value);

	TYPE a1 = a0 * Constants::f_rcp_three;

	TYPE a = value - a1;


	TYPE abs = Abs(value);

	return Select(abs > f_twenty, c, Select(abs > f_twenty_rcp, b, a));
}


