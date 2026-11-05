#pragma once

#include "detail/properties.h"
#include "group.h"




//
//Processor

REFLEX_EXTERN_NULL(SamplerV3::Processor);

class SamplerV3::Processor : public Object
{
public:

	struct GroupRef;

	struct ZoneRef;

	template <class EVENT> using EventFn = FunctionPointer<void(Processor &, Object &, const EVENT &)>;


	typedef Netx::VoiceManager::AbstractProcessor VoiceProcessor;

	typedef VoiceProcessor::VoiceRef VoiceRef;


	struct Iterator;


	
	//access

	Iterator begin() const;

	Iterator end() const;



	//rt
	
	virtual VoiceRef CreateVoiceRt(ZoneRef & zone, const Trigger & trigger) = 0;

	virtual void GlideRt(VoiceRef voiceref, Float pitch, Float time) = 0;

	void ReleaseVoiceRt(VoiceRef voiceref, bool choke);

	void ReleaseVoicesByPacketRt(UInt16 packetid, bool choke);

	void ReleaseVoicesByZoneRt(UInt32 zoneid, bool choke);

	void ReleaseVoicesByGroupRt(UInt16 groupid, bool choke);

	void ReleaseVoicesByVoiceBlockRt(VoiceBlock & voiceblock, UInt8 flags, bool choke);


	VoiceBlock * GetFirstActiveVoiceBlockRt() const;

	virtual ArrayView <VoiceBlock*> GetActiveVoiceBlocksRt(GroupRef & group) const = 0;



	//callbacks (set in OnCompile)

	FunctionPointer <void(Processor & processor, Object & clientdata, const Netx::ClockEvent & gate)> on_clock = 0;
	FunctionPointer <void(Processor & processor, Object & clientdata, const Netx::GateEvent & gate)> on_note_on = [](Processor & processor, Object & clientdata, const Netx::GateEvent & gate) {};
	FunctionPointer <void(Processor & processor, Object & clientdata, const Netx::GateEvent & gate)> on_note_off = on_note_on;
	FunctionPointer <void(Processor & processor, Object & clientdata, GroupRef & groupref, const Netx::Value32 * values)> on_set_custom_param = 0;



	//links
	
	const AlreadyRetained <VoiceProcessor> voice_processor_rt;

	const UInt ngroup;



protected:

	Processor(Netx::VoiceManager::AbstractProcessor & processor, UInt ngroup);

	
	mutable GroupRef * m_pfirst;

};




//
//Processor::ZoneRef

struct SamplerV3::Processor::ZoneRef
{
	const UIntNative reserved;				//ADVANCED -> this is Zone * but accessing zone is only safe during compile, not Rt

	const Ref <GroupRef> groupref;
	const ConstAlreadyRetained <DSP::Sample> sample;
	const ConstAlreadyRetained <Object> enginex_data;	//this should be sample for Standard, this will save one retain/release

	const UInt uid;							//matches Zone::uid
	const UInt32 vbflags;					//this is group and vb already merged

	const UInt8 env_release_flags;			//ampenv|modenv1|modenv2|modenv3
	const bool loop_enabled;
	const bool release_loop;
	const UInt8 pad;

	const Float start, length;
	const Float loop_start, loop_length;

	const Float fade_out;
	const Float loop_xfade;
};




//
//Processor::GroupRef

struct SamplerV3::Processor::GroupRef
{
	const UIntNative reserved;				//ADVANCED -> this is Zone * but accessing zone is only safe during compile, not Rt

	const AlreadyRetained <Netx::StereoStream> output;
	AlreadyRetained <Netx::StereoStream> aux1;
	AlreadyRetained <Netx::StereoStream> aux2;
	const AlreadyRetained <Netx::ParameterSet::Allocation> parameters;
	const AlreadyRetained <Netx::VoiceFx::Processor> voicefx;
	const AlreadyRetained <Netx::Matrix::Processor> matrix;
	Tuple <DSP::Table::Processor&,SIMD::BoolV4> tables[3];	//processor, retrig
	const AlreadyRetained <EngineX> enginex;
	const Triad <Netx::Value32> enginex_properties;
	const FunctionPointer<void(Processor&, VoiceBlock&, UInt, ZoneRef&, const Trigger&)> trigger;

	const UInt8 nmodenv;
	const UInt8 nlfo;
	const UInt8 ntable;
	const UInt8 pad;

	const UInt16 uid;							//matches Group::uid
	const UInt16 nzone;


	auto begin() const { return &zonerefs; }
	auto end() const { return &zonerefs + nzone; }


private:

	GroupRef();

	mutable ZoneRef zonerefs;
};




//
//VoiceBlock

REFLEX_EXTERN_NULL(SamplerV3::VoiceBlock);

class SamplerV3::VoiceBlock : public Netx::VoiceManager::VoiceBlock
{
public:

	//info

	Float GetPosition(UInt vidx) const;

	const IntV4 zoneids;



protected:

	VoiceBlock();

	Processor::ZoneRef * m_zone_refs[4];			//guaranteed to exist by processor

	VoiceBlockX * m_enginex_voiceblock_rt;						//belongs to group, guaranteed to exist by processor
};




//
//VoiceBlockX

REFLEX_EXTERN_NULL(SamplerV3::VoiceBlockX);

struct SamplerV3::VoiceBlockX : public Object
{
	virtual void SetSampleRate(Float32 samplerate) = 0;

	virtual void Start(UInt vidx, Float64 start, Float64 end, Float f, bool reverse, const Float32 * params) = 0;

	virtual void Release(UInt vidx) = 0;

	virtual void End(UInt vidx) = 0;

	
	REFLEX_INLINE void Update(const FloatV4 & f, const Float32 * xyz) { m_update(*this, f, xyz); }

	REFLEX_INLINE SIMD::BoolV4 ProcessSamples(UInt samples, FloatV4 * output) { return m_rendermono(*this, samples, output); }

	REFLEX_INLINE SIMD::BoolV4 ProcessSamples(UInt samples, DSP::StereoV4 * output) { return m_renderstereo(*this, samples, output); }

	virtual FloatV4 GetPosition() const = 0;



	//links
	
	VoiceBlock & vb;



protected:

	template <class DERIVED> using UpdateFn = FunctionPointer<void(DERIVED&, const FloatV4&, const Float32*)>;

	template <class DERIVED> using RenderMonoFn = FunctionPointer<SIMD::BoolV4(DERIVED &, UInt, FloatV4 *)>;

	template <class DERIVED> using RenderStereoFn = FunctionPointer<SIMD::BoolV4(DERIVED&, UInt, DSP::StereoV4 *)>;


	VoiceBlockX(VoiceBlock & vb);

	template <class DERIVED> void SetRenderFns(UpdateFn <DERIVED> updatefn, RenderMonoFn <DERIVED> rendermono, RenderStereoFn <DERIVED> renderstereo);



protected:

	UpdateFn <VoiceBlockX> m_update;

	RenderMonoFn <VoiceBlockX> m_rendermono;

	RenderStereoFn <VoiceBlockX> m_renderstereo;

};




//
//impl

struct SamplerV3::Processor::Iterator
{
	bool operator!=(const Iterator & itr) const { return idx != itr.idx; }
	GroupRef & operator*() const { return *pgroupref; }
	GroupRef * operator->() const { return pgroupref; }
	void operator++();

	GroupRef * pgroupref;
	UInt idx;
};

inline SamplerV3::Processor::Iterator SamplerV3::Processor::begin() const { return { m_pfirst, 0 }; }

inline SamplerV3::Processor::Iterator SamplerV3::Processor::end() const { return { 0, ngroup }; }

inline SamplerV3::Processor::Processor(VoiceProcessor & processor, UInt ngroup)
	: voice_processor_rt(processor)
	, ngroup(ngroup)
{
}

REFLEX_INLINE void SamplerV3::Processor::ReleaseVoiceRt(VoiceRef voiceref, bool choke) 
{
	voice_processor_rt->ReleaseVoiceRt(voiceref, choke); 
}

REFLEX_INLINE void SamplerV3::Processor::ReleaseVoicesByPacketRt(UInt16 packetid, bool choke)
{
	voice_processor_rt->ReleaseVoicesByPacketRt(packetid, choke);
}

REFLEX_INLINE void SamplerV3::Processor::ReleaseVoicesByZoneRt(UInt32 zoneid, bool choke)
{
	voice_processor_rt->ReleaseVoicesByValueRt(REFLEX_OFFSETOF(VoiceBlock, zoneids), zoneid, 0, choke);
}

REFLEX_INLINE void SamplerV3::Processor::ReleaseVoicesByGroupRt(UInt16 groupid, bool choke)
{
	voice_processor_rt->ReleaseVoicesByValueRt(REFLEX_OFFSETOF(VoiceBlock, zoneids), groupid << 16, kMaxUInt16, choke);
}

REFLEX_INLINE SamplerV3::VoiceBlock * SamplerV3::Processor::GetFirstActiveVoiceBlockRt() const
{
	return Cast<VoiceBlock>(voice_processor_rt->GetFirstActiveVoiceBlockRt());
}

REFLEX_INLINE void SamplerV3::Processor::ReleaseVoicesByVoiceBlockRt(VoiceBlock & voiceblock, UInt8 flags, bool choke)
{
	voice_processor_rt->ReleaseVoicesByVoiceBlockRt(voiceblock.index, flags, choke);
}

REFLEX_INLINE Float SamplerV3::VoiceBlock::GetPosition(UInt vidx) const 
{
	return m_enginex_voiceblock_rt->GetPosition()[vidx];
}

template <class DERIVED> inline void SamplerV3::VoiceBlockX::SetRenderFns(UpdateFn <DERIVED> update, RenderMonoFn <DERIVED> rendermono, RenderStereoFn <DERIVED> renderstereo)
{
	m_update = Reflex::Detail::CastFunctionPointer< UpdateFn <VoiceBlockX> >(update);
	m_rendermono = Reflex::Detail::CastFunctionPointer< RenderMonoFn <VoiceBlockX> >(rendermono);
	m_renderstereo = Reflex::Detail::CastFunctionPointer< RenderStereoFn <VoiceBlockX> >(renderstereo);
}
