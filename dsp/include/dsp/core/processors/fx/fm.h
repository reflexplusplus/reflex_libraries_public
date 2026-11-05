#pragma once

#include "../osc.h"
#include "../delay.h"




//
//(@doc:group=Audio)

REFLEX_NS(DSP)

template <class TYPE> class FM;

REFLEX_END




//
//am

template <class TYPE>
class DSP::FM
{
public:

	DSP_PROCESSOR(TYPE);


	
	//lifetime

	FM();



	//setup

	void SetSampleRate(const Parameters & parameters, Float sr);



	//reset

	void Reset();

	void Reset(UInt vidx);



	//process

	void Process(const Parameters & parameters, TYPE & value);

	void Process(const Parameters & parameters, Stereo <TYPE> & value);



private:

	Phase <TYPE> m_phase;


	Delay::Master m_master;

	Delay::ProcessorV4::Parameters m_delay_parameters_l, m_delay_parameters_r;

	Delay::ProcessorV4 m_left, m_right;


	static inline const TYPE kMinus0_25 = -0.25f;

	static inline const TYPE k0_75 = 0.75f;

};




//
//parameters

template <class TYPE>
class DSP::FM<TYPE>::Parameters
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

	friend class FM;


	typename Phase<TYPE>::Parameters m_phase_parameters;

	Float m_delaymult;
	
	TYPE m_max_p;

	TYPE m_delay;


	static inline const TYPE k72 = 72.0f;

};




//
//implementation

template <class TYPE> REFLEX_INLINE DSP::FM<TYPE>::FM()
	: m_master(DSP::Delay::kBufferSize4096)
{
	m_left.Connect(m_master);

	m_right.Connect(m_master);
}

template <class TYPE> REFLEX_INLINE void DSP::FM<TYPE>::SetSampleRate(const Parameters & parameters, Float sr)
{
	UInt max = Truncate(30.0f * parameters.m_delaymult) + 1;

	m_master.SetBufferSize(DSP::Delay::SelectBufferSize(max));
}

template <class TYPE> REFLEX_INLINE void DSP::FM<TYPE>::Reset()
{
	m_phase.SetPhase(kMinus0_25);

	m_left.Clear();

	m_right.Clear();
}

template <class TYPE> REFLEX_INLINE void DSP::FM<TYPE>::Reset(UInt vidx)
{
	m_phase.SetPhase(vidx, -0.25f);	//kMinus0_25

	m_left.Clear(vidx);
	
	m_right.Clear(vidx);
}

template <class TYPE> REFLEX_INLINE void DSP::FM<TYPE>::Process(const Parameters & parameters, TYPE & value)
{
	const TYPE & phase = m_phase.Process(parameters.m_phase_parameters);

	TYPE osc = Parabol_Uni(phase);

	//osc = (Triangle_Uni(phase) * Constants::f_rcp_four) + (osc * k0_75);

	m_delay_parameters_l.SetTime(osc * parameters.m_delay);

	value = m_left.Process4P(m_delay_parameters_l, value);

	m_master.Process();
}

template <class TYPE> REFLEX_INLINE void DSP::FM<TYPE>::Process(const Parameters & parameters, Stereo <TYPE> & value)
{
	const TYPE & phase = m_phase.Process(parameters.m_phase_parameters);

	TYPE osc = Parabol_Uni(phase);
	
	TYPE time = osc * parameters.m_delay;

	m_delay_parameters_l.SetTime(time);

	m_delay_parameters_r.SetTime(parameters.m_delay - time);

	TYPE & left = value.l;

	TYPE & right = value.r;

	left = m_left.Process4P(m_delay_parameters_l, left);

	right = m_right.Process4P(m_delay_parameters_r, right);

	m_master.Process();
}




//
//parameters implementation

template <class TYPE> REFLEX_INLINE DSP::FM<TYPE>::Parameters::Parameters()
	: m_delaymult(1.0f)
	, m_max_p(1.0f)
	, m_delay(0.0f)
{
}

template <class TYPE> REFLEX_INLINE void DSP::FM<TYPE>::Parameters::SetSampleRate(Float sr)
{
	m_phase_parameters.SetSampleRate(sr);

	m_delaymult = sr / 1000.0f;

	m_max_p = f2p(sr * 0.25f, 60.0f);
}

template <class TYPE> REFLEX_INLINE void DSP::FM<TYPE>::Parameters::Set(const TYPE & rate, const TYPE & amount)
{
	TYPE p = Min(LinearInterpolate(rate, Constants::f_thirty, Constants::f_onehundred_twenty), m_max_p);

	TYPE f = p2f<kPrecisionFast>(p);

	m_phase_parameters.SetFrequency(f);

	TYPE scale = Min(SIMD::kfOne, Reciprocal(p2ff<kPrecisionFast>(p, k72)));

	m_delay = Cube(amount) * scale * Constants::f_thirty * m_delaymult;
}

template <class TYPE> REFLEX_INLINE void DSP::FM<TYPE>::Parameters::Set(const TYPE & rate, const TYPE & amount, const TYPE & pitchmod)
{
	TYPE p = Min(LinearInterpolate(rate, Constants::f_thirty, Constants::f_onehundred_twenty) + pitchmod, m_max_p);

	TYPE f = p2f<kPrecisionFast>(p);

	m_phase_parameters.SetFrequency(f);

	TYPE scale = Min(SIMD::kfOne, Reciprocal(p2ff<kPrecisionFast>(p, k72)));

	m_delay = Cube(amount) * scale * Constants::f_thirty * m_delaymult;
}

template <class TYPE> REFLEX_INLINE void DSP::FM<TYPE>::Parameters::Set(UInt vidx, Float rate, Float amount, Float pitchmod)
{
	Float p = Min(LinearInterpolate(rate, 30.0f, 120.0f) + pitchmod, m_max_p[vidx]);

	Float f = p2f<kPrecisionFast>(p);

	m_phase_parameters.SetFrequency(vidx, f);

	Float scale = Min(1.0f, Reciprocal(p2ff<kPrecisionFast>(p, 72.0f)));

	m_delay[vidx] = Cube(amount) * scale * 30.0f * m_delaymult;
}


