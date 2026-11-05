#pragma once

#include "../../functions/interpolate.h"




//
//(@doc:group=Delay)

REFLEX_NS(DSP::Delay)
				
enum BufferSize
{
	kBufferSize1024 = 1024,
	kBufferSize2048 = 2048,
	kBufferSize4096 = 4096,
	kBufferSize8192 = 8192,
	kBufferSize16384 = 16384,
	kBufferSize32768 = 32768,
	kBufferSize65536 = 65536,
	kBufferSize524288 = 524288,
};

class Master;

class AbstractProcessor;

REFLEX_END




//
//master

class DSP::Delay::Master
{
public:

	//lifetime
	
	Master(BufferSize buffersize = kBufferSize32768);


	
	//buffersize
	
	void SetBufferSize(BufferSize buffersize);


	
	//increment
	
	void Process();



private:

	friend class AbstractProcessor;


	Reflex::Item<AbstractProcessor>::List m_processors;

	Reflex::Int m_buffersize;

	Reflex::Int m_wrap;

	Reflex::Int m_writepos;

};




//
//processor

class DSP::Delay::AbstractProcessor : public Item <AbstractProcessor>
{
public:

	void Connect(Master & master);



protected:

	bool Connected() const { return True(GetList()); }

	
	UInt m_buffersize = 0;

	Int m_wrap = 0;

	const ::Int * m_writepos;



private:

	virtual void OnConnect(const Master & master) = 0;

};




//
//master implementation

inline DSP::Delay::Master::Master(BufferSize buffersize)
	: m_buffersize(buffersize)
	, m_wrap(m_buffersize - 1)
{
	m_writepos = 0;
}

REFLEX_INLINE void DSP::Delay::Master::Process()
{
	m_writepos = (m_writepos + 1) & m_wrap;
}




//
//

inline void DSP::Delay::AbstractProcessor::Connect(Master & master)
{
	if (GetList() != &master.m_processors) Attach(master.m_processors);

	m_buffersize = master.m_buffersize;

	m_wrap = master.m_wrap;

	m_writepos = &master.m_writepos;

	OnConnect(master);
}
