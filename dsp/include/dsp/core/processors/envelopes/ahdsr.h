#pragma once

#include "[require].h"




//
//(@doc:group=Modulation)

REFLEX_NS(DSP::Envelopes)

template <class TYPE> class AHDSR;

REFLEX_END
		



//
//envelope

template <class TYPE>
class DSP::Envelopes::AHDSR
{
public:

	DSP_PROCESSOR(TYPE);
	

	
	//types

	typedef typename Constants::Bool Bool;



	//lifetime

	AHDSR();



	//realtime
	
	void Trigger(const Parameters & parameters, const FloatV4 & velocity);

	void Trigger(const Parameters & parameters, UInt vidx, Float velocity);

	
	void Release(const Parameters & parameters);

	void Release(const Parameters & parameters, UInt vidx);


	void Choke(const Parameters & parameters);

	void Choke(const Parameters & parameters, UInt vidx);



	//process
	
	const TYPE & Process(const Parameters & parameters);

	Bool Active(const Parameters & parameters) const;



protected:

	friend class Parameters;

	void SetTime(UInt vidx, UInt stage, Float value);

	
	TYPE m_state;

	TYPE m_atk_value;

	TYPE m_dcy_mult;

	TYPE m_dcy_value;

	TYPE m_dcy_floor, m_dcy_range;


	static const TYPE kFloor;

};

template <>
class DSP::Envelopes::AHDSR <Float>
{
};




//
//parameters

template <class TYPE>
class DSP::Envelopes::AHDSR<TYPE>::Parameters
{
public:

	//lifetime

	Parameters();



	//setup

	void SetSampleRate(Float sr);



	//parameters

	void SetAttack(const TYPE & attack_ms, const TYPE & hold_ms);
	
	void SetDecay(const TYPE & decay_ms);
	
	void SetSustain(const TYPE & sustain);
	
	void SetRelease(const TYPE & release_ms);


	void SetAttack(UInt vidx, Float attack_ms, Float hold_ms);

	void SetDecay(UInt vidx, Float decay_ms);

	void SetSustain(UInt vidx, Float sustain);

	void SetRelease(UInt vidx, Float release_ms);



protected:

	friend AHDSR;


	TYPE m_samplerate;

	TYPE m_a, m_h, m_d, m_sustain, m_r;


	TYPE m_atk_inc;

	TYPE m_atk_end;

	TYPE m_dcy_mult;

	TYPE m_rel_mult;

	TYPE m_choke_mult;

};




//
//parameters implementation

template <class TYPE> REFLEX_INLINE DSP::Envelopes::AHDSR<TYPE>::Parameters::Parameters()
	: m_samplerate(1.0f)
	, m_a(0.0f)
	, m_h(0.0f)
	, m_d(0.0f)
	, m_sustain(0.0f)
	, m_r(0.0f)
	, m_atk_inc(0.0f)
	, m_atk_end(0.0f)
	, m_dcy_mult(0.0f)
	, m_rel_mult(0.0f)
	, m_choke_mult(1.0f)
{
}

template <class TYPE> REFLEX_INLINE void DSP::Envelopes::AHDSR<TYPE>::Parameters::SetSampleRate(Float samplerate)
{	
	m_samplerate = samplerate;

	SetAttack(m_a, m_h);
	
	SetDecay(m_d);
	
	SetSustain(m_sustain);
	
	SetRelease(m_r);

	m_choke_mult = ms2mult(samplerate, 25.0f);
}

template <class TYPE> REFLEX_INLINE void DSP::Envelopes::AHDSR<TYPE>::Parameters::SetAttack(const TYPE & attack_ms, const TYPE & hold_ms)
{
	m_a = attack_ms;
	m_h = hold_ms;

	auto atk_inc = ms2inc(m_samplerate, attack_ms);

	m_atk_inc = atk_inc;

	m_atk_end = SIMD::kfOne + (ms2samples(hold_ms, m_samplerate) * atk_inc);
}

template <class TYPE> REFLEX_INLINE void DSP::Envelopes::AHDSR<TYPE>::Parameters::SetDecay(const TYPE & decay_ms)
{
	m_d = decay_ms;

	m_dcy_mult = ms2mult(m_samplerate, decay_ms);
}

template <class TYPE> REFLEX_INLINE void DSP::Envelopes::AHDSR<TYPE>::Parameters::SetSustain(const TYPE & sustain)
{
	m_sustain = sustain;
}

template <class TYPE> REFLEX_INLINE void DSP::Envelopes::AHDSR<TYPE>::Parameters::SetRelease(const TYPE & release_ms)
{
	m_r = release_ms;

	m_rel_mult = ms2mult(m_samplerate, release_ms);
}

template <class TYPE> REFLEX_INLINE void DSP::Envelopes::AHDSR<TYPE>::Parameters::SetAttack(UInt vidx, Float attack_ms, Float hold_ms)
{
	m_a[vidx] = attack_ms;
	m_h[vidx] = hold_ms;

	auto atk_inc = ms2inc(m_samplerate[vidx], attack_ms);

	m_atk_inc[vidx] = atk_inc;

	m_atk_end[vidx] = 1.0f + (ms2samples(hold_ms, m_samplerate[vidx]) * atk_inc);
}

template <class TYPE> REFLEX_INLINE void DSP::Envelopes::AHDSR<TYPE>::Parameters::SetDecay(UInt vidx, Float decay_ms)
{
	m_d[vidx] = decay_ms;

	m_dcy_mult[vidx] = ms2mult(m_samplerate[vidx], decay_ms);
}

template <class TYPE> REFLEX_INLINE void DSP::Envelopes::AHDSR<TYPE>::Parameters::SetSustain(UInt vidx, Float sustain)
{
	m_sustain[vidx] = sustain;
}

template <class TYPE> REFLEX_INLINE void DSP::Envelopes::AHDSR<TYPE>::Parameters::SetRelease(UInt vidx, Float release_ms)
{
	m_r[vidx] = release_ms;

	m_rel_mult = ms2mult(m_samplerate[vidx], release_ms);
}

//template <class TYPE> REFLEX_INLINE void DSP::Envelopes::AHDSR<TYPE>::Parameters::Set(const TYPE & attack_ms, const TYPE & hold_ms, const TYPE & decay_ms, const TYPE & sustain, const TYPE & release_ms)
//{
//	m_a = attack_ms;
//	m_h = hold_ms;
//	m_d = decay_ms;
//	m_s = sustain;
//	m_r = release_ms;
//
//	auto atk_inc = ms2inc(m_samplerate, attack_ms);
//
//	m_atk_inc =  atk_inc;
//
//	m_atk_end = SIMD::kfOne + (DSP::ms2samples(hold_ms, m_samplerate) * atk_inc);
//
//
//	m_dcy_mult = ms2mult(m_samplerate, decay_ms);
//
//	m_rel_mult = ms2mult(m_samplerate, release_ms);
//
//	m_sustain = sustain;
//}
//
//template <class TYPE> REFLEX_INLINE void DSP::Envelopes::AHDSR<TYPE>::Parameters::Set(UInt vidx, Float attack_ms, Float hold_ms, Float decay_ms, Float sustain, Float release_ms)
//{
//	Float sr = m_samplerate[vidx];
//
//	m_a[vidx] = attack_ms;
//	m_h[vidx] = hold_ms;
//	m_d[vidx] = decay_ms;
//	m_s[vidx] = sustain;
//	m_r[vidx] = release_ms;
//
//	//FloatV4 ms(attack_ms, hold_ms, decay_ms, release_ms);
//
//	
//	Float atk_inc = ms2inc(sr, attack_ms);
//
//	m_atk_inc[vidx] = atk_inc;
//
//	m_atk_end[vidx] = 1.0f + (DSP::ms2samples(hold_ms, sr) * atk_inc);
//
//
//	m_dcy_mult[vidx] = ms2mult(sr, decay_ms);
//
//	m_rel_mult[vidx] = ms2mult(sr, release_ms);
//
//	m_sustain[vidx] = sustain;
//}





//
//envelope implementation

template <class TYPE> const TYPE DSP::Envelopes::AHDSR<TYPE>::kFloor(db2a<kPrecisionHigh>(-60.0f));

template <class TYPE> inline DSP::Envelopes::AHDSR<TYPE>::AHDSR()
	: m_state(0.0f)
	, m_atk_value(44100000.0f)
	, m_dcy_mult(1.0f)
	, m_dcy_value(0.0f)
	, m_dcy_floor(0.0f)
	, m_dcy_range(0.0f)
{
}

template <class TYPE> REFLEX_INLINE void DSP::Envelopes::AHDSR<TYPE>::Trigger(const Parameters & parameters, const FloatV4 & velocity)
{
	m_atk_value = SIMD::kfZero;

	m_dcy_mult = parameters.m_dcy_mult;

	m_dcy_value = SIMD::kfOne;

	m_dcy_floor = parameters.m_sustain * velocity;

	m_dcy_range = (SIMD::kfOne - parameters.m_sustain) * velocity;

	m_state = SIMD::kfOne;
}

template <class TYPE> REFLEX_INLINE void DSP::Envelopes::AHDSR<TYPE>::Trigger(const Parameters & parameters, UInt vidx, Float velocity)
{
	auto sustain = parameters.m_sustain[vidx];

	m_atk_value[vidx] = 0.0f;

	m_dcy_mult[vidx] = parameters.m_dcy_mult[vidx];

	m_dcy_value[vidx] = 1.0f;

	m_dcy_floor[vidx] = sustain * velocity;

	m_dcy_range[vidx] = (1.0f - sustain) * velocity;

	m_state[vidx] = 1.0f;	//for choking if no env
}

template <class TYPE> REFLEX_INLINE void DSP::Envelopes::AHDSR<TYPE>::Release(const Parameters & parameters)
{
	m_atk_value = parameters.m_atk_end;

	m_dcy_mult = parameters.m_rel_mult;

	m_dcy_value = kfOne;

	m_dcy_floor = kfZero;

	m_dcy_range = m_state;
}

template <class TYPE> REFLEX_INLINE void DSP::Envelopes::AHDSR<TYPE>::Release(const Parameters & parameters, UInt vidx)
{
	m_atk_value[vidx] = parameters.m_atk_end[vidx];

	m_dcy_mult[vidx] = parameters.m_rel_mult[vidx];

	m_dcy_value[vidx] = 1.0f;

	m_dcy_floor[vidx] = 0.0f;

	m_dcy_range[vidx] = m_state[vidx];
}

template <class TYPE> REFLEX_INLINE void DSP::Envelopes::AHDSR<TYPE>::Choke(const Parameters & parameters)
{
	Release(parameters);

	m_dcy_mult = parameters.m_choke_mult;
}

template <class TYPE> REFLEX_INLINE void DSP::Envelopes::AHDSR<TYPE>::Choke(const Parameters & parameters, UInt vidx)
{
	Release(parameters, vidx);

	m_dcy_mult[vidx] = parameters.m_choke_mult[vidx];
}

template <class TYPE> REFLEX_INLINE typename DSP::Detail::Constants<TYPE>::Bool DSP::Envelopes::AHDSR<TYPE>::Active(const Parameters & parameters) const
{
	return SIMD::Or(m_atk_value < parameters.m_atk_end, m_state > kFloor);
}

template <class TYPE> REFLEX_INLINE const TYPE & DSP::Envelopes::AHDSR<TYPE>::Process(const Parameters & parameters)
{
	m_atk_value += parameters.m_atk_inc;
		
	auto attack = m_atk_value < parameters.m_atk_end;


	m_dcy_value *= Select(attack, kfOne, m_dcy_mult);

	auto dcy_value = m_dcy_floor + (m_dcy_value * m_dcy_range);


	m_state = Min(m_atk_value, kfOne) * dcy_value;

	return m_state;
}
