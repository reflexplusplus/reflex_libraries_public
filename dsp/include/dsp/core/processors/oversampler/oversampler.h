#pragma once

#include "../[require].h"




//
//declarations

namespace DSP
{

	// Wraps a processor with a compile-time oversampling algorithm. PROCESSOR supplies
	// SampleType, SetSampleRate(), Reset(), and Process(const SampleType &, ...).
	// ALGORITHM supplies
	// a SampleType-bound implementation containing independent Upsampler and
	// Downsampler components. Oversampler owns and performs the processing sequence.

	template <class PROCESSOR, template <class> class ALGORITHM> class Oversampler;

}




//
//Oversampler

template <class PROCESSOR, template <class> class ALGORITHM>
class DSP::Oversampler : public PROCESSOR
{
public:

	//types

	using SampleType = typename PROCESSOR::SampleType;

	using Algorithm = ALGORITHM <SampleType>;



	//constants

	static constexpr UInt kFactor = Algorithm::kFactor;

	static constexpr UInt kLatency = Algorithm::kLatency;



	//setup

	void SetSampleRate(Float sr);

	void Reset();

	void Reset(UInt vidx);



	//processing

	template <class ... VARGS> SampleType Process(const SampleType & sample, VARGS &&... v);



private:

	typename Algorithm::Upsampler m_upsampler;

	typename Algorithm::Downsampler m_downsampler;

};




//
//implementation

template <class PROCESSOR, template <class> class ALGORITHM> inline void DSP::Oversampler<PROCESSOR,ALGORITHM>::SetSampleRate(Float sr)
{
	m_upsampler.SetSampleRate(sr);
	m_downsampler.SetSampleRate(sr);

	PROCESSOR::SetSampleRate(sr * Float(kFactor));
}

template <class PROCESSOR, template <class> class ALGORITHM> inline void DSP::Oversampler<PROCESSOR,ALGORITHM>::Reset()
{
	m_upsampler.Reset();
	m_downsampler.Reset();

	PROCESSOR::Reset();
}

template <class PROCESSOR, template <class> class ALGORITHM> inline void DSP::Oversampler<PROCESSOR,ALGORITHM>::Reset(UInt vidx)
{
	m_upsampler.Reset(vidx);
	m_downsampler.Reset(vidx);

	PROCESSOR::Reset(vidx);
}

template <class PROCESSOR, template <class> class ALGORITHM> template <class ... VARGS>
REFLEX_INLINE typename DSP::Oversampler<PROCESSOR,ALGORITHM>::SampleType DSP::Oversampler<PROCESSOR,ALGORITHM>::Process(const SampleType & sample, VARGS &&... v)
{
	SampleType values[kFactor];

	m_upsampler.Process(sample, values);

	for (auto & value : values) value = PROCESSOR::Process(value, v...);

	return m_downsampler.Process(values);
}
