#pragma once

#include "[require].h"
#include "../detail/unitdelay.h"




//
//declarations

REFLEX_NS(DSP::Filters)

template <class TYPE> class AllPass_1P;

REFLEX_END




//
//AllPass_1P

template <class TYPE>
class DSP::Filters::AllPass_1P
{
public:

	//declarations
		
	typedef Detail::Constants <TYPE> Constants;

	class Parameters;


	
	//lifetime

	AllPass_1P();



	//process

	void Process(const Parameters & parameters, TYPE & value, TYPE & z);

	void Process(const Parameters & parameters, Stereo <TYPE> & value, Stereo <TYPE>  & z);



private:

	Detail::UnitDelay <TYPE,true> m_z_left, m_z_right;
	
};




//
//parameters

template <class TYPE>
class DSP::Filters::AllPass_1P<TYPE>::Parameters
{
public:

	//lifetime

	Parameters();



	//setup
	
	void SetSampleRate(Float sr);

	
	
	//values

	void Set(const TYPE & coef);

	void Set(UInt vidx, Float coef);



private:

	friend class AllPass_1P;


	TYPE m_sr_rcp;
	
	TYPE m_coef;

	static const TYPE st_fifteen;

};




//
//implementation

template <class TYPE> REFLEX_INLINE DSP::Filters::AllPass_1P<TYPE>::AllPass_1P()
{
}

template <class TYPE> REFLEX_INLINE void DSP::Filters::AllPass_1P<TYPE>::Process(const Parameters & parameters, TYPE & value, TYPE & z)
{
	TYPE fb = parameters.m_coef * (value - m_z_left.GetValue());

	value = z + fb;

	z = value;

	m_z_left.Process(z);
}

template <class TYPE> REFLEX_INLINE void DSP::Filters::AllPass_1P<TYPE>::Process(const Parameters & parameters, Stereo <TYPE> & value, Stereo <TYPE> & z)
{
	TYPE fbl = parameters.m_coef * (value.l - m_z_left.GetValue());

	value.l = z.l + fbl;

	z.l = value.l;

	m_z_left.Process(z.l);


	TYPE fbr = parameters.m_coef * (value.r - m_z_right.GetValue());

	value.r = z.r + fbr;

	z.r = value.r;

	m_z_right.Process(z.r);
}




//
//parameters implementation

template <class TYPE> const TYPE DSP::Filters::AllPass_1P<TYPE>::Parameters::st_fifteen(15.0f);

template <class TYPE> inline DSP::Filters::AllPass_1P<TYPE>::Parameters::Parameters()
	: m_sr_rcp(1.0f)
{
	Set(1.0f);
}

template <class TYPE> inline void DSP::Filters::AllPass_1P<TYPE>::Parameters::SetSampleRate(Float sr)
{
	m_sr_rcp = Reciprocal(sr);
}

template <class TYPE> REFLEX_INLINE void DSP::Filters::AllPass_1P<TYPE>::Parameters::Set(const TYPE & coef)
{
	TYPE temp0 = Constants::f_pi * st_fifteen;

	TYPE temp1 = temp0 * m_sr_rcp * coef;

	TYPE temp2 = (Constants::f_one - temp1) / (Constants::f_one + temp1);

	m_coef = -temp2;
}

template <class TYPE> REFLEX_INLINE void DSP::Filters::AllPass_1P<TYPE>::Parameters::Set(UInt vidx, Float coef)
{
	Float temp0 = DSP::Detail::Constants<Float32>::f_pi * 15.0f;

	Float temp1 = temp0 * m_sr_rcp * coef;

	Float temp2 = (1.0f - temp1) / (1.0f + temp1);

	m_coef = -temp2;
}


