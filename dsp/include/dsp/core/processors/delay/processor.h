#pragma once

#include "parameters.h"
#include "detail/processor.h"




//
//(@doc:group=Delay)

REFLEX_NS(DSP::Delay)

template <class FLOAT> class Processor;

typedef Processor <Float32> Processor_V1;

typedef Processor <FloatV4> ProcessorV4;

REFLEX_END




//
//Float32

template <class FLOAT> 
class DSP::Delay::Processor : public DSP::Delay::Detail::ProcessorOfType <FLOAT,1>
{
public:

	typedef Detail::ProcessorOfType <FLOAT,1> Base;

	typedef Parameters <FLOAT> Parameters;



	//rt
	
	FLOAT Process1P(const Delay::Parameters <FLOAT,false> & params, const FLOAT & in);

	FLOAT Process4P(const Parameters & params, const FLOAT & in);

};




//
//FloatV4

template <> 
class DSP::Delay::Processor <FloatV4> : public DSP::Delay::Detail::ProcessorOfType <Float,4>
{
public:

	typedef Parameters <FloatV4> Parameters;



	//rt

	FloatV4 Process1P(const Delay::Parameters <FloatV4,false> & params, const FloatV4 & in);

	FloatV4 Process4P(const Parameters & params, const FloatV4 & in);
	


private:

	typedef Detail::ProcessorOfType <Float,4> Base;

	void OnConnect(const Master & master) override;


	Float32 * m_voices[4];

};




//
//impl

template <class FLOAT> REFLEX_INLINE FLOAT DSP::Delay::Processor<FLOAT>::Process1P(const Delay::Parameters <FLOAT,false> & params, const FLOAT & in)
{
	auto buffer = Base::m_buffer.GetData();


	//write
	
	auto writepos = *Base::m_writepos;

	buffer[writepos] = in;
	
	
	//read
	
	auto readpos = (writepos - params.m_idelay) & Base::m_wrap;

	return buffer[readpos];
}

template <class FLOAT> REFLEX_INLINE FLOAT DSP::Delay::Processor<FLOAT>::Process4P(const Parameters & params, const FLOAT & in)
{
	//write

	Float * buffer = Base::m_buffer.GetData();

	auto writepos = *Base::m_writepos;

	buffer[writepos] = in;
	
	//TODO wrap end like in Vector?


	//read

	auto wrap = Base::m_wrap;

	::Int readpos = (writepos - 3 - params.m_idelay) & wrap;

	FLOAT v_1 = buffer[readpos];
	FLOAT v0 = buffer[++readpos & wrap];
	FLOAT v1 = buffer[++readpos & wrap];
	FLOAT v2 = buffer[++readpos & wrap];

	return Interpolate4P_RT(params.m_x, v_1, v0, v1, v2);
}

REFLEX_INLINE SIMD::FloatV4 DSP::Delay::ProcessorV4::Process1P(const Delay::Parameters <FloatV4,false> & params, const FloatV4 & in)
{
	//write
	
	auto writepos = *Base::m_writepos;

	REFLEX_LOOP(vidx, 4) m_voices[vidx][writepos] = in[vidx];
	
	
	//read
	
	IntV4 readpos = (writepos - params.m_idelay) & Base::m_wrap;

	FloatV4 read(m_voices[0][readpos[0]], m_voices[1][readpos[1]], m_voices[2][readpos[2]], m_voices[3][readpos[3]]);
	
	return read;
}

REFLEX_INLINE SIMD::FloatV4 DSP::Delay::ProcessorV4::Process4P(const Parameters & params, const FloatV4 & in)
{
	//write

	auto writepos = *Base::m_writepos;

	REFLEX_LOOP(vidx, 4)
	{
		auto voice = m_voices[vidx];

		voice[writepos] = in[vidx];


		auto start = Reinterpret<FloatV4>(voice);

		auto wrap = Reinterpret<FloatV4>(voice + m_buffersize);

		SIMD::AssertAlignment(start);

		SIMD::AssertAlignment(wrap);

		*wrap = *start;
	}

	REFLEX_ASSERT(m_voices[0] == Base::m_buffer.GetData());
	//REFLEX_ASSERT(m_voices[1] == m_voices[0] + m_buffersize2 + 4);
	//REFLEX_ASSERT(m_voices[2] == m_voices[1] + m_buffersize2 + 4);
	REFLEX_ASSERT(m_voices[3] == m_voices[2] + m_buffersize + 4);


	//read

	IntV4 readpos = (writepos - IntV4(3) - params.m_idelay) & Base::m_wrap;

	FloatV4 a(LoadUnaligned(m_voices[0] + readpos[0]));
	FloatV4 b(LoadUnaligned(m_voices[1] + readpos[1]));
	FloatV4 c(LoadUnaligned(m_voices[2] + readpos[2]));
	FloatV4 d(LoadUnaligned(m_voices[3] + readpos[3]));

	Transpose(a, b, c, d);

	return Interpolate4P_RT(params.m_x, a, b, c, d);
}

inline void DSP::Delay::ProcessorV4::OnConnect(const Master & master)
{
	Base::OnConnect(master);

	UInt blocksize = m_buffersize + 4;	//4 is for 4P interpol overrun at edge

	auto buffer = Base::m_buffer.GetData();

	REFLEX_LOOP(vidx, 4)
	{
		m_voices[vidx] = buffer;
		
		buffer += blocksize;
	}
}
