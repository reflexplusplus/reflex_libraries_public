#pragma once

#include "sample.h"
#include "../functions/convert.h"




//
//declarations

REFLEX_NS(DSP)

inline const decltype(&File::GetPath) kGetPathFns[] = { &File::GetPath, &File::GetResolvedPath };

Pair <WString::View, File::Attributes::Status> GetSamplePathEx(const File::ResourcePool::Lock & lock, const Sample & sample, bool resolved = true, File::Attributes::Status fallback = {});

inline WString::View GetSamplePath(const File::ResourcePool::Lock & lock, const Sample & sample, bool resolved = true)
{
	return GetSamplePathEx(lock, sample, resolved).a;
}


inline Pair <UInt> GetMetaDataLoop(const Sample & sample)	//b will be zero if no loop
{
	return { sample.Get(DSP::Sample::kLoopStart).uvalue, sample.Get(DSP::Sample::kLoopLength).uvalue };
}

inline Float64 beats2bps(const Sample & sample, Float64 beats)
{
	return beats2bps(sample.samplerate, sample.length, beats);
}


template <class TYPE> Pair <TYPE> ClipRange(const Pair <TYPE> & outer, const Pair <TYPE> & inner);


UInt FindZeroX(const Float32 * sample, UInt length, Int position, Int lookback, Int lookahead);

UInt FindZeroX(const Sample & sample, const Pair <Int> & bounds, Int position, Int max_distance);


bool WriteWav(System::FileHandle & dest, const DSP::Sample & sample, UInt start, UInt length, UInt bitdepth, const Data::PropertySet & metadata = REFLEX_NULL(Data::PropertySet));

bool WriteWav(System::FileHandle & dest, System::FileHandle & source, UInt nchn, Float sr, UInt bitdepth, const Data::PropertySet & metadata = REFLEX_NULL(Data::PropertySet));

REFLEX_END




//
//detail

REFLEX_NS(DSP::Detail)

UInt32 GetFileExtensionHash(const WString::View & path);

DSP::Sample * LoadWAVE(Key32 id, File::Attributes::Status status, const Data::PropertySet & riff);

DSP::Sample * LoadWAVE(Key32 id, File::Attributes::Status status, System::FileHandle & instream);

DSP::Sample * LoadAIFF(Key32 id, File::Attributes::Status status, System::FileHandle & instream);

REFLEX_END




//
//impl

template <class TYPE> REFLEX_INLINE Pair <TYPE> DSP::ClipRange(const Pair <TYPE> & outer, const Pair <TYPE> & inner)
{
	REFLEX_ASSERT(outer.b);

	auto length = Clip<TYPE>(inner.b, 1, outer.b);

	return { Clip<TYPE>(inner.a, outer.a, outer.a + (outer.b - length)), length };
}
