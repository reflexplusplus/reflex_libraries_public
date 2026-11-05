#pragma once

#include "parameters.h"




//
//(@doc:group=Delay)

REFLEX_NS(DSP::Delay)

class Processor;

REFLEX_END




//
//mono4p

class DSP::Delay::Processor : public AbstractProcessor
{
public:

	Processor(Master & master);


	void Clear();


	//parameters (move to seperate class, for efficient stereo solution)
	
	void SetDelay(Float value);

	void SetDelay(Reflex::Int whole, Float frac);


	Float Process(Float in);



private:

	virtual void OnAllocate(const Master & master);


	Int m_wrap;

	const ::Int & writepos;


	Array <Float> m_buffer;

	Reflex::Int m_idelay;

	Float m_x;

};




//
//

inline DSP::Delay::Processor::Processor(Master & master)
	: AbstractProcessor(master)
	, m_wrap(master.GetBufferSize() - 1)
	, writepos(master.GetWritePos())
	, m_idelay(0)
	, m_x(0.0f)
{
	m_buffer.SetSize(master.GetBufferSize() + 4);

	m_buffer.Wipe();
}

inline void DSP::Delay::Processor::Clear()
{
	m_buffer.Wipe();
}

REFLEX_INLINE void DSP::Delay::Processor::SetDelay(Float value)
{
	m_idelay = Truncate(value);

	m_x = 1.0f - (value - Float(m_idelay));
}

REFLEX_INLINE void DSP::Delay::Processor::SetDelay(Reflex::Int whole, Float invfrac)
{
	m_idelay = whole;

	m_x = invfrac;
}

REFLEX_INLINE Float DSP::Delay::Processor::Process(Float in)
{
	//write

	Float * buffer = m_buffer.GetData();

	buffer[writepos] = in;
	
	//TODO wrap end like in Vector?


	//read

	//::Int wrap = Master::bfrlen - 1;

	::Int pos = (writepos - 3 - m_idelay) & m_wrap;
	
	Float v_1 = buffer[pos];
	Float v0 = buffer[++pos & m_wrap];
	Float v1 = buffer[++pos & m_wrap];
	Float v2 = buffer[++pos & m_wrap];

	return Interpolate4P_RT(m_x, v_1, v0, v1, v2);
}

inline void DSP::Delay::Processor::OnAllocate(const Master & master)
{
	m_wrap = master.GetBufferSize() - 1;

	m_buffer.SetSize(master.GetBufferSize() + 4);

	m_buffer.Wipe();
}
