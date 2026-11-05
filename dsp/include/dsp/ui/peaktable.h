#pragma once

#include "[require].h"




//
//declarations

REFLEX_NS(DSP)

struct BufferRef;

class PeakTable;

BufferRef MakeBufferRef(const Sample & sample, UInt chn);

REFLEX_END




//
//BufferRef

struct DSP::BufferRef
{
	ConstReference <Object> source;

	Float32 sr = 1.0f;

	ArrayView <Float> data;
};




//
//PeakTable

class DSP::PeakTable : public Object
{
public:

	REFLEX_OBJECT(DSP::PeakTable, Object);

	static PeakTable & null;


	class Writer;

	typedef Pair <Float> Peak;



	//lifetime

	[[nodiscard]] static Unretained <PeakTable> Create(Float sr, Float seconds);

	[[nodiscard]] static Unretained <PeakTable> Create(const BufferRef & bufferref);



	//info

	virtual Float GetSampleRate() const = 0;

	virtual ArrayView <Float> GetSource() const = 0;	//source address is optional, but size must be set



	//read

	virtual ArrayView <Peak> GetPeaks() const = 0;	//view and start offset

	virtual const UInt * GetOffset() const = 0;		//if writeable, return a pointer to the writeposition

};




//
//PeakTable::Writer

class DSP::PeakTable::Writer : public Object
{
public:

	REFLEX_OBJECT(DSP::PeakTable::Writer, Object);

	static Writer & null;



	//lifetime

	[[nodiscard]] static Unretained <Writer> Create();


	
	//setup

	virtual void Bind(WillRetain <DSP::PeakTable> target, const BufferRef & source) = 0;



	//process

	virtual void Write(UInt samples) = 0;

};




//
//impl

inline DSP::BufferRef DSP::MakeBufferRef(const Sample & sample, UInt chn)
{
	auto channels = sample.Read();

	return { sample, sample.samplerate, { channels[chn % channels.size], sample.length } };
}
