#pragma once

#include "defines.h"
#include "access.h"
#include "token.h"




//
//Core::Module

class Netx::Core::Module :
	public Item <Netx::Core::Module,true,Data::PropertySet>,
	public Identity,
	public StateMt
{
public:

	static Module & null;



	//types

	struct StreamsView;

	struct ProcessInfo;

	struct RtConfig;

	typedef FunctionPointer <void(UInt,ProcessInfo&)> ProcessRtFn;



	//lifetime

	void Attach(Access access, Network & network);			//attach to network

	void Detach(Access access);								//remove from network

	Network * GetNetwork() { return Cast<Network>(GetList(true)); }



	//setup

	void ForceActive(Access access, bool enable = true);



	//info
	
	bool IsForcedActive() const { return m_force_active; }

	bool IsActive() const { return m_active; }

	UInt GetProcessOrder() const { return m_process_order; }


	
	//update

	void Update(Access, UInt8 flags);	//above plus schedules OnUpdate

	void Reconnect(Access);				//scheduule kRefreshFlagModuleConnections



	//pins

	Reflex::List <Pin> inputs, outputs;

	

protected:

	//lifetime
	
	Module();	

	~Module();



	//callbacks

	virtual void OnAttachNetwork() = 0;

	virtual Unretained <Object> OnInitialiseProcessor() = 0;

	virtual void OnDetachNetwork() = 0;

	virtual void OnSetMainThread() = 0;

	virtual RtConfig OnUpdate(const Data::PropertySet & properties, const StreamsView & streams, UInt8 flags, ConstAlreadyRetained <Object> previous_processor) = 0;



	//object callbacks

	//void OnReleaseData() override;



private:

	friend Item;

	friend Network;

	friend Pin;
	
	friend Stream;


	void QueueUpdate(Network * network, RefreshFlags flag);

	void OnAttach(List & list);

	void OnDetach(List & list);


	using Item::Detach;
	using Item::Attach;
	using Item::InsertAfter;
	using Item::InsertBefore;
	using Item::SendBottom;
	using Item::SendTop;

	using StateMt::Notify;


	UInt8 m_update_flags;

	bool m_force_active;

	bool m_active;

	Int16 m_process_order;


	UInt64 m_pin_hashes;

	UInt64 m_connections_hash;


	ProcessRtFn m_onprocess_rt;

};




//
//Module::ProcessInfo

struct Netx::Core::Module::StreamsView
{
	ArrayView <ConstAlreadyRetained<Stream>> inputs;
	
	ArrayView <AlreadyRetained<Stream>> outputs;
};

struct Netx::Core::Module::ProcessInfo
{
	StreamsView streams;

	const AlreadyRetained <Object> processor;

	ProcessRtFn callback;
};

struct Netx::Core::Module::RtConfig
{
	ProcessRtFn callback;

	WillRetain <Object> processor;
};

REFLEX_SET_TRAIT(Netx::Core::Module::ProcessInfo, IsRawCopyable);




//
//impl

REFLEX_INLINE void Netx::Core::Module::Reconnect(Access)
{
	m_connections_hash = kHashSeed;

	QueueUpdate(Cast<Network>(GetList(true)), kRefreshFlagModuleConnections);
}

REFLEX_INLINE void Netx::Core::Module::Update(Access, UInt8 flags)
{
	QueueUpdate(Cast<Network>(GetList(true)), RefreshFlags(flags));
}
