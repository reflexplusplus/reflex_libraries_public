#pragma once

#include "osc.h"




//
//declarations

REFLEX_NS(DSP)

template <class TYPE> class LFO;

REFLEX_END
			



//
//LFO

template <class TYPE>
class DSP::LFO : public DSP::Phase <TYPE>
{
public:

	//types

	typedef Detail::Constants <TYPE> Constants;

	class Parameters;



	//lifetime
	
	using DSP::Phase<TYPE>::Phase;




	//process

	TYPE Reset(const Parameters & params, const TYPE & phase);

	Float Reset(const Parameters & params, UInt vidx, Float phase);


	TYPE Process(const Parameters & params);



private:

	static const TYPE kPointSevenFive;

};




//
//processor

template <class TYPE> 
class DSP::LFO<TYPE>::Parameters : public DSP::Phase<TYPE>::Parameters
{
public:

	//lifetime
	
	Parameters();



	//setup
	
	using DSP::Phase<TYPE>::Parameters::SetSampleRate;



	//access

	using DSP::Phase<TYPE>::Parameters::SetFrequency;


	void SetShape(const TYPE & shape);

	void SetShape(UInt vidx, Float shape);



	//special access

	static TYPE Calculate(const Parameters & params, const TYPE & phase);

	static Float Calculate(const Parameters & params, UInt vidx, Float phase);



private:

	TYPE m_satlevel, m_triangular, m_parabolic;


	static const TYPE kForty;

};




//
//parameters implementation

template <class TYPE> const TYPE DSP::LFO<TYPE>::kPointSevenFive(0.75f);

template <class TYPE> const TYPE DSP::LFO<TYPE>::Parameters::kForty(40.0f);

template <class TYPE> inline DSP::LFO<TYPE>::Parameters::Parameters()
	: m_satlevel(0.499999881f)
	, m_triangular(0.0f)
	, m_parabolic(2.0f)
{
}

template <class TYPE> REFLEX_INLINE void DSP::LFO<TYPE>::Parameters::SetShape(const TYPE & shape)
{
	TYPE one = Constants::f_one;

	TYPE mix = shape * Constants::f_two;

	TYPE t0 = Max(mix - one, Constants::f_zero);

	TYPE postamp = db2a(t0 * kForty) + one;

	m_satlevel = Reciprocal(postamp);

	mix = Min(mix, one);

	m_triangular = one - mix;

	m_parabolic = mix * postamp;
}

template <class TYPE> REFLEX_INLINE void DSP::LFO<TYPE>::Parameters::SetShape(UInt vidx, Float shape)
{
	Float mix = shape * 2.0f;

	Float t0 = Max(mix - 1.0f, 0.0f);

	Float postamp = db2a(t0 * 40.0f) + 1.0f;

	m_satlevel[vidx] = Reciprocal(postamp);

	mix = Min(mix, 1.0f);

	m_triangular[vidx] = 1.0f - mix;

	m_parabolic[vidx] = mix * postamp;
}

template <class TYPE> REFLEX_INLINE TYPE DSP::LFO<TYPE>::Parameters::Calculate(const Parameters & params, const TYPE & phase)
{
	TYPE tri = Triangle(phase);

	return (tri * params.m_triangular) + (ParabolicSat(tri, params.m_satlevel) * params.m_parabolic);
}

template <class TYPE> REFLEX_INLINE Float DSP::LFO<TYPE>::Parameters::Calculate(const Parameters & params, UInt vidx, Float phase)
{
	Float tri = Triangle(phase);

	return (tri * params.m_triangular[vidx]) + (ParabolicSat(tri, params.m_satlevel[vidx]) * params.m_parabolic[vidx]);
}




//
//implementation

template <class TYPE> REFLEX_INLINE TYPE DSP::LFO<TYPE>::Reset(const Parameters & params, const TYPE & phase)
{
	auto wrap = Modulo(phase + kPointSevenFive, Constants::f_one) - Constants::f_rcp_two;

	DSP::Phase<TYPE>::SetPhase(wrap);

	return Parameters::Calculate(params, wrap);
}

template <class TYPE> REFLEX_INLINE Float DSP::LFO<TYPE>::Reset(const Parameters & params, UInt vidx, Float phase)
{
	auto wrap = Modulo(phase + 0.75f, 1.0f) - 0.5f;

	DSP::Phase<TYPE>::SetPhase(vidx, wrap);

	return Parameters::Calculate(params, vidx, wrap);
}

template <class TYPE> REFLEX_INLINE TYPE DSP::LFO<TYPE>::Process(const Parameters & params)
{
	return Parameters::Calculate(params, DSP::Phase<TYPE>::Process(params));
}

