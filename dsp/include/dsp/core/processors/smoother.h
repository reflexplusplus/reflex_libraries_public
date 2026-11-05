#pragma once

#include "../functions/convert.h"
#include "detail/dnc.h"




//
//(@doc:group=Audio)

REFLEX_NS(DSP)
		
template <class TYPE, bool LINEAR = false> class Smoother;

template <class TYPE, bool LINEAR = false> class SmootherWithState;

REFLEX_END




//
//Smoother

template <class TYPE, bool LINEAR>
class DSP::Smoother
{
public:

	struct PreCalc;

	DSP_PROCESSOR(TYPE);



	//lifetime 
	
	Smoother();



	//process

	void SetTarget(const TYPE & value);

	void SetTarget(UInt vidx, Float value);

	const TYPE & GetTarget() const;


	TYPE ProcessV2(const PreCalc & parameters, const TYPE & value);

	void ProcessReplacing(const PreCalc & parameters, TYPE & value_in_out);


	typename Detail::Constants<TYPE>::Bool Active(const Parameters & parameters, const TYPE & value_in) const;



private:

	TYPE m_target;
	
};




//
//SmootherWithState

template <class TYPE, bool LINEAR>
class DSP::SmootherWithState : public Smoother <TYPE,LINEAR>
{
public:

	typedef typename Smoother<TYPE,LINEAR>::Constants Constants;

	typedef typename Smoother<TYPE,LINEAR>::PreCalc PreCalc;

	typedef typename Smoother<TYPE,LINEAR>::Parameters Parameters;



	//lifetime
	
	SmootherWithState();



	//process

	void SetState(const TYPE & value);

	void SetState(UInt vidx, Float value);

	const TYPE & GetState() const;


	const TYPE & Process(const PreCalc & parameters);

	typename Detail::Constants<TYPE>::Bool Active(const Parameters & parameters) const;



private:

	TYPE m_state;

};






//
//Smoother::Parameters

template <class TYPE, bool LINEAR>
struct DSP::Smoother<TYPE,LINEAR>::PreCalc
{
	TYPE m_multiplier_or_inc = 0.0f;
};

template <class TYPE, bool LINEAR>
class DSP::Smoother<TYPE,LINEAR>::Parameters : public PreCalc
{
public:

	//lifetime 

	Parameters(Float range = 1.0f, Float time = ms2secs(50.0f));



	//setup

	void SetSampleRate(Float sr);



	//parameters

	void SetRange(const TYPE & range);

	TYPE GetRange() const;


	void SetTime_SECONDS(const TYPE & secs);		//_SECONDS is temp transitional

	void SetTime_SECONDS(UInt vidx, Float secs);

	const TYPE & GetTime_SECONDS() const;



private:

	friend Smoother;

	TYPE m_sr;

	TYPE m_threshold;

	TYPE m_time_seconds;

};




//
//impl

template <class TYPE, bool LINEAR> inline DSP::Smoother<TYPE,LINEAR>::Parameters::Parameters(Float range, Float time_ms)
	: m_sr(1.0f)
	, m_threshold(DSP::Detail::Constants<Float>::f_rcp_tenthousand * range)
	, m_time_seconds(time_ms)
{
	REFLEX_ASSERT(time_ms < 1.0f);
}

template <class TYPE, bool LINEAR> inline void DSP::Smoother<TYPE,LINEAR>::Parameters::SetSampleRate(Float sr)
{
	m_sr = sr;

	SetTime_SECONDS(m_time_seconds);
}

template <class TYPE, bool LINEAR> REFLEX_INLINE void DSP::Smoother<TYPE,LINEAR>::Parameters::SetRange(const TYPE & range)
{
	m_threshold = range * Constants::f_rcp_tenthousand;
}

template <class TYPE, bool LINEAR> REFLEX_INLINE TYPE DSP::Smoother<TYPE,LINEAR>::Parameters::GetRange() const
{
	return m_threshold * Constants::f_tenthousand;
}

template <class TYPE, bool LINEAR> REFLEX_INLINE void DSP::Smoother<TYPE,LINEAR>::Parameters::SetTime_SECONDS(const TYPE & secs)
{
	m_time_seconds = secs;

	TYPE ss = Max(secs * m_sr, Constants::f_one);

	if constexpr (LINEAR)
	{
		auto range = GetRange();

		PreCalc::m_multiplier_or_inc = range / ss;
	}
	else
	{
		PreCalc::m_multiplier_or_inc = Constants::f_one - (Exp2<kPrecisionFast>(Detail::ConvertConstants<TYPE>::f_log2_rcp_onehundred / ss));
	}
}

template <class TYPE, bool LINEAR> REFLEX_INLINE void DSP::Smoother<TYPE,LINEAR>::Parameters::SetTime_SECONDS(UInt vidx, Float32 secs)
{
	m_time_seconds[vidx] = secs;

	Float ss = Max(secs * m_sr[vidx], 1.0f);

	if constexpr (LINEAR)
	{
		auto range = m_threshold[vidx] * DSP::Constants<Float>::f_tenthousand;

		PreCalc::m_multiplier_or_inc[vidx] = range / ss;
	}
	else
	{
		PreCalc::m_multiplier_or_inc[vidx] = 1.0f - (Exp2<kPrecisionFast>(Detail::ConvertConstants<Float32>::f_log2_rcp_onehundred / ss));
	}
}

template <class TYPE, bool LINEAR> REFLEX_INLINE const TYPE & DSP::Smoother<TYPE,LINEAR>::Parameters::GetTime_SECONDS() const
{
	return m_time_seconds;
}

template <class TYPE, bool LINEAR> REFLEX_INLINE DSP::Smoother<TYPE,LINEAR>::Smoother()
	: m_target(Constants::f_zero)
{
}

template <class TYPE, bool LINEAR> REFLEX_INLINE void DSP::Smoother<TYPE,LINEAR>::SetTarget(const TYPE & value)
{
	m_target = value;
}

template <class TYPE, bool LINEAR> REFLEX_INLINE void DSP::Smoother<TYPE,LINEAR>::SetTarget(UInt vidx, Float value)
{
	m_target[vidx] = value;
}

template <class TYPE, bool LINEAR> REFLEX_INLINE const TYPE & DSP::Smoother<TYPE,LINEAR>::GetTarget() const
{
	return m_target;
}

template <class TYPE, bool LINEAR> REFLEX_INLINE TYPE DSP::Smoother<TYPE,LINEAR>::ProcessV2(const PreCalc & parameters, const TYPE & in)
{
	if constexpr (LINEAR)
	{
		auto delta = Clip(m_target - in, -parameters.m_multiplier_or_inc, parameters.m_multiplier_or_inc);

		return in + delta;
	}
	else
	{
		//return in + (Detail::DNC((m_target - in) * parameters.m_multiplier_or_inc));

		return MulAdd((m_target - in), parameters.m_multiplier_or_inc, in);
	}
}

template <class TYPE, bool LINEAR> REFLEX_INLINE void DSP::Smoother<TYPE,LINEAR>::ProcessReplacing(const PreCalc & parameters, TYPE & inout)
{
	inout = ProcessV2(parameters, inout);
}

template <class TYPE, bool LINEAR> REFLEX_INLINE typename DSP::Detail::Constants<TYPE>::Bool DSP::Smoother<TYPE,LINEAR>::Active(const Parameters & parameters, const TYPE & state) const
{
	return Abs(m_target - state) > parameters.m_threshold;
}

template <class TYPE, bool LINEAR> REFLEX_INLINE DSP::SmootherWithState<TYPE,LINEAR>::SmootherWithState()
	: m_state(Constants::f_zero)
{
}

template <class TYPE, bool LINEAR> REFLEX_INLINE void DSP::SmootherWithState<TYPE,LINEAR>::SetState(const TYPE & value)
{
	DSP::Smoother<TYPE,LINEAR>::SetTarget(value);

	m_state = value;
}

template <class TYPE, bool LINEAR> REFLEX_INLINE void DSP::SmootherWithState<TYPE,LINEAR>::SetState(UInt vidx, Float value)
{
	DSP::Smoother<TYPE,LINEAR>::SetTarget(vidx, value);

	m_state[vidx] = value;
}

template <class TYPE, bool LINEAR> REFLEX_INLINE const TYPE & DSP::SmootherWithState<TYPE,LINEAR>::GetState() const
{
	return m_state;
}

template <class TYPE, bool LINEAR> REFLEX_INLINE const TYPE & DSP::SmootherWithState<TYPE,LINEAR>::Process(const PreCalc & parameters)
{
	m_state = DSP::Smoother<TYPE,LINEAR>::ProcessV2(parameters, m_state);

	return m_state;
}

template <class TYPE, bool LINEAR> REFLEX_INLINE typename DSP::Detail::Constants<TYPE>::Bool DSP::SmootherWithState<TYPE,LINEAR>::Active(const Parameters & parameters) const
{
	return DSP::Smoother<TYPE,LINEAR>::Active(parameters, m_state);
}
