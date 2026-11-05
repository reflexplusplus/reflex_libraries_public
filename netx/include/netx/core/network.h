#pragma once

#include "referencemt.h"
#include "module.h"
	



//
//Core::Network

class Netx::Core::Network : 
	public Data::PropertySet,
	public Module::List,
	public StateMt
{
public:

	REFLEX_OBJECT(Netx::Core::Network, Object);

	static Network & null;



	//types

	class Lock;

	struct Processor;



	//lifetime

	Network();

	~Network();

	void Deinit();



	//setup

	void RegisterStream(ClassID classid, FunctionPointer <Unretained<Stream>(const Data::PropertySet&, bool)> ctr);

	const Stream * QueryNullStream(ClassID classid) const;


	void EnableProfiling(bool module_level, bool enable = true);

	bool ProfilingEnabled(bool module_level) const;


	void ScheduleCallback(void * client, FunctionPointer <bool(void * client)> callback);	//emitted on network primary thread
	
	void CancelCallback(void * client);






	//refresh/purge
	
	bool Refresh();		//true if "circular update" occured



	//process
	
	void ProcessRt(UInt samples);
	
	ConstAlreadyRetained <Output::Profiler> GetPerformanceInfo() const;



	//debug

	void SetMainThread();

	bool IsLocked() const;



	//allocator

	const AlreadyRetained <Allocator> allocator_st;

	const AlreadyRetained <Allocator> allocator_rt;



	//threadinfo

	Reflex::Detail::ThreadValidator <REFLEX_DEBUG> threadinfo;



protected:

	//access
	
	static void RefreshImpl(Network & self);


	void OnUnsetProperty(Address address) override;

	void OnSetProperty(Address adr, Object & object) override;

	void OnQueryProperty(Address address, Object * & pointer) const override;



private:

	struct Private;

	struct ProcessingStateMt;


	friend Module;

	friend Pin;

	friend Stream;

	
	void CallOnRefreshClients();


	
	ConstReference <Data::PropertySet> m_properties_rt;


	Pin::StreamType m_nullstreamtype;

	Map <ClassID,Pin::StreamType> m_streamtypes;

	
	Array < Pair < void*,FunctionPointer<bool(void*)> > > m_refresh_listeners;

	Map <Module*,ConstReference<Reflex::Object>> m_module_processors;


	const UInt8 m_uid;
	
	UInt8 m_lockcount;

	UInt8 m_update_flags;

	Flags8 m_processflags;

	
	const Reference <ProcessingStateMt> m_state_mt;

	ReferenceMt <Processor> m_processor_mt;

};




//
//Network::Lock (DEPRECATED)

class Netx::Core::Network::Lock
{
public:

	Lock(Network & network);

	~Lock();

	Network & network;
};




//
//impl

struct Netx::Core::Network::Processor : public Object
{
	REFLEX_OBJECT(Processor, Object);

	REFLEX_INLINE void ProcessRt(UInt samples) { (m_processfn_current)(*this, samples); }

	FunctionPointer <void(const Processor&, UInt)> m_processfn_current = [](const Processor&, UInt) {};
};

REFLEX_INLINE bool Netx::Core::Network::Refresh()
{
	RefreshImpl(*this);

	return m_update_flags;
}

REFLEX_INLINE void Netx::Core::Network::ProcessRt(UInt samples)
{
	Reference processor = m_processor_mt.LoadMt();

	processor->ProcessRt(samples);
}

REFLEX_INLINE bool Netx::Core::Network::IsLocked() const
{
	REFLEX_ASSERT(m_processor_mt.threadvalidator);

	return True(m_lockcount);
}
