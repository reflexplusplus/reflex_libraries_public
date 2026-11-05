#pragma once

#include "module.h"




//
// declarations

namespace Audio
{
	
	struct TimelinePoint;
	
	class SourceTimeline;
	

	class AudioPlayer;
	
	class Pattern;

}




//
// common descriptions

struct Audio::TimelinePoint
{
	Float64 sample_position = 0.0;
	Float64 source_bps = 0.0;
};

class Audio::SourceTimeline : public Object
{
public:
	
	REFLEX_OBJECT(SourceTimeline, Object);

	static SourceTimeline & null;


	[[nodiscard]] static TRef <SourceTimeline> CreateLinear(Float64 sample_rate, UInt32 ppq, Float64 source_bps);

	virtual TimelinePoint Lookup(UInt32 song_position) const = 0;
};




//
// AudioPlayer

class Audio::AudioPlayer : public Netx::ModuleBase <Netx::Module>
{
public:

	REFLEX_OBJECT(AudioPlayer, ModuleBase);

	using ModuleBase = Netx::ModuleBase <Netx::Module>;

	class Pattern;


	
	//lifetime

	[[nodiscard]] static TRef <AudioPlayer> Create(Netx::ClassID class_id);



	// content

	virtual void SetPattern(Key32 id, TRef <Pattern> pattern) = 0;	//reject !IsSet(id)

	virtual void UnsetPattern(Key32 id) = 0;

	virtual Array <Key32> GetPatterns() const = 0;

	virtual const Pattern * QueryPattern(Key32 id) const = 0;


	
	// transport and selection

	virtual void SetPlaying(bool playing) = 0;

	virtual bool IsPlaying() const = 0;

	virtual void SetGain(Float32 gain) = 0;

	virtual Float32 GetGain() const = 0;

	virtual TRef <Netx::ParameterSet> GetParameters() const = 0;

	virtual void SetCurrentPattern(Key32 id) = 0;

	virtual Key32 GetCurrentPattern() const = 0;

	virtual void SetSongMode(bool enabled) = 0;

	virtual bool IsSongMode() const = 0;



protected:

	AudioPlayer(Netx::ClassID class_id);

};





//
// AudioPlayer::Pattern

class Audio::AudioPlayer::Pattern : public Object
{
public:

	REFLEX_OBJECT(Pattern, Object);

	static Pattern & null;

	enum Mode : UInt8
	{
		kModeAudio,
		kModeVTime,
	};

	[[nodiscard]] static TRef <Pattern> Create(Mode mode, Pair <Int32> region = { 0, 0 });
	[[nodiscard]] static TRef <Pattern> Deserialize(Data::Archive::View & stream, File::ResourcePool::Lock & lock);

	virtual TRef <Pattern> Clone() const = 0;
	virtual Mode GetMode() const = 0;
	virtual ConstTRef <DSP::Sample> GetSample() const = 0;
	virtual void SetSample(ConstTRef <DSP::Sample> sample) = 0;
	// A runtime timing attachment. Concrete timeline persistence is intentionally
	// owned by the caller until SourceTimeline gains a portable archive format.
	virtual ConstTRef <SourceTimeline> GetSourceTimeline() const = 0;
	virtual void SetSourceTimeline(ConstTRef <SourceTimeline> timeline) = 0;
	virtual Pair <Int32> GetRegion() const = 0;
	virtual void SetRegion(Pair <Int32> region) = 0;
	virtual Float32 GetAmp() const = 0;
	virtual void SetAmp(Float32 amp) = 0;
	virtual Float32 GetPan() const = 0;
	virtual void SetPan(Float32 pan) = 0;

	virtual void Serialize(Data::Archive & stream, File::ResourcePool::Lock & lock) const = 0;

	virtual const Netx::Value32 * Query(Key32 id, const Netx::Value32 * fallback = nullptr) const = 0;
	virtual Netx::Value32 Get(Key32 id, Netx::Value32 fallback = {}) const = 0;
	virtual void Set(Key32 id, Netx::Value32 value) = 0;
	virtual void Unset(Key32 id) = 0;


protected:

	Pattern() = default;
	Pattern(const Pattern & copy) = default;
};
