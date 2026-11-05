#pragma once

#include "forward.h"
#include "processor.h"
#include "interfaces/ipages.h"




//
//Component

REFLEX_EXTERN_NULL(SamplerV3::Instrument);

class SamplerV3::Instrument:
	public Netx::Component,
	public List <Group>,
	public Detail::MetaData,
	public iPages
{
public:

	//types

	typedef Netx::KeyboardState <UInt16,Float32> KeyboardState;



	//lifetime

	NETX_COMPONENT(SamplerV3, Instrument, K32("Sampler"), Netx::ClassID config_classid);
	
	
	
	//config

	virtual void SetClient(Access access, Object & client) = 0;

	virtual void SetNumOutput(UInt noutput) = 0;



	//groups

	virtual void Clear() = 0;

	virtual void Sort(const Function <UInt(const Group&)> & get_index) = 0;



	//audio preview from UI thread

	virtual void Panic() = 0;

	
	virtual void PlayNote(UInt8 note, Float velocity) = 0;

	virtual void ReleaseNote(UInt8 note, bool choke = false) = 0;


	virtual void PlayZone(const Zone & zone, bool reverse, Float velocity) = 0;

	virtual void ReleaseZone(const Zone & zone, bool choke = false) = 0;



	//info

	virtual ConstAlreadyRetained <KeyboardState> GetKeyState() const = 0;



	//links

	const ConstRef <Config> config;

	const Pair <UInt> pages[Group::kNumPage];

	const AlreadyRetained <Object> client;

	const AlreadyRetained <Netx::VoiceManager> voicemanager;

	const AlreadyRetained <Netx::ParameterSet> parameterset;

	const ConstAlreadyRetained <Processor> processor;


	
	//disambig
	
	using Detail::MetaData::Set;	//disambig (maybe vstudio bug as no other set declared?)



protected:

	friend Group;

	friend Zone;

	typedef Netx::Component::Record Record;
	
	using Netx::Component::Update;	//will refresh map


	Instrument(const Config & config, const Pair <UInt> * pages, Netx::ParameterSet &);

	Instrument(const Config & config);	//null


	bool OnCommand(Key32 cmd, Data::PropertySet & args) override;
};




//
//Zone::Transaction

struct SamplerV3::Zone::Transaction :
	public Object,
	public Instrument::Record
{
	REFLEX_OBJECT(Transaction, Object);

	Transaction(Zone & zone);

	~Transaction();

	const Reference <Zone> zone;
};




//
//impl

REFLEX_INLINE void SamplerV3::Group::Notify()
{
	instrument->Update();

	State::Notify();
}
