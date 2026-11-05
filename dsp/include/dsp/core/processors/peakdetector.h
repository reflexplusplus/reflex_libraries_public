#pragma once

#include "[require].h"
#include "../functions/math.h"
#include "../functions/convert.h"
#include "detail/dnc.h"




//
//(@doc:group=Audio)

REFLEX_NS(DSP)

template <class TYPE> class PeakDetector;

REFLEX_END


		

//
//peakdetector

template <class TYPE>
class DSP::PeakDetector
{
public:

	//types

	DSP_PROCESSOR(TYPE);



	//lifetime

	PeakDetector();



	//process
	
	void SetState(const TYPE & value);

	void SetState(UInt vidx, Float value);

	const TYPE & GetState() const;


	const TYPE & Process(const Parameters & parameters, const TYPE & input);

	const TYPE & Process(const Parameters & parameters, const Stereo <TYPE> & input);



private:

	TYPE m_state;

};




//
//parameters

template <class TYPE>
class DSP::PeakDetector<TYPE>::Parameters
{
public:

	//lifetime

	Parameters();



	//setup
	
	void SetSampleRate(Float sr);

	
	
	//paramters
		
	void SetTime(const TYPE & ms);

	
	
private:

	friend class PeakDetector;


	TYPE m_sr;

	TYPE m_ms;

	TYPE m_multiplier;

};




//
//parameters implementation

template <class TYPE> DSP::PeakDetector<TYPE>::Parameters::Parameters()
	: m_sr(Constants::f_one)
	, m_ms(Constants::f_one)
	, m_multiplier(Constants::f_zero)
{
}

template <class TYPE> inline void DSP::PeakDetector<TYPE>::Parameters::SetSampleRate(Float sr)
{
	m_sr = sr;

	SetTime(m_ms);
}

template <class TYPE> REFLEX_INLINE void DSP::PeakDetector<TYPE>::Parameters::SetTime(const TYPE & ms)
{
	m_ms = ms;

	TYPE ns = ms2samples(ms, m_sr);

	m_multiplier = Exp2<kPrecisionFast>(Detail::ConvertConstants<TYPE>::f_log2_rcp_onehundred / Max(ns, Constants::f_one));
}




//
//processor implementation

template <class TYPE> inline DSP::PeakDetector<TYPE>::PeakDetector()
	: m_state(0.0f)
{
}

template <class TYPE> REFLEX_INLINE void DSP::PeakDetector<TYPE>::SetState(const TYPE & value)
{
	m_state = value;
}

template <class TYPE> REFLEX_INLINE void DSP::PeakDetector<TYPE>::SetState(UInt vidx, Float value)
{
	m_state[vidx] = value;
}

template <class TYPE> REFLEX_INLINE const TYPE & DSP::PeakDetector<TYPE>::GetState() const
{
	return m_state;
}

template <class TYPE> REFLEX_INLINE const TYPE & DSP::PeakDetector<TYPE>::Process(const Parameters & parameters, const TYPE & input)
{
	TYPE abs(Abs(input));

	m_state = Select(abs > m_state, abs, Detail::DNC(m_state * parameters.m_multiplier));

	return m_state;
}

template <class TYPE> REFLEX_INLINE const TYPE & DSP::PeakDetector<TYPE>::Process(const Parameters & parameters, const Stereo <TYPE> & input)
{
	TYPE abs(Max(Abs(input.l), Abs(input.r)));

	m_state = Select(abs > m_state, abs, Detail::DNC(m_state * parameters.m_multiplier));

	return m_state;
}
