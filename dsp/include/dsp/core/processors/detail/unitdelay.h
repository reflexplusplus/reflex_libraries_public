#pragma once

#include "dnc.h"




//
//declarations

REFLEX_NS(DSP::Detail)

template <class TYPE, bool DNC> class UnitDelay;

REFLEX_END




//
//noise

template <class TYPE, bool DNC> 
class DSP::Detail::UnitDelay
{
public:

	//types

	typedef Detail::Constants <TYPE> Constants;



	//lifetime

	UnitDelay();



	//process

	void Process(TYPE & value);

	const TYPE & GetValue() const;



private:

	TYPE m_state;
	
};




//
//implementation

template <class TYPE, bool DNC> inline DSP::Detail::UnitDelay<TYPE,DNC>::UnitDelay()
	: m_state(Constants::f_zero)
{
}

template <class TYPE, bool DNC> REFLEX_INLINE void DSP::Detail::UnitDelay<TYPE,DNC>::Process(TYPE & value)
{
	TYPE temp = m_state;

	if constexpr (DNC)
	{
		m_state = Detail::DNC(value);
	}
	else
	{
		m_state = value;
	}

	value = temp;
}

template <class TYPE, bool DNC> REFLEX_INLINE const TYPE & DSP::Detail::UnitDelay<TYPE,DNC>::GetValue() const
{
	return m_state;
}

