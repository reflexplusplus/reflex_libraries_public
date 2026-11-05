#pragma once

#include "[require].h"




//
//(@doc:group=Filter)

REFLEX_NS(DSP::Filters)

template <class TYPE> class Peak2P;

REFLEX_END




//
//peak2p

template <class TYPE> 
class DSP::Filters::Peak2P
{
public:

	DSP_PROCESSOR(TYPE);



	//lifetime

	Peak2P();

	
	
	//process

	TYPE Process(const Parameters & parameters, const TYPE & in);



private:

	TYPE m_z1, m_z2, m_z3, m_z4;

};




//
//peak2p

template <class TYPE> 
class DSP::Filters::Peak2P<TYPE>::Parameters
{
public:

	//lifetime

	Parameters();



	//setup

	void SetSampleRate(Float sr);


	
	//set
	
	void Set(const TYPE & frequency, const TYPE & res, const TYPE & b);

	void Set(UInt vidx, Float frequency, Float res, Float b);



private:

	friend Peak2P;


	TYPE Sin(const TYPE & in);

	TYPE Cos(const TYPE & in);


	TYPE m_sr;

	TYPE m_p0, m_p1, m_p2;

	TYPE m_srmult;
	TYPE m_fmin;
	TYPE m_fmax;

	TYPE m_b0, m_b1, m_b2, m_a1, m_a2;

};




//
//parameters

template <class TYPE> REFLEX_INLINE DSP::Filters::Peak2P<TYPE>::Parameters::Parameters()
	: m_p0(440.0f)
	, m_p1(0.0f)
	, m_p2(0.0f)
{
	SetSampleRate(1.0f);
}

template <class TYPE> REFLEX_INLINE void DSP::Filters::Peak2P<TYPE>::Parameters::SetSampleRate(Float sr)
{
	m_sr = sr;

	m_srmult = 6.28319f / sr;

	m_fmin = sr / 24576.0f;
	m_fmax = sr / 2.125f;

	Set(m_p0, m_p1, m_p2);
}

template <class TYPE> REFLEX_INLINE void DSP::Filters::Peak2P<TYPE>::Parameters::Set(const TYPE & frequency, const TYPE & res, const TYPE & b)
{
	m_p0 = frequency;
	m_p1 = res;
	m_p2 = b;

	TYPE omega(Clip(frequency, m_fmin, m_fmax) * m_srmult);
	TYPE sin(Sin(omega));
	TYPE cos(Cos(omega));
	TYPE alfa(sin * (kfOne - res));
	TYPE a(Exp2((b / TYPE(40.0f)) * TYPE(3.12193f)));

	TYPE t0(alfa / a);
	TYPE t1(alfa * a);

	TYPE a0(kfOne / (kfOne + t0));

	m_b0 = (kfOne + t1) * a0;
	m_b1 = (cos * TYPE(-2.0f)) * a0;
	m_b2 = (kfOne - t1) * a0;

	m_a1 = Invert(m_b1);
	m_a2 = Invert((kfOne - t0) * a0);
}

template <class TYPE> REFLEX_INLINE void DSP::Filters::Peak2P<TYPE>::Parameters::Set(UInt idx, Float frequency, Float res, Float b)
{
	m_p0[idx] = frequency;
	m_p1[idx] = res;
	m_p2[idx] = b;

	Set(m_p0, m_p1, m_p2);
}

template <class TYPE> REFLEX_INLINE TYPE DSP::Filters::Peak2P<TYPE>::Parameters::Sin(const TYPE & in)
{
	TYPE t0000000C(in);

	TYPE t00000010(t0000000C * t0000000C);
	TYPE t00000014(t00000010 * TYPE(-2.39e-008f));
	TYPE t00000018(t00000014 + TYPE(2.7526e-006f));
	TYPE t0000001C(t00000010 * t00000018);
	TYPE t00000020(t0000001C + TYPE(-0.000198409f));
	TYPE t00000024(t00000010 * t00000020);
	TYPE t00000028(t00000024 + TYPE(0.00833333f));
	TYPE t0000002C(t00000010 * t00000028);
	TYPE t00000030(t0000002C + TYPE(-0.166667f));
	TYPE t00000034(t00000010 * t00000030);
	TYPE t00000038(t00000034 + kfOne);

	return t0000000C * t00000038;
}

template <class TYPE> REFLEX_INLINE TYPE DSP::Filters::Peak2P<TYPE>::Parameters::Cos(const TYPE & in)
{
	TYPE t0000000C(in);

	TYPE t00000010(t0000000C * t0000000C);
	TYPE t00000014(t00000010 * TYPE(-2.605e-007f));
	TYPE t00000018(t00000014 + TYPE(2.47609e-005f));
	TYPE t0000001C(t00000010 * t00000018);
	TYPE t00000020(t0000001C + TYPE(-0.00138884f));
	TYPE t00000024(t00000010 * t00000020);
	TYPE t00000028(t00000024 + TYPE(0.0416666f));
	TYPE t0000002C(t00000010 * t00000028);
	TYPE t00000030(t0000002C + TYPE(-0.499923f));
	TYPE t00000034(t00000010 * t00000030);

	return t00000034 + kfOne;
}




//
//implementation

template <class TYPE> REFLEX_INLINE DSP::Filters::Peak2P<TYPE>::Peak2P()
{
	m_z1 = Constants::f_zero;
	m_z2 = Constants::f_zero;
	m_z3 = Constants::f_zero;
	m_z4 = Constants::f_zero;
}

template <class TYPE> REFLEX_INLINE TYPE DSP::Filters::Peak2P<TYPE>::Process(const Parameters & parameters, const TYPE & in)
{
	TYPE t0(in * parameters.m_b0);
	TYPE t1(Detail::DNC(m_z1 * parameters.m_b1));
	TYPE t2(m_z2 * parameters.m_b2);

	m_z2 = m_z1;
	m_z1 = in;

	TYPE t3(m_z3 * parameters.m_a1);
	TYPE t4(m_z4 * parameters.m_a2);

	TYPE t5(t0 + t1 + t2 + t3 + t4);

	m_z4 = m_z3;
	m_z3 = t5;

	return t5;
}
