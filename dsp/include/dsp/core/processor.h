#pragma once

#include "stereo.h"




REFLEX_NS(DSP)

template <class TYPE>
struct Processor :
	public TYPE::Parameters,
	public TYPE
{
	typedef typename TYPE::SampleType SampleType;


	template <class ... VARGS> REFLEX_INLINE SampleType Process(VARGS &&... v) { return TYPE::Process(*this, std::forward<VARGS>(v)...); }

	template <class ... VARGS> REFLEX_INLINE Stereo <SampleType> ProcessStereo(VARGS &&... v) { return TYPE::ProcessStereo(*this, std::forward<VARGS>(v)...); }

};

REFLEX_END
