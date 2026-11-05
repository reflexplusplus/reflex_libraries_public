#pragma once

#include "master.h"




//
//(@doc:group=Delay)

REFLEX_NS(DSP::Delay)
				
BufferSize SelectBufferSize(UInt32 max_samples);

BufferSize SelectBufferSize(Float sr, Float max_seconds);

REFLEX_END




//
//master

inline DSP::Delay::BufferSize DSP::Delay::SelectBufferSize(UInt32 max)
{
	UInt t = kBufferSize1024;

	while (max > t) t *= 2;

	return BufferSize(t);
}

inline DSP::Delay::BufferSize DSP::Delay::SelectBufferSize(Float sr, Float max_seconds)
{
	return SelectBufferSize(Truncate(max_seconds * sr) + 1);
}