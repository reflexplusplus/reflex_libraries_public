#pragma once

#include "module.h"




//
// declarations

namespace Audio
{

	struct TimelinePoint;

	class SourceTimeline;
	class Pattern;


	class AudioPlayer;

	// Uses source BPM or embedded beat metadata, then falls back to measuring at fallback_bps.
	Float32 GetSourceLengthBeats(const Pattern & pattern, Float64 fallback_bps);

	template <class TYPE>
	struct PropertyDef
	{
		Key32 id;
	};

	template <class TYPE> Address MakeAddress(PropertyDef<TYPE> def) { return MakeAddress<TYPE>(def.id); }
}




//
// common descriptions

struct Audio::TimelinePoint
{
	TimelinePoint(Float64 position, Float64 bps) : sample_position(position), source_bps(bps) {}

	Float64 sample_position;
	Float64 source_bps; // Finite and positive.
};

class Audio::SourceTimeline : public Object
{
public:
	
	REFLEX_OBJECT(SourceTimeline, Object);

	static SourceTimeline & null;

	struct TempoEvent
	{
		UInt32 position = 0;
		Float64 bps = 1.0;
	};

	[[nodiscard]] static Unretained <SourceTimeline> CreateLinear(Float64 sample_rate, UInt32 ppq, Float64 source_bps);
	// Event ticks must be strictly increasing. The first tempo also applies before its event.
	[[nodiscard]] static Unretained <SourceTimeline> CreateTempoMap(Float64 sample_rate, UInt32 ppq, ArrayView <TempoEvent> events, Float64 offset_seconds = 0.0);

	virtual TimelinePoint Lookup(UInt32 song_position) const = 0;
	virtual Float64 GetMaxBps() const = 0;
};




//
// AudioPlayer

class Audio::Pattern : public Data::PropertySet
{
public:
	REFLEX_OBJECT(Pattern, Data::PropertySet);

	static Pattern & null;

	static constexpr Key32 kSample = K32("sample");

	[[nodiscard]] static Unretained <Pattern> Create();

	virtual ConstTRef <DSP::Sample> GetSample() const = 0;
	virtual Pair <UInt> GetRegion() const = 0;

	virtual void Serialize(Data::Archive & stream, File::ResourcePool::Lock & lock) const = 0;
	virtual bool Deserialize(Data::Archive::View & stream, File::ResourcePool::Lock & lock) = 0;


protected:
	Pattern() = default;
};

class Audio::AudioPlayer : public Netx::Component
{
public:

	REFLEX_OBJECT(AudioPlayer, Netx::Component);
	static constexpr Netx::ClassID kClassID = K32("AudioPlayer");

	struct PlayState
	{
		Key32 pattern = kNullKey;
		UInt32 position = 0; // Pattern-relative ticks
	};

	static constexpr Key32 kModeAudio = K32("audio");
	static constexpr Key32 kModeVTime = K32("vtime");

	static constexpr PropertyDef <Data::Key32Property> kMode = { "mode" };
	static constexpr PropertyDef <ObjectOf <Pair <Int32>>> kRegion = { "region" };
	// Positive, finite clip duration in beats. Invalid assignments are ignored.
	static constexpr PropertyDef <Data::Float32Property> kLengthBeats = { "length_beats" };
	static constexpr PropertyDef <Data::Float32Property> kSourceBpm = { "source_bpm" };
	static constexpr PropertyDef <Data::Float32Property> kGain = { "gain" };
	static constexpr PropertyDef <Data::Float32Property> kPan = { "pan" };
	static constexpr PropertyDef <Data::Int32Property> kQuality = { "quality" };
	static constexpr PropertyDef <Data::Float32Property> kPitch = { "pitch" };
	static constexpr PropertyDef <Data::Float32Property> kSpeed = { "speed" };
	static constexpr PropertyDef <Data::BoolProperty> kSync = { "sync" };
	static constexpr PropertyDef <Data::Float32Property> kFit = { "fit" };
	// Repeat the trimmed source within one pass of the clip.
	static constexpr PropertyDef <Data::BoolProperty> kLoop = { "loop" };
	// Runtime attachment; omitted from persistent pattern state.
	static constexpr PropertyDef <SourceTimeline> kSourceTimeline = { "source_timeline" };
	// Runtime track tempo map for unsynced seeks. Tick zero is the clip start, and positions are in source sample frames.
	static constexpr PropertyDef <SourceTimeline> kTrackTimeline = { "track_timeline" };


	
	//lifetime

	// The host module supplies an EventStream input and a StereoStream output.
	// Supply the former controller class ID to retain its saved component address.
	[[nodiscard]] static Unretained <AudioPlayer> Create(Netx::ClassID class_id = kClassID);



	// content

	virtual void UnsetPattern(Key32 id) = 0;

	virtual void SetPattern(Key32 id, WillRetain <Pattern> pattern) = 0;
	virtual Reference <Object> BeginPatternTransaction(Key32 id, WString::View description) = 0;

	virtual Array <Key32> GetPatterns() const = 0;

	virtual Pattern * QueryPattern(Key32 id) = 0;


	
	// transport and selection

	// Song mode selects by clock source. Pattern mode uses the selected pattern. Both wrap at the pattern length.
	virtual void SetSongMode(bool enabled) = 0;

	virtual bool IsSongMode() const = 0;


	virtual void SetPlaying(bool playing) = 0;

	virtual bool IsPlaying() const = 0;


	virtual void SetCurrentPattern(Key32 id) = 0;

	virtual Key32 GetCurrentPattern() const = 0;


	virtual AlreadyRetained <Netx::ParameterSet> GetParameters() const = 0;



	//info

	Netx::ObservedValue <PlayState> playstate;



protected:

	AudioPlayer(Netx::ClassID class_id);

};
