#pragma once

#include "[require].h"




//
//(@doc:group=Filter)

REFLEX_NS(DSP::Filters)

template <class SAMPLE_T, UInt NPOLE> class Cascaded1P;

template <class SAMPLE_T> using FIR_1P = Cascaded1P <SAMPLE_T,1>;

REFLEX_END




//
//6db eq

template <class SAMPLE_T, UInt NPOLE>
class DSP::Filters::Cascaded1P
{
public:

	DSP_PROCESSOR(SAMPLE_T);

	
	
	//lifetime

	Cascaded1P();



	//process

	SAMPLE_T ProcessLP(const Parameters & parameters, const SAMPLE_T & in);

	SAMPLE_T ProcessHP(const Parameters & parameters, const SAMPLE_T & in);

	Pair <SAMPLE_T> ProcessSplit(const Parameters & parameters, const SAMPLE_T & in);



private:

	SAMPLE_T m_z[NPOLE];

};




//
//parameters

template <class SAMPLE_T, UInt NPOLE>
class DSP::Filters::Cascaded1P<SAMPLE_T,NPOLE>::Parameters
{
public:

	//lifetime

	Parameters();



	//setup
	
	void SetSampleRate(Float sr);

	
	
	//calculate
	
	static SAMPLE_T CalculateLoShelf(SAMPLE_T & frequency, const SAMPLE_T & db);

	static SAMPLE_T CalculateHiShelf(SAMPLE_T & frequency, const SAMPLE_T & db);
	
	
	
	//values

	void SetFrequency(const SAMPLE_T & frequency);

	void SetFrequency(UInt idx, Float frequency);



private:

	friend class Cascaded1P;


	void Calculate();


	SAMPLE_T m_frequency_mult;

	SAMPLE_T m_frequency;
	
	SAMPLE_T m_w, m_1diva0, m_a1;


	static const SAMPLE_T log_1_05925_mult;

};





//
//parameters

template <class SAMPLE_T, UInt NPOLE> const SAMPLE_T DSP::Filters::Cascaded1P<SAMPLE_T,NPOLE>::Parameters::log_1_05925_mult = DSP::Log2(1.05925f);

template <class SAMPLE_T, UInt NPOLE> inline DSP::Filters::Cascaded1P<SAMPLE_T,NPOLE>::Parameters::Parameters()
	: m_frequency_mult(1.0f)
	, m_frequency(1.0f)
	, m_w(0.0f)
	, m_1diva0(0.0f)
	, m_a1(0.0f)
{
}

template <class SAMPLE_T, UInt NPOLE> inline void DSP::Filters::Cascaded1P<SAMPLE_T,NPOLE>::Parameters::SetSampleRate(Float sr)
{
	m_frequency_mult = DSP::Detail::Constants<Float32>::f_pi / sr;

	Calculate();
}

template <class SAMPLE_T, UInt NPOLE> REFLEX_INLINE SAMPLE_T DSP::Filters::Cascaded1P<SAMPLE_T,NPOLE>::Parameters::CalculateHiShelf(SAMPLE_T & frequency, const SAMPLE_T & db)
{
	SAMPLE_T expdb = Exp2<kPrecisionFast>(db * log_1_05925_mult);

	frequency *= expdb;

	return Square(expdb) - Constants::f_one;
}

template <class SAMPLE_T, UInt NPOLE> REFLEX_INLINE SAMPLE_T DSP::Filters::Cascaded1P<SAMPLE_T,NPOLE>::Parameters::CalculateLoShelf(SAMPLE_T & frequency, const SAMPLE_T & db)
{
	SAMPLE_T expdb = Exp2<kPrecisionFast>(db * log_1_05925_mult);

	frequency /= expdb;

	return Square(expdb) - Constants::f_one;
}

template <class SAMPLE_T, UInt NPOLE> REFLEX_INLINE void DSP::Filters::Cascaded1P<SAMPLE_T,NPOLE>::Parameters::SetFrequency(const SAMPLE_T & frequency)
{
	m_frequency = frequency;

	Calculate();
}

template <class SAMPLE_T, UInt NPOLE> REFLEX_INLINE void DSP::Filters::Cascaded1P<SAMPLE_T,NPOLE>::Parameters::Calculate()
{
	SAMPLE_T f = m_frequency * m_frequency_mult;

	SAMPLE_T temp0 = Square(f);
	SAMPLE_T temp1 = temp0 * f;
	SAMPLE_T temp3 = temp0 * temp1;
	SAMPLE_T temp4 = temp3 * Constants::f_rcp_sevenpointfive;
	SAMPLE_T temp5 = temp1 * Constants::f_rcp_three;
	SAMPLE_T temp6 = temp4 + temp5;
	SAMPLE_T w = f + temp6;
	//TODO SAMPLE_T w = Tan<kPrecisionFast>(f);

	SAMPLE_T f_one = Constants::f_one;

	m_1diva0 = f_one / (w + f_one);

	m_a1 = w - f_one;

	m_w = w;
}




//
//eq implementation

template <class SAMPLE_T, UInt NPOLE> inline DSP::Filters::Cascaded1P<SAMPLE_T,NPOLE>::Cascaded1P()
{
	REFLEX_LOOP(idx, NPOLE) m_z[idx] = 0.0f;
}

template <class SAMPLE_T, UInt NPOLE> REFLEX_INLINE SAMPLE_T DSP::Filters::Cascaded1P<SAMPLE_T, NPOLE>::ProcessLP(const Parameters & parameters, const SAMPLE_T & in)
{
	SAMPLE_T t = in;

	REFLEX_LOOP(idx, NPOLE)
	{
		SAMPLE_T & z = m_z[idx];

		SAMPLE_T a = (t - (parameters.m_a1 * z)) * parameters.m_1diva0;

		t = (a + z) * parameters.m_w;

		z = Detail::DNC(a);
	}

	return t;
}

template <class SAMPLE_T, UInt NPOLE> REFLEX_INLINE SAMPLE_T DSP::Filters::Cascaded1P<SAMPLE_T,NPOLE>::ProcessHP(const Parameters & parameters, const SAMPLE_T & in)
{
	SAMPLE_T t = in;

	REFLEX_LOOP(idx, NPOLE)
	{
		SAMPLE_T & z = m_z[idx];

		SAMPLE_T a = (t - (parameters.m_a1 * z)) * parameters.m_1diva0;

		t = a - z;

		z = Detail::DNC(a);
	}

	return t;
}

template <class SAMPLE_T, UInt NPOLE> REFLEX_INLINE Pair <SAMPLE_T> DSP::Filters::Cascaded1P<SAMPLE_T,NPOLE>::ProcessSplit(const Parameters & parameters, const SAMPLE_T & in)
{
	REFLEX_STATIC_ASSERT(NPOLE == 1)

	SAMPLE_T & z = m_z[0];

	SAMPLE_T a = (in - (parameters.m_a1 * z)) * parameters.m_1diva0;

	Pair <SAMPLE_T> rtn = { (a + z) * parameters.m_w, a - z };

	z = Detail::DNC(a);

	return rtn;
}

