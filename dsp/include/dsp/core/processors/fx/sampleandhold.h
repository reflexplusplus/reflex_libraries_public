#pragma once

#include "[require].h"




//
//(@doc:group=Audio)

REFLEX_NS(DSP)

template <class TYPE> class SampleAndHold;

REFLEX_END



//
//sample and hold

template <class TYPE>
class DSP::SampleAndHold
{
public:

	DSP_PROCESSOR(TYPE);



	//lifetime

	SampleAndHold();

		
		
	//process
	
	template <bool DAC> TYPE Process(const Parameters & parameters, const TYPE & value);

	template <bool DAC> Stereo <TYPE> ProcessStereo(const Parameters & parameters, const Stereo <TYPE> & stereo);



private:

	TYPE m_phase;

	Stereo <TYPE> m_latch, m_latch_z;

};




//
//parameters

template <class TYPE>
class DSP::SampleAndHold<TYPE>::Parameters
{
public:

	//lifetime
	
	Parameters();

	
	
	//setup
	
	void SetSampleRate(Float sr);


	
	//set

	void Set(const TYPE & frequency);

	void Set(UInt vidx, Float frequency);



private:

	friend SampleAndHold;


	TYPE m_sr, m_sr_rcp;

	TYPE m_frequency, m_inc;

};




//
//parameters implementation

template <class TYPE> inline DSP::SampleAndHold<TYPE>::Parameters::Parameters()
	: m_sr(1.0f)
	, m_sr_rcp(1.0f)
	, m_frequency(440.0f)
	, m_inc(0.0f)
{
}

template <class TYPE> inline void DSP::SampleAndHold<TYPE>::Parameters::SetSampleRate(Float sr)
{
	m_sr = sr;

	m_sr_rcp = 1.0f / sr;
}

template <class TYPE> REFLEX_INLINE void DSP::SampleAndHold<TYPE>::Parameters::Set(const TYPE & f)
{
	//TYPE frequency = p2f_LEGACY<kPrecisionFast>(LinearInterpolate(value, st_lo, st_hi) + pitchmod);
	
	auto frequency = Min(f, m_sr);

	m_frequency = frequency;

	m_inc = frequency * m_sr_rcp;

	//m_amp = Max((value - TYPE(0.28f)) * TYPE(2.65f), Constants::f_one);
}

template <class TYPE> REFLEX_INLINE void DSP::SampleAndHold<TYPE>::Parameters::Set(UInt vidx, Float f)
{
	//Float frequency = p2f_LEGACY<kPrecisionFast>(LinearInterpolate(value, st_lo[0], st_hi[0]) + pitchmod);
	
	auto frequency = Min(f, m_sr[0]);

	m_frequency[vidx] = frequency;

	m_inc[vidx] = frequency * m_sr_rcp[0];

	//m_amp[vidx] = Max((value - 0.28f) * 2.65f, 1.0f);
}




//
//processor implementation

template <class TYPE> inline DSP::SampleAndHold<TYPE>::SampleAndHold()
	: m_phase(1.0f)
	, m_latch(0.0f)
	, m_latch_z(0.0f)
{
}

template <class TYPE> template <bool DAC> REFLEX_INLINE TYPE DSP::SampleAndHold<TYPE>::Process(const Parameters & parameters, const TYPE & value)
{
	m_phase += parameters.m_inc;
	
	BoolType <TYPE> clock = m_phase >= Constants::f_one;
	
	
	TYPE phase_z = m_phase;
	
	m_phase -= Select(clock, Constants::f_one);


	TYPE & m_latch = SampleAndHold::m_latch.l;

	TYPE & m_latch_z = SampleAndHold::m_latch_z.l;

	if constexpr (DAC)
	{
		//TYPE latch_z = m_latch;

		m_latch_z = Select(clock, m_latch, m_latch_z);

		m_latch = Select(clock, value, m_latch);

		return LinearInterpolate(phase_z, m_latch_z, m_latch);
	}
	else
	{
		m_latch = Select(clock, value, m_latch);

		return m_latch;
	}
}

template <class TYPE> template <bool DAC> REFLEX_INLINE DSP::Stereo <TYPE> DSP::SampleAndHold<TYPE>::ProcessStereo(const Parameters & parameters, const Stereo <TYPE> & value)
{
	m_phase += parameters.m_inc;
	
	BoolType <TYPE> clock = m_phase >= Constants::f_one;
	

	TYPE phase_z = m_phase;

	m_phase -= Select(clock, Constants::f_one);


	if constexpr (DAC)
	{
		m_latch_z = Select(clock, m_latch, m_latch_z);

		m_latch = Select(clock, value, m_latch);

		return { LinearInterpolate(phase_z, m_latch_z.l, m_latch.l), LinearInterpolate(phase_z, m_latch_z.r, m_latch.r) };
	}
	else
	{
		m_latch = Select(clock, value, m_latch);

		return m_latch;
	}
}
