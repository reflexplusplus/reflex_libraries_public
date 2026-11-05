#pragma once

#include "processor.h"




//
//(@doc:group=Delay)

REFLEX_NS(DSP::Delay)
		
class DiffusionProcessor;

REFLEX_END




//
//diffusion

class DSP::Delay::DiffusionProcessor : public Processor <Float>
{
public:

	//lifetime
	
	DiffusionProcessor();



	//setup
	
	void Clear();


	
	//RT
	
	void SetDiffusion(Float diff);

	Float Process(const Parameters & parameters, const Float & in);



private:

	Float m_diff, m_diff_inv, m_z;

};




//
//implementation

inline DSP::Delay::DiffusionProcessor::DiffusionProcessor()
	: //Processor(master),
	m_diff(0.5f)
	, m_diff_inv(-0.5f)
	, m_z(0.0f)
{
}

inline void DSP::Delay::DiffusionProcessor::Clear()
{
	Processor::Clear();

	m_z = 0.0f;
}

REFLEX_INLINE void DSP::Delay::DiffusionProcessor::SetDiffusion(Float diff)
{
	m_diff = diff;
	
	m_diff_inv = -diff;
}

REFLEX_INLINE Float DSP::Delay::DiffusionProcessor::Process(const Parameters & parameters, const Float & in)
{
	Float out = m_z + (in * m_diff_inv);

	m_z = Processor::Process4P(parameters, in + (out * m_diff));

	return out;
}

