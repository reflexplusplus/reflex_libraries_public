#pragma once

#include "defines.h"





//
//functions

REFLEX_NS(DSP)

class FrequencyRangeNormaliser;

REFLEX_END




//
//

class DSP::FrequencyRangeNormaliser
{
public:

	//lifetime

	FrequencyRangeNormaliser(Float minhz, Float maxhz);



	//access

	Float Normalise(Float hz) const;

	FloatV4 Normalize(const FloatV4 & value) const;

	Float NormalisePrecise(Float hz) const;


	Float Denormalize(Float value) const;

	FloatV4 Denormalize(const FloatV4 & value) const;

	Float DenormalizePrecise(Float value) const;


	
	//range
	
	const Float min, max;



private:

	Float m_min, m_max, m_range;

};
