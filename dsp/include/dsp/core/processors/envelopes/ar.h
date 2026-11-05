#pragma once

#include "[require].h"




//
//(@doc:group=Modulation)

REFLEX_NS(DSP::Envelopes)

template <class SAMPLE> class AR;

REFLEX_END




//
//envelope_ar

template <class SAMPLE>
class DSP::Envelopes::AR
{
public:

	DSP_PROCESSOR(SAMPLE);

	
	
	//lifetime
	
	AR();



	//events

	void Reset();


	void SetGate(const BoolType <SAMPLE> & gate);

	BoolType <SAMPLE> GetGate() const { return Not(m_release); }



	//process

	SAMPLE Process(const Parameters & parameters);

	BoolType <SAMPLE> Released() const;

	BoolType <SAMPLE> Active() const;



private:

	BoolType <SAMPLE> m_release;

	SAMPLE m_amp;

};




//
//parameters

template <class SAMPLE>
class DSP::Envelopes::AR<SAMPLE>::Parameters
{
public:

	//lifetime

	Parameters();



	//setup

	void SetSampleRate(Float sr);



	//parameters

	void SetAttack(const SAMPLE & attack_ms);

	const SAMPLE & GetAttack() const { return m_attack_ms; }


	void SetRelease(const SAMPLE & release_ms);

	const SAMPLE & GetRelease() const { return m_release_ms; }



private:

	friend AR;

	
	SAMPLE m_sr;

	SAMPLE m_attack_ms, m_release_ms;

	SAMPLE m_attack_increment, m_release_multiplier;

};




//
//implementation

template <class SAMPLE> inline DSP::Envelopes::AR<SAMPLE>::Parameters::Parameters()
	: m_attack_ms(0)
	, m_release_ms(0)
{
	SetSampleRate(1.0f);
}

template <class SAMPLE> inline void DSP::Envelopes::AR<SAMPLE>::Parameters::SetSampleRate(Float sr)
{
	m_sr = sr;
	
	SetAttack(m_attack_ms);
	
	SetRelease(m_release_ms);
}

template <class SAMPLE> inline void DSP::Envelopes::AR<SAMPLE>::Parameters::SetAttack(const SAMPLE & attack_ms)
{
	m_attack_ms = attack_ms;

	m_attack_increment = ms2inc(m_sr, attack_ms);
}

template <class SAMPLE> inline void DSP::Envelopes::AR<SAMPLE>::Parameters::SetRelease(const SAMPLE & release_ms)
{
	m_release_ms = release_ms;

	m_release_multiplier = ms2mult(m_sr, release_ms);
}




//
//processor

template <class SAMPLE> inline DSP::Envelopes::AR<SAMPLE>::AR()
{
	Reset();
}

template <class SAMPLE> REFLEX_INLINE void DSP::Envelopes::AR<SAMPLE>::Reset()
{
	m_amp = Constants::f_zero;

	m_release = Constants::kTrue;
}

template <class SAMPLE> REFLEX_INLINE void DSP::Envelopes::AR<SAMPLE>::SetGate(const BoolType <SAMPLE> & gate)
{
	m_release = DSP::Not(gate);
}

template <class SAMPLE> REFLEX_INLINE SAMPLE DSP::Envelopes::AR<SAMPLE>::Process(const Parameters & params)
{
	auto a = Select(m_release, Detail::DNC(m_amp * params.m_release_multiplier), Min(m_amp + params.m_attack_increment, 1.0f));
	
	m_amp = a;

	return a;
}

template <class SAMPLE> REFLEX_INLINE DSP::BoolType <SAMPLE> DSP::Envelopes::AR<SAMPLE>::Released() const
{
	return m_release;
}

template <class SAMPLE> REFLEX_INLINE DSP::BoolType <SAMPLE> DSP::Envelopes::AR<SAMPLE>::Active() const
{
	return DSP::Or(DSP::Not(m_release), m_amp > Constants::f_rcp_onethousand);
}
