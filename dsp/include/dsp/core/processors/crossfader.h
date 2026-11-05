#pragma once

#include "[require].h"
#include "../functions/control.h"




//
//(@doc:group=Audio)

REFLEX_NS(DSP)

template <class TYPE> class Crossfader;

REFLEX_END




//
//crossfader

template <class TYPE>
class DSP::Crossfader 
{
public:

	DSP_PROCESSOR(TYPE);



	//lifetime

	Crossfader();

		
		
	//process
	
	TYPE Process(const Parameters & parameters, const TYPE & x0, const TYPE & x1);

	Stereo <TYPE> ProcessStereo(const Parameters & parameters, const Stereo <TYPE> & x0, const Stereo <TYPE> & x1);

};




//
//parameters

template <class TYPE>
class DSP::Crossfader<TYPE>::Parameters 
{
public:

	//lifetime
	
	Parameters();

	
	
	//set

	void SetLinear(const TYPE & value);

	void SetLinear(UInt vidx, Float value);

	
	void SetLinearClipped(const TYPE & value);


	void SetParabolic(const TYPE & value);

	void SetParabolic(UInt vidx, Float value);


	void SetDryBias(const TYPE & value);

	void SetDryBias(UInt vidx, Float value);


	void SetWetMix(const TYPE & value);



private:

	friend class Crossfader;


	TYPE m_dry, m_wet;

};





//
//parameters implementation

template <class TYPE> inline DSP::Crossfader<TYPE>::Parameters::Parameters()
	: m_dry(1.0f)
	, m_wet(0.0f)
{
}

template <class TYPE> inline void DSP::Crossfader<TYPE>::Parameters::SetLinear(const TYPE & value)
{
	m_wet = value;

	m_dry = Constants::f_one - m_wet;
}

template <class TYPE> inline void DSP::Crossfader<TYPE>::Parameters::SetLinear(UInt vidx, Float value)
{
	Float & dry = m_dry[vidx];

	Float & wet = m_wet[vidx];

	wet = value;

	dry = 1.0f - wet;
}

template <class TYPE> inline void DSP::Crossfader<TYPE>::Parameters::SetParabolic(const TYPE & value)
{
	m_dry = Parabolic(Constants::f_one - value);

	m_wet = Parabolic(value);
}

template <class TYPE> inline void DSP::Crossfader<TYPE>::Parameters::SetParabolic(UInt vidx, Float value)
{
	Float & dry = m_dry[vidx];

	Float & wet = m_wet[vidx];

	dry = Parabolic(1.0f - wet);

	wet = Parabolic(wet);
}

template <class TYPE> inline void DSP::Crossfader<TYPE>::Parameters::SetDryBias(const TYPE & value)
{
	m_dry = Constants::f_one - value;

	m_wet = Square(value);
}

template <class TYPE> inline void DSP::Crossfader<TYPE>::Parameters::SetDryBias(UInt vidx, Float value)
{
	auto & dry = m_dry[vidx];

	auto & wet = m_wet[vidx];

	wet = Square(value);

	dry = 1.0f - value;
}

template <class TYPE> inline void DSP::Crossfader<TYPE>::Parameters::SetWetMix(const TYPE & value)
{
	m_dry = Constants::f_one - value;

	m_dry = Min(m_dry + m_dry, Constants::f_one);

	m_wet = Parabolic(value);
}

//template <class TYPE> inline void DSP::Crossfader<TYPE>::Parameters::AdjustWet(const TYPE & amp)
//{
//	m_wet *= amp;
//}




//
//processor implementation

template <class TYPE> inline DSP::Crossfader<TYPE>::Crossfader()
{
}

template <class TYPE> REFLEX_INLINE TYPE DSP::Crossfader<TYPE>::Process(const Parameters & parameters, const TYPE & x0, const TYPE & x1)
{
	return (x0 * parameters.m_dry) + (x1 * parameters.m_wet);
}

template <class TYPE> REFLEX_INLINE DSP::Stereo <TYPE> DSP::Crossfader<TYPE>::ProcessStereo(const Parameters & parameters, const Stereo <TYPE> & x0, const Stereo <TYPE> & x1)
{
	return { (x0.l * parameters.m_dry) + (x1.l * parameters.m_wet), (x0.r * parameters.m_dry) + (x1.r * parameters.m_wet) };
}
