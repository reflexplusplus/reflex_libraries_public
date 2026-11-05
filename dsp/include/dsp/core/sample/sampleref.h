#pragma once

#include "../sample.h"




//
//declarations

REFLEX_NS(DSP)

struct SampleRef;

REFLEX_END




//
//

struct DSP::SampleRef
{
	SampleRef()
		: trim({ 0, 1 })
		, loop(false)
	{
	}

	SampleRef(const Sample & sample)
		: sample(sample)
		, trim({0, Int(sample.length)})
		, loop(false)
	{
	}

	SampleRef(ConstReference <Sample> && temp)
		: sample(std::move(temp))
		, trim({ 0, Int(sample->length) })
		, loop(false)
	{
	}

	SampleRef(const Sample & sample, const Pair <Int> & trim, bool loop)
		: sample(sample)
		, trim(trim)
		, loop(loop)
	{
	}

	ConstReference <Sample> sample;

	Pair <Int> trim;

	bool loop;
};
