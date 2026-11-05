#pragma once

#include "master.h"




//
//(@doc:group=Delay)

namespace DSP
{

	namespace Delay
	{

		class Processor_V4;

	}

}



//
//mono4p

class DSP::Delay::Processor_V4 : public AbstractProcessor
{
public:

	//types
	
	typedef SIMD::FloatV4 FloatV4;

	typedef SIMD::IntV4 IntV4;

	
	
	//lifetime
	
	Processor_V4(Master & master);


	void Clear();

	void Clear(UInt vidx);


	void SetDelay(FloatV4 value);

	void SetDelay(IntV4 whole, FloatV4 frac);


	FloatV4 Process(const FloatV4 & in);

	FloatV4 Process1P(const FloatV4 & in);
	


private:

	virtual void OnAllocate(const Master & master);


	IntV4 m_wrap;

	const ::Int & writepos;


	UInt m_buffersize;

	Array <Float32> m_buffer;

	Float32 * m_voices[4];


	IntV4 m_idelay;

	FloatV4 m_x;

};




//
//

inline DSP::Delay::Processor_V4::Processor_V4(Master & master)
	: AbstractProcessor(master)
	, writepos(master.GetWritePos())
	, m_idelay(0)
	, m_x(0.0f)
{
	//OnAllocate(master);
}

inline void DSP::Delay::Processor_V4::Clear()
{
	m_buffer.Wipe();
}

inline void DSP::Delay::Processor_V4::Clear(UInt vidx)
{
	MemClear(m_voices[vidx], (m_buffersize + 4) * sizeof(Float32));
}

REFLEX_INLINE void DSP::Delay::Processor_V4::SetDelay(FloatV4 value)
{
	m_idelay = SIMD::Truncate(value);

	m_x = SIMD::kfOne - (value - SIMD::IntToFloat(m_idelay));

	REFLEX_ASSERT(UInt(m_idelay[0]) < m_buffersize);
}

REFLEX_INLINE void DSP::Delay::Processor_V4::SetDelay(IntV4 whole, FloatV4 invfrac)
{
	m_idelay = whole;

	m_x = invfrac;
}

REFLEX_INLINE SIMD::FloatV4 DSP::Delay::Processor_V4::Process(const FloatV4 & in)
{
	//write

	REFLEX_ASSERT(m_voices[0] == m_buffer.GetData());

	REFLEX_LOOP(vidx, 4) m_voices[vidx][writepos] = in[vidx];

	REFLEX_LOOP(vidx, 4)
	{
		const FloatV4 & start = *Reinterpret<FloatV4>(m_voices[vidx]);

		FloatV4 & wrap = *Reinterpret<FloatV4>(m_voices[vidx] + m_buffersize);

		wrap = start;

		//REFLEX_LOOP(idx, 4)
		//{
		//	ptr[m_buffersize] = ptr[0];

		//	ptr++;
		//}
	}


	//read

	IntV4 ipos = (writepos - IntV4(3) - m_idelay) & m_wrap;

	//Int t = ipos[0];
	
	FloatV4 a(LoadUnaligned(m_voices[0] + ipos[0]));
	FloatV4 b(LoadUnaligned(m_voices[1] + ipos[1]));
	FloatV4 c(LoadUnaligned(m_voices[2] + ipos[2]));
	FloatV4 d(LoadUnaligned(m_voices[3] + ipos[3]));

	Transpose(a, b, c, d);

	return Interpolate4P_RT(m_x, a, b, c, d);
}

REFLEX_INLINE SIMD::FloatV4 DSP::Delay::Processor_V4::Process1P(const FloatV4 & in)
{
	//write
	
	REFLEX_LOOP(vidx, 4) m_voices[vidx][writepos] = in[vidx];
	
	
	//read
	
	IntV4 ipos = (writepos - m_idelay) & m_wrap;
	
	FloatV4 read(m_voices[0][ipos[0]], m_voices[1][ipos[1]], m_voices[2][ipos[2]], m_voices[3][ipos[3]]);
	
	return read;
}

inline void DSP::Delay::Processor_V4::OnAllocate(const Master & master)
{
	m_buffersize = master.GetBufferSize();

	m_wrap = m_buffersize - 1;

	UInt blocksize = m_buffersize + 4;
	
	m_buffer.SetSize(blocksize * 4);

	REFLEX_LOOP(vidx, 4) m_voices[vidx] = m_buffer.GetData() + (blocksize * vidx);

	m_buffer.Wipe();
}

