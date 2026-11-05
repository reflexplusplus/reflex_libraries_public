#pragma once

#include "[require].h"




//
//(@doc:group=Filter)

REFLEX_NS(DSP::Filters)

template <class TYPE> class SVF;

REFLEX_END



//
//svf

template <class TYPE>
class DSP::Filters::SVF
{
public:

	DSP_PROCESSOR(TYPE);

	
	
	//lifetime

	SVF();



	//reset

	void Reset();

	void Reset(UInt idx);



	//process

	void Process(const Parameters & parameters, const TYPE & input, TYPE * lp_bp_hp);

	template <FilterType MODE> TYPE Process(const Parameters & parameters, const TYPE & input);



private:

	TYPE m_u0_z1, m_u1_z1, m_u2_z1;

};




//
//parameters

template <class TYPE>
class DSP::Filters::SVF<TYPE>::Parameters
{
public:

	//lifetime

	Parameters();



	//setup
	
	void SetSampleRate(Float sr);

	
	
	//values

	void SetFrequency(const TYPE & frequency, const TYPE & res);

	void SetFrequency(UInt idx, Float frequency, Float res);



private:

	friend SVF;


	void Calculate();


	TYPE m_1_div_sr_pi;


	TYPE m_frequency, m_resonance, m_sat;


	TYPE m_wdiv2, m_2rw, m_g, m_1div1g; 
	
	
	TYPE m_gc;


	inline static const TYPE kMax = 21100.0f;

	inline static const TYPE kMinus3db = 0.707844f;

	inline static const TYPE kMinus6db = 0.501187f;

};




//
//parameters implementation

template <class TYPE> inline DSP::Filters::SVF<TYPE>::Parameters::Parameters()
	: m_1_div_sr_pi(1.0f)
	, m_sat(250.0f) //around 48db
{
	SetFrequency(1.0f, 0.0f);
}

template <class TYPE> inline void DSP::Filters::SVF<TYPE>::Parameters::SetSampleRate(Float sr)
{
	m_1_div_sr_pi = (1.0f / sr) * Constants::f_pi;

	Calculate();
}

template <class TYPE> REFLEX_INLINE void DSP::Filters::SVF<TYPE>::Parameters::SetFrequency(const TYPE & frequency, const TYPE & res)
{
	m_frequency = Clip(frequency, Constants::f_one, kMax);

	m_resonance = res;

	Calculate();
}

template <class TYPE> REFLEX_INLINE void DSP::Filters::SVF<TYPE>::Parameters::SetFrequency(UInt idx, Float frequency, Float res)
{
	m_frequency[idx] = Clip(frequency, 1.0f, 21100.0f);

	m_resonance[idx] = res;

	Calculate();
}

template <class TYPE> REFLEX_INLINE void DSP::Filters::SVF<TYPE>::Parameters::Calculate()
{
	//prewarp bt

	TYPE f = m_frequency * m_1_div_sr_pi;

	TYPE temp0 = Square(f);
	TYPE temp1 = temp0 * f;
	TYPE temp3 = temp0 * temp1;
	TYPE temp4 = temp3 * Constants::f_rcp_sevenpointfive;
	TYPE temp5 = temp1 * Constants::f_rcp_three;
	TYPE temp6 = temp4 + temp5;

	m_wdiv2 = f + temp6;

	
	
	//sat (temp)

	//m_sat = TYPE(250.0f);	//around 48db



	//coefficients

	TYPE f_one = Constants::f_one;


	TYPE t0 = f_one - m_resonance;

	auto t2r = t0 + t0;

	TYPE t1 = t2r + m_wdiv2;
	
	m_2rw = t1 + m_wdiv2;

	m_g = t1 * m_wdiv2;
	
	m_1div1g = Reciprocal(m_g + f_one); 
	
	
	TYPE res4 = Quartic(m_resonance);
	
	TYPE minus3db = kMinus3db;

	m_gc = ((kMinus3db - f_one) * res4) + f_one;

	//TYPE minus6db = kMinus3db;		//this was never implemented
	
	//m_gc_bp = ((kMinus3db - f_one) * res4) + f_one;
}




//
//implementation

template <class TYPE> REFLEX_INLINE DSP::Filters::SVF<TYPE>::SVF()
{
	Reset();
}

template <class TYPE> REFLEX_INLINE void DSP::Filters::SVF<TYPE>::Reset()
{
	auto temp = Constants::f_zero;

	m_u0_z1 = temp;
	m_u1_z1 = temp;
	m_u2_z1 = temp;
}

template <class TYPE> REFLEX_INLINE void DSP::Filters::SVF<TYPE>::Reset(UInt vidx)
{
	m_u0_z1[vidx] = 0.0f;
	m_u1_z1[vidx] = 0.0f;
	m_u2_z1[vidx] = 0.0f;
}

template <class TYPE> REFLEX_INLINE void DSP::Filters::SVF<TYPE>::Process(const Parameters & parameters, const TYPE & input, TYPE * lp_bp_hp)
{
	//fbksolv
	
	TYPE t0 = (m_u0_z1 * parameters.m_g) + (m_u1_z1 * parameters.m_2rw) + m_u2_z1;
	
	TYPE hp_temp = (input - t0) * parameters.m_1div1g;

	

	//u0 and u1

	TYPE wdiv2 = parameters.m_wdiv2;

	TYPE sat = parameters.m_sat;
	

	TYPE t1 = ((hp_temp + m_u0_z1) * wdiv2) + m_u1_z1;
	
	TYPE lp_temp = ((t1 + m_u1_z1) * wdiv2) + m_u2_z1;


	m_u0_z1 = Detail::DNC(hp_temp);

	m_u1_z1 = Detail::DNC(ParabolicSat(t1, sat));
	
	m_u2_z1 = Detail::DNC(ParabolicSat(lp_temp, sat));

	

	//gain correct
	
	lp_bp_hp[0] = lp_temp * parameters.m_gc;

	lp_bp_hp[1] = (t1 + t1) * parameters.m_gc;

	lp_bp_hp[2] = hp_temp * parameters.m_gc;
}

template <class TYPE> template <DSP::FilterType MODE> REFLEX_INLINE TYPE DSP::Filters::SVF<TYPE>::Process(const Parameters & parameters, const TYPE & input)
{
	//fbksolv

	TYPE t0 = (m_u0_z1 * parameters.m_g) + (m_u1_z1 * parameters.m_2rw) + m_u2_z1;

	TYPE hp_temp = (input - t0) * parameters.m_1div1g;



	//u0 and u1

	TYPE wdiv2 = parameters.m_wdiv2;

	TYPE sat = parameters.m_sat;


	TYPE t1 = ((hp_temp + m_u0_z1) * wdiv2) + m_u1_z1;

	TYPE lp_temp = ((t1 + m_u1_z1) * wdiv2) + m_u2_z1;


	m_u0_z1 = Detail::DNC(hp_temp);

	m_u1_z1 = Detail::DNC(ParabolicSat(t1, sat));

	m_u2_z1 = Detail::DNC(ParabolicSat(lp_temp, sat));


	if constexpr (MODE == kFilterTypeLP)
	{
		return lp_temp * parameters.m_gc;
	}
	else if constexpr (MODE == kFilterTypeBP)
	{
		return (t1 + t1) * parameters.m_gc;
	}
	else if constexpr (MODE == kFilterTypeHP)
	{
		return hp_temp * parameters.m_gc;
	}
	else if constexpr (MODE == kFilterTypeNotch)
	{
		return (lp_temp + hp_temp) * parameters.m_gc;
	}

	return 0.0f;
}
