#pragma once

#include "network.h"




//
//Component

class Netx::Component 
	: public Item <Component>,
	public StateMt,
	public Core::iClass,
	public Detail::iUndoable,
	public Protected
{
public:

	REFLEX_OBJECT(Netx::Component, Object);

	static Component & null;

	
	using ComponentType = Component;

	template <class TYPE> using ReferenceType = void;

	typedef Map <Address> Resources;

	enum Flags : UInt8
	{
		kFlagResources = MakeBit(0),
		kFlagNetworkAttachment = MakeBit(1)
	};


	typedef Core::Module::StreamsView StreamsView;

	typedef Core::Module::ProcessRtFn ProcessRtFn;

	typedef Core::Module::ProcessInfo ProcessInfo;



	//lifetime

	Component(ClassID classid, UInt16 version, Key32 id = kNullKey, UInt8 flags = 0);

	~Component();

	void Attach(Access access, Module & module, bool module_persistence = true);

	void Detach(Access access);



	//config

	void SetUpdateFlags(Access, RefreshFlags flags = kRefreshFlagModuleUpdateGroup0, RefreshFlags mask = kRefreshFlagNetworkProperties | kRefreshFlagModuleConnections | kRefreshFlagModuleUpdateGroup0);	//flags: set on Update/~Record, mask: filters out OnUpdate calls




	//io

	void Reset(Key32 context = {});

	void Deserialize(Data::Archive::View & stream, Key32 context = {});

	void Serialize(Data::Archive & stream) const;



	//generic

	bool SendCommand(Key32 id, Data::PropertySet & params);



	//special

	void StoreInitState(Access);



	//properties

	const Key32 uid;

	const bool valid;

	const bool module_storage;	//module owns automatic store, restore, reset and dormant state

	const UInt16 chunk_version;


	
	//links

	const AlreadyRetained <Module> module;



	//debug
	
	Reflex::Detail::ThreadValidator <REFLEX_DEBUG> threadvalidator;



protected:

	//types

	struct RtConfig
	{
		ConstAlreadyRetained <Object> processor;
		ProcessRtFn callback = nullptr;
		Int32 priority = 0;
	};

	class Record;	//use this inside the class, view uses TransactionScope



	//notify

	void Notify();		//notifies Component, Module and Network

	void Update();		//same as above plus schedules OnUpdate


		
	//callbacks

	virtual bool OnCommand(Key32 id, Data::PropertySet & params) { return false; }

	virtual RtConfig OnUpdate(UInt8 flags, const Data::PropertySet & properties, const StreamsView & streams, const RtConfig * previous) { REFLEX_IF_DEBUG(output.Warn(object_t->tname, "default OnUpdate called, consider using SetUpdateFlags(access,0,0)")); return {}; }

	virtual void OnAttachModule() {}

	virtual void OnDetachModule() {}

	virtual void OnAttachNetwork(Network &) {}	//requires kFlagNetworkAttachment

	virtual void OnDetachNetwork(Network &) {}	//requires kFlagNetworkAttachment

	virtual void OnCollectResources(File::ResourcePool::Lock & lock, Resources & resources) const {}

	virtual void OnRelocateResource(File::ResourcePool::Lock & lock, Address from, const WString & to) {}

	
	void OnRestore(Data::Archive::View & stream, Key32 context) override { REFLEX_ASSERT(false); }

	void OnStore(Data::Archive & stream) const override { REFLEX_ASSERT(false); }

	virtual bool OnImport(UInt16 version, Data::Archive::View & stream, Key32 context) { return false; };


	void OnStoreAction(Key32 action, Data::Archive::View param, Data::Archive & stream, bool redo) const override;

	void OnRestoreAction(Key32 action, Data::Archive::View param, Data::Archive::View & stream, bool redo) override;


	
private:

	typedef Item <Component> ItemBase;

	friend ItemBase;

	friend class Network;

	friend class Module;


	bool OnQueryAddress(Data::Archive & address) const override final;

	void OnComplete(bool recorded, bool redo) override final;

	
	[[deprecated]] virtual void OnRefresh(ProcessRtFn & callback, Reference <Object> & data) final {}
	[[deprecated]] virtual void OnSetVariable(Key32 id, Value32 value) final {}
	[[deprecated]] virtual Int32 GetPriority() final { return 0; }

	void OnAttach(List & list);

	void OnDetach(List & list);


	using Item::Detach;
	using Item::Attach;
	using Item::InsertAfter;
	using Item::InsertBefore;
	using Item::SendBottom;
	using Item::SendTop;

	using StateMt::Notify;



	UInt8 m_flags;

	UInt8 m_update_flags;

	UInt8 m_update_mask;

	UInt8 m_restore_tid;

	UInt32 m_init_point;

	Data::Archive m_init_state;

	Optional <RtConfig> m_rt_config;

	REFLEX_IF_DEBUG(bool m_destructing;)

};




//
//Component::Record

class Netx::Component::Record : public Component::Action
{
public:

	//lifetime

	Record(Component & component);

	Record(Component & component, Key32 action, Data::Archive::View params = {});

};




//
//impl

inline void Netx::Component::SetUpdateFlags(Access, RefreshFlags flags, RefreshFlags mask)
{
#if REFLEX_DEBUG
	//constexpr UInt32 kAllowed = kRefreshFlagModuleUpdateGroup0 | kRefreshFlagModuleUpdateGroup1 | kRefreshFlagModuleUpdateGroup2 | kRefreshFlagModuleUpdateGroup3 | kRefreshFlagModuleUpdateGroup4;
	//REFLEX_ASSERT((flags & ~kAllowed) == 0);
#endif

	m_update_flags = flags;

	m_update_mask = mask;
}

inline bool Netx::Component::SendCommand(Key32 id, Data::PropertySet & params)
{
	if (module)
	{
		return OnCommand(id, params);
	}

	return false; 
}
