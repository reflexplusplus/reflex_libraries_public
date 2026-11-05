#pragma once

#include "master.h"
#include "../../detail/constants.h"




REFLEX_NS(DSP::Delay)

template <class FLOAT, bool INTERPOLATED = true> class Parameters;

template <class FLOAT> class Processor;

REFLEX_END




//
//parameters

template <class FLOAT, bool INTERPOLATED> 
class DSP::Delay::Parameters 
{
public:
	
	//types
	
	typedef DSP::Detail::Constants <FLOAT> Constants;



	//setup

	void SetTime(FLOAT value);

	void SetTime(typename Constants::Int whole, FLOAT frac);



private:

	friend class Processor <FLOAT>;

	typename Constants::Int m_idelay = Constants::i_one;

	FLOAT m_x = Constants::f_one;
};





//
//parameters (non-interpolated)

template <class FLOAT> 
class DSP::Delay::Parameters <FLOAT,false>
{
public:
	
	//types
	
	typedef DSP::Detail::Constants <FLOAT> Constants;

	typedef typename Constants::Int Int;



	//setup

	void SetTime(Int whole);

	Int GetTime() const;



private:

	friend class Processor <FLOAT>;

	Int m_idelay = Constants::i_zero;

};




//
//impl

template <class FLOAT, bool I> REFLEX_INLINE void DSP::Delay::Parameters<FLOAT,I>::SetTime(FLOAT value)
{
	m_idelay = Truncate(value);

	if constexpr (kIsType<FLOAT, SIMD::FloatV4>)
	{
		m_x = Constants::f_one - (value - ToFloatV4(m_idelay));
	}
	else
	{
		m_x = 1.0f - (value - Float32(m_idelay));
	}
}

template <class FLOAT, bool I> REFLEX_INLINE void DSP::Delay::Parameters<FLOAT,I>::SetTime(typename Constants::Int whole, FLOAT invfrac)
{
	m_idelay = whole;

	m_x = invfrac;
}

template <class FLOAT> REFLEX_INLINE void DSP::Delay::Parameters<FLOAT,false>::SetTime(Int whole)
{
	m_idelay = whole;
}

template <class FLOAT> REFLEX_INLINE typename DSP::Detail::Constants<FLOAT>::Int DSP::Delay::Parameters<FLOAT,false>::GetTime() const
{
	return m_idelay;
}