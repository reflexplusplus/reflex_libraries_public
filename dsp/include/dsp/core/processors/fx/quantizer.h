#pragma once

#include "[require].h"




//
//(@doc:group=Audio)

REFLEX_NS(DSP)

template <class TYPE> class Quantizer;

REFLEX_END




//
//quantizer

template <class TYPE>
class DSP::Quantizer
{
public:

	DSP_PROCESSOR(TYPE);



	//lifetime

	Quantizer();

		
		
	//process
	
	TYPE Process(const Parameters & parameters, const TYPE & in);

	Stereo <TYPE> ProcessStereo(const Parameters & parameters, const Stereo <TYPE> & in);

};




//
//parameters

template <class TYPE>
class DSP::Quantizer<TYPE>::Parameters
{
public:

	//lifetime
	
	Parameters();

	
	
	//set

	void Set(const TYPE & bits);

	void Set(UInt vidx, Float bits);

	void Set(const TYPE & bits, const TYPE & output_gain);

	void Set(UInt vidx, Float bits, Float output_gain);



private:

	friend class Quantizer;


	TYPE m_mult_up, m_mult_down;

};




//
//parameters implementation

template <class TYPE> inline DSP::Quantizer<TYPE>::Parameters::Parameters()
	: m_mult_up(2048.0f)
	, m_mult_down(1.0f / 2048.0f)
{
}

template <class TYPE> inline void DSP::Quantizer<TYPE>::Parameters::Set(const TYPE & bits)
{
	m_mult_up = Exp2(bits - Constants::f_one);

	m_mult_down = Reciprocal(m_mult_up);
}

template <class TYPE> inline void DSP::Quantizer<TYPE>::Parameters::Set(UInt vidx, Float bits)
{
	Float & mult_up = m_mult_up[vidx];

	Float & mult_down = m_mult_down[vidx];

	mult_up = Exp2(bits - 1.0f);

	mult_down = Reciprocal(mult_up);
}

template <class TYPE> inline void DSP::Quantizer<TYPE>::Parameters::Set(const TYPE & bits, const TYPE & output_gain)
{
	Set(bits);

	m_mult_down *= output_gain;
}

template <class TYPE> inline void DSP::Quantizer<TYPE>::Parameters::Set(UInt vidx, Float bits, Float output_gain)
{
	Set(vidx, bits);

	m_mult_down[vidx] *= output_gain;
}




//
//processor implementation

template <class TYPE> inline DSP::Quantizer<TYPE>::Quantizer()
{
}

template <class TYPE> REFLEX_INLINE TYPE DSP::Quantizer<TYPE>::Process(const Parameters & parameters, const TYPE & in)
{
	auto t = (in * parameters.m_mult_up) + Constants::f_rcp_two;

	return (DSP::RoundNearest(t) - Constants::f_rcp_two) * parameters.m_mult_down;
}

template <class TYPE> REFLEX_INLINE DSP::Stereo <TYPE> DSP::Quantizer<TYPE>::ProcessStereo(const Parameters & parameters, const Stereo <TYPE> & in)
{
	auto left = (in.l * parameters.m_mult_up) + Constants::f_rcp_two;

	left = (DSP::RoundNearest(left) - Constants::f_rcp_two) * parameters.m_mult_down;


	auto right = (in.r * parameters.m_mult_up) + Constants::f_rcp_two;

	right = (DSP::RoundNearest(right) - Constants::f_rcp_two) * parameters.m_mult_down;


	return { left, right };
}
