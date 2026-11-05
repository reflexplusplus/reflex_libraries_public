#pragma once

#include "../master.h"




//
//(@doc:group=Delay)

REFLEX_NS(DSP::Delay::Detail)

template <class FLOAT, UInt WIDTH> 
struct ProcessorOfType : public AbstractProcessor
{
	void Clear();

	void Clear(UInt vidx);


protected:

	void OnConnect(const Master & master) override;

	Array <FLOAT> m_buffer;
};

REFLEX_END




//
//impl

template <class FLOAT, UInt WIDTH> inline void DSP::Delay::Detail::ProcessorOfType<FLOAT,WIDTH>::Clear()
{
	m_buffer.Wipe();
}

template <class FLOAT, UInt WIDTH> inline void DSP::Delay::Detail::ProcessorOfType<FLOAT,WIDTH>::Clear(UInt vidx)
{
	REFLEX_ASSERT(vidx < WIDTH);

	UInt vlength = (m_buffersize + 4);

	MemClear(m_buffer.GetData() + (vlength * vidx), vlength * sizeof(FLOAT));
}

template <class FLOAT, UInt WIDTH> inline void DSP::Delay::Detail::ProcessorOfType<FLOAT,WIDTH>::OnConnect(const Master & master)
{
	UInt blocksize = m_buffersize + 4;	//4 is for 4P interpol overrun at edge
	
	m_buffer.SetSize(blocksize * WIDTH);

	//REFLEX_LOOP(vidx, 4) m_voices[vidx] = m_buffer.GetData() + (blocksize * vidx);

	m_buffer.Wipe();
}
