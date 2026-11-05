#pragma once

#include "../osc.h"




//
//(@doc:group=Audio)

REFLEX_NS(DSP)

template <class TYPE> class AM;

REFLEX_END




//
//am

template <class TYPE>
class DSP::AM
{
public:

	DSP_PROCESSOR(TYPE);


	
	//lifetime

	AM();



	//setup
	
	void SetSampleRate(const Parameters & parameters, Float sr) {}



	//reset

	void Reset(UInt vidx);



	//process

	void Process(const Parameters & parameters, TYPE & value);

	void Process(const Parameters & parameters, Stereo <TYPE> & value);



private:

	Phase <TYPE> m_phase;
	

	static const TYPE st_minus_rcp_four, st_zeropointsevenfive;

};




//
//parameters

template <class TYPE>
class DSP::AM<TYPE>::Parameters
{
public:

	//lifetime

	Parameters();



	//setup
	
	void SetSampleRate(Float sr);

	
	
	//values

	void Set(const TYPE & rate, const TYPE & amount);

	void Set(const TYPE & rate, const TYPE & amount, const TYPE & pitchmod);

	void Set(UInt vidx, Float rate, Float amount, Float pitchmod = 0.0f);



private:

	friend class AM;


	typename Phase<TYPE>::Parameters m_phase_parameters;

	TYPE m_wet, m_dry;

};




//
//implementation

template <class TYPE> const TYPE DSP::AM<TYPE>::st_minus_rcp_four(-0.25f);

template <class TYPE> const TYPE DSP::AM<TYPE>::st_zeropointsevenfive(0.75f);

template <class TYPE> REFLEX_INLINE DSP::AM<TYPE>::AM()
{
}

template <class TYPE> REFLEX_INLINE void DSP::AM<TYPE>::Reset(UInt vidx)
{
	m_phase.SetPhase(st_minus_rcp_four);
}

template <class TYPE> REFLEX_INLINE void DSP::AM<TYPE>::Process(const Parameters & parameters, TYPE & value)
{
	const TYPE & phase = m_phase.Process(parameters.m_phase_parameters);

	TYPE osc = Parabol_Uni(phase);

	osc *= parameters.m_wet;

	value = (value * parameters.m_dry) + (value * osc);
}

template <class TYPE> REFLEX_INLINE void DSP::AM<TYPE>::Process(const Parameters & parameters, Stereo <TYPE> & value)
{
	const TYPE & phase = m_phase.Process(parameters.m_phase_parameters);

	TYPE osc = Parabol_Uni(phase);

	osc *= parameters.m_wet;

	const TYPE & dry = parameters.m_dry;

	TYPE & left = value.l;

	TYPE & right = value.r;

	left = (left * dry) + (left * osc);

	right = (right * dry) + (right * osc);
}




//
//parameters implementation

template <class TYPE> inline DSP::AM<TYPE>::Parameters::Parameters()
{
	Set(0.0f, 0.0f);
}

template <class TYPE> inline void DSP::AM<TYPE>::Parameters::SetSampleRate(Float sr)
{
	m_phase_parameters.SetSampleRate(sr);
}

template <class TYPE> REFLEX_INLINE void DSP::AM<TYPE>::Parameters::Set(const TYPE & rate, const TYPE & amount)
{
	TYPE f = p2f_LEGACY<kPrecisionFast>(LinearInterpolate(rate, Constants::f_thirty, Constants::f_onehundred_twenty));

	m_phase_parameters.SetFrequency(f);

	m_wet = amount;

	m_dry = Constants::f_one - m_wet;
}

template <class TYPE> REFLEX_INLINE void DSP::AM<TYPE>::Parameters::Set(const TYPE & rate, const TYPE & amount, const TYPE & pitchmod)
{
	TYPE f = p2f_LEGACY<kPrecisionFast>(LinearInterpolate(rate, Constants::f_thirty, Constants::f_onehundred_twenty) + pitchmod);

	m_phase_parameters.SetFrequency(f);

	m_wet = amount;

	m_dry = Constants::f_one - m_wet;
}
