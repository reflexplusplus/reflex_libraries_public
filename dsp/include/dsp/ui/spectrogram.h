#pragma once

#include "[require].h"




//
//declarations

REFLEX_NS(DSP)

class Spectrogram;

REFLEX_END




//
//spectrogram

class DSP::Spectrogram :
	public Object,
	public Signal <>
{
public:

	REFLEX_OBJECT(DSP::Spectrogram, Object);

	static Spectrogram & null;



	//lifetime

	static const Spectrogram & Acquire(const DSP::Sample & sample, UInt channel);



	//info

	virtual bool IsReady() const = 0;



	//access

	virtual Float GetSampleRate() const = 0;

	virtual UInt GetSampleLength() const = 0;

	virtual void GetAmplitudes(Float64 position, const Array <Float> & frequencies, Array <Float> & values) const = 0;

};
