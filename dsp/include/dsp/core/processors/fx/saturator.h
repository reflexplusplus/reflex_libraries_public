#pragma once

#include "../../functions/saturation.h"




//
//(@doc:group=Audio)

namespace DSP
{

	template <class TYPE> class Saturator;

	typedef Saturator <FloatV4> Saturator_V4;	//TODO: DSP::Saturator::Parameter should be pre-calc for ParabolicSat function, move this "flavoured" class to Freestyle

}
			



//
//saturator

template <class TYPE>
class DSP::Saturator
{
public:

	//types

	typedef Detail::Constants <TYPE> Constants;

	//TODO class Parameters;



	//lifetime
	
	Saturator();



	//setup

	void SetAmount(const TYPE & amount);

		
		
	//process

	void Process(TYPE & value);

	void Process(Stereo <TYPE> & value);



private:

	TYPE m_amp;

	TYPE m_dry;

	TYPE m_wet;

};




//
//parameters implementation

template <class TYPE> REFLEX_INLINE DSP::Saturator<TYPE>::Saturator()
	: m_amp(Constants::f_one)
	, m_dry(Constants::f_one)
	, m_wet(Constants::f_zero)
{
}

template <class TYPE> REFLEX_INLINE void DSP::Saturator<TYPE>::SetAmount(const TYPE & amount)
{
	m_amp = db2a<kPrecisionFast>(amount * 60.0f);

	
	TYPE postamp = Square(Constants::f_one - amount);

	postamp = ((Square(postamp) * postamp) * FloatV4(0.85f)) + FloatV4(0.15f);


	m_wet = Min(Constants::f_eight * amount, Constants::f_one);
	
	m_dry = Constants::f_one - m_wet;

	m_wet *= postamp;
}

template <class TYPE> REFLEX_INLINE void DSP::Saturator<TYPE>::Process(TYPE & value)
{
	value = (value * m_dry) + ((ParabolicSatL4(value * m_amp)) * m_wet);
}

template <class TYPE> REFLEX_INLINE void DSP::Saturator<TYPE>::Process(Stereo <TYPE> & value)
{
	value.l = (value.l * m_dry) + ((ParabolicSatL4(value.l * m_amp) - value.l) * m_wet);

	value.r = (value.r * m_dry) + ((ParabolicSatL4(value.r * m_amp) - value.r) * m_wet);
}
