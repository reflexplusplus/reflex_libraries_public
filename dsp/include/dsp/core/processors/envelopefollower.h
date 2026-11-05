#pragma once

#include "../functions/convert.h"
#include "detail/dnc.h"




//
//(@doc:group=Audio)

namespace DSP
{

	template <class TYPE> class EnvelopeFollower;

	typedef EnvelopeFollower <FloatV4> EnvelopeFollower_V4;

}


		

//
//peakdetector

template <class TYPE>
class DSP::EnvelopeFollower
{
public:

	//types

	typedef Detail::Constants <TYPE> Constants;

	class Parameters;



	//lifetime

	EnvelopeFollower();



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
class DSP::EnvelopeFollower<TYPE>::Parameters
{
public:

	//lifetime

	Parameters();



	//setup
	
	void SetSampleRate(Float sr);

	
	
	//paramters
		
	void Set(const TYPE & attack_curve, const TYPE & attack_ms, const TYPE & release_ms);

	
	
private:

	friend class EnvelopeFollower;


	TYPE m_sr;

	TYPE m_attack_curve, m_attack_ms, m_release_ms;


	TYPE m_attack_inc;

	TYPE m_attack_mult, m_release_mult;

};




//
//parameters implementation

template <class TYPE> DSP::EnvelopeFollower<TYPE>::Parameters::Parameters()
	: m_sr(Constants::f_one)
	, m_attack_ms(Constants::f_one)
	, m_release_ms(Constants::f_one)
	, m_attack_mult(Constants::f_zero)
	, m_attack_inc(Constants::f_zero)
	, m_release_mult(Constants::f_zero)
{
}

template <class TYPE> inline void DSP::EnvelopeFollower<TYPE>::Parameters::SetSampleRate(Float sr)
{
	m_sr = sr;

	Set(m_attack_curve, m_attack_ms, m_release_ms);
}

//template <class TYPE> REFLEX_INLINE void DSP::EnvelopeFollower<TYPE>::Parameters::Set(const TYPE & attack_curve, const TYPE & attack_ms, const TYPE & release_ms)
//{
//	TYPE f_log2_rcp_twenty = DSP::Log2(1.0f / 20.0f);
//
//
//	m_attack_curve = attack_curve;
//
//
//	m_attack_ms = attack_ms;
//
//	TYPE as = ms2samples(attack_ms, m_sr);
//
//	m_attack_inc = Reciprocal(as);
//
//	m_attack_mult = Constants::f_one - Exp2<kPrecisionFast>(f_log2_rcp_twenty / Max(as, Constants::f_one));
//
//
//	m_attack_inc *= Constants::f_one - attack_curve;
//
//	m_attack_mult *= attack_curve;
//
//
//	m_release_ms = release_ms;
//
//	TYPE rs = ms2samples(release_ms, m_sr);
//
//	m_release_mult = Constants::f_one - Exp2<kPrecisionFast>(Constants::f_log2_rcp_onehundred / Max(rs, Constants::f_one));
//}




//
//processor implementation

template <class TYPE> inline DSP::EnvelopeFollower<TYPE>::EnvelopeFollower()
	: m_state(0.0f)
{
}

template <class TYPE> REFLEX_INLINE void DSP::EnvelopeFollower<TYPE>::SetState(const TYPE & value)
{
	m_state = value;
}

template <class TYPE> REFLEX_INLINE void DSP::EnvelopeFollower<TYPE>::SetState(UInt vidx, Float value)
{
	m_state[vidx] = value;
}

template <class TYPE> REFLEX_INLINE const TYPE & DSP::EnvelopeFollower<TYPE>::GetState() const
{
	return m_state;
}

template <class TYPE> REFLEX_INLINE void DSP::EnvelopeFollower<TYPE>::Parameters::Set(const TYPE & attack_curve, const TYPE & attack_ms, const TYPE & release_ms)
{
	auto & f_log2_rcp_onehundred = Detail::ConvertConstants<TYPE>::f_log2_rcp_onehundred;


	m_attack_curve = attack_curve;


	m_attack_ms = attack_ms;

	TYPE arcp = Reciprocal(Max(ms2samples(attack_ms, m_sr), Constants::f_one));

	m_attack_mult = Constants::f_one - Exp2<kPrecisionFast>(f_log2_rcp_onehundred * arcp);

	m_attack_mult *= Square(attack_curve);	//approxmiate level correction for mixing linear and curve

	m_attack_inc = arcp * (Constants::f_one - attack_curve);


	m_release_ms = release_ms;

	TYPE rs = ms2samples(release_ms, m_sr);

	m_release_mult = Constants::f_one - Exp2<kPrecisionFast>(f_log2_rcp_onehundred / Max(rs, Constants::f_one));
}

template <class TYPE> REFLEX_INLINE const TYPE & DSP::EnvelopeFollower<TYPE>::Process(const Parameters & parameters, const TYPE & input)
{
	TYPE abs = Abs(input);

	TYPE diff = abs - m_state;


	TYPE up = parameters.m_attack_inc + Detail::DNC(diff * parameters.m_attack_mult);

	up = Min(up, abs - up);


	TYPE down = Detail::DNC(diff * parameters.m_release_mult);


	m_state += Select(abs > m_state, up, down);


	return m_state;
}