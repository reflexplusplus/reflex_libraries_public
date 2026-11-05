#pragma once

#include "[require].h"




//
//(@doc:group=Audio)

REFLEX_NS(DSP)

template <class TYPE> class Phase;


template <class TYPE> TYPE Saw_Uni(const TYPE & phase);

template <class TYPE> TYPE Triangle_Uni(const TYPE & phase);	//needs external modulo for phase shift

template <class TYPE> TYPE Parabol_Uni(const TYPE & phase);

template <class TYPE> TYPE Pulse_Uni(const TYPE & phase);


template <class TYPE> TYPE Saw(const TYPE & phase);

template <class TYPE> TYPE Triangle(const TYPE & phase);		//needs external modulo for phase shift

template <class TYPE> TYPE Parabol(const TYPE & phase);

template <class TYPE> TYPE Pulse(const TYPE & phase);

REFLEX_END




//
//phase

template <class TYPE> 
class DSP::Phase
{
public:

	DSP_PROCESSOR(TYPE);
	


	//lifetime
	
	Phase();



	//phase
	
	void SetPhase(const TYPE & phase);

	void SetPhase(UInt vidx, Float phase);



	//process

	const TYPE & Process(const Parameters & parameters);



protected:

	TYPE m_t;

};




//
//parameters

template <class TYPE> 
class DSP::Phase<TYPE>::Parameters
{
public:

	//lifetime

	Parameters();



	//setup

	void SetSampleRate(Float samplerate);



	//state

	void SetFrequency(const TYPE & frequency);

	void SetFrequency(const BoolV4 & mask, const TYPE & frequency);

	void SetFrequency(UInt vidx, Float frequency);
	


	//access

	TYPE GetInc() const {return m_inc;}



protected:

	friend class Phase;


	TYPE m_sr_rcp;

	TYPE m_frequency;
		
	TYPE m_inc;

};




//
//impl

REFLEX_NS(DSP::Detail)

template <class TYPE> TYPE WrapPhase(const TYPE & phase);

REFLEX_END




//
//implementation

template <class TYPE> REFLEX_INLINE DSP::Phase<TYPE>::Phase()
	: m_t(0.0f)
{
}

template <class TYPE> REFLEX_INLINE void DSP::Phase<TYPE>::SetPhase(const TYPE & phase)
{
	m_t = phase;
}

template <class TYPE> REFLEX_INLINE void DSP::Phase<TYPE>::SetPhase(UInt vidx, Float phase)
{
	m_t[vidx] = phase;
}

template <class TYPE> REFLEX_INLINE const TYPE & DSP::Phase<TYPE>::Process(const Parameters & parameters)
{
	m_t += parameters.m_inc;

	m_t -= DSP::RoundNearest(m_t);

	return m_t;
}




//
//parameters implementation

template <class TYPE> inline DSP::Phase<TYPE>::Parameters::Parameters()
	: m_sr_rcp(1.0f)
	, m_frequency(1.0f)
	, m_inc(0.0f)
{
}

template <class TYPE> inline void DSP::Phase<TYPE>::Parameters::SetSampleRate(Float samplerate)
{
	m_sr_rcp = 1.0f / samplerate;

	SetFrequency(m_frequency);
}

template <class TYPE> REFLEX_INLINE void DSP::Phase<TYPE>::Parameters::SetFrequency(const TYPE & frequency)
{
	m_frequency = frequency;

	m_inc = frequency * m_sr_rcp;
}

template <class TYPE> REFLEX_INLINE void DSP::Phase<TYPE>::Parameters::SetFrequency(const BoolV4 & mask, const TYPE & frequency)
{
	m_frequency = Select(mask, frequency, m_frequency);

	m_inc = m_frequency * m_sr_rcp;
}

template <class TYPE> REFLEX_INLINE void DSP::Phase<TYPE>::Parameters::SetFrequency(UInt vidx, Float frequency)
{
	m_frequency[vidx] = frequency;

	m_inc[vidx] = m_frequency[vidx] * m_sr_rcp[vidx];
}




//
//shape implementation

template <class TYPE> REFLEX_INLINE TYPE DSP::Detail::WrapPhase(const TYPE & phase)
{
	return phase - DSP::RoundNearest(phase);
}

template <class TYPE> REFLEX_INLINE TYPE DSP::Saw_Uni(const TYPE & phase)
{
	typedef Detail::Constants <TYPE> Constants;

	return phase + Constants::f_rcp_two;
}

template <class TYPE> REFLEX_INLINE TYPE DSP::Triangle_Uni(const TYPE & phase)
{
	typedef Detail::Constants <TYPE> Constants;

	TYPE ph = Abs(phase);

	ph *= Constants::f_rcp_two;

	ph += ph;

	return ph * Constants::f_two;
}

template <class TYPE> REFLEX_INLINE TYPE DSP::Parabol_Uni(const TYPE & phase)
{
	typedef Detail::Constants <TYPE> Constants;

	TYPE ph = phase;

	ph = ph * (Constants::f_four - (Abs(ph) * Constants::f_eight));

	return ph + Constants::f_rcp_two;
}

template <class TYPE> REFLEX_INLINE TYPE DSP::Pulse_Uni(const TYPE & phase)
{
	typedef Detail::Constants <TYPE> Constants;

	return Select(phase > Constants::f_zero, Constants::f_one);
}

template <class TYPE> REFLEX_INLINE TYPE DSP::Saw(const TYPE & phase)
{
	typedef Detail::Constants<TYPE> Constants;

	return (phase * Constants::f_two);
}

template <class TYPE> REFLEX_INLINE TYPE DSP::Triangle(const TYPE & phase)
{
	typedef Detail::Constants <TYPE> Constants;

	TYPE ph = Abs(phase);

	ph *= Constants::f_two;

	ph += ph;

	return ph - Constants::f_one;
}

template <class TYPE> REFLEX_INLINE TYPE DSP::Parabol(const TYPE & phase)
{
	typedef Detail::Constants <TYPE> Constants;

	return phase * (Constants::f_eight - (Abs(phase) * Constants::f_sixteen));
}

template <class TYPE> REFLEX_INLINE TYPE DSP::Pulse(const TYPE & phase)
{
	typedef Detail::Constants <TYPE> Constants;

	return Select(phase > Constants::f_zero, Constants::f_one, Constants::f_minus_one);
}
