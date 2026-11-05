#pragma once

#include "../../factory.h"




//
//Detail

namespace Netx::Detail
{

	class Group;

}




//
//Detail::Group 

class Netx::Detail::Group : public Netx::Component
{
public:

	REFLEX_OBJECT(Netx::Detail::Group, Netx::Component);

	template <bool CONST> class ItrImpl;
	using Itr = ItrImpl<false>;
	using ConstItr = ItrImpl<true>;

	
	//lifetime
	
	Group(ConstWillRetain<Factory> factory);
	~Group();


	//content

	Reference<Object> Clear(bool recycle);
	
	AlreadyRetained<Module> AddModule(ClassID classid);
	UInt32 AddModule(WillRetain<Module> module);
	void RemoveModule(UInt32 uid);
	
	bool Empty() const;
	void Enumerate(const Function<void(Module&)> & callback) const;
	Module * QueryModuleByID(UInt32 uid) const;

	
	//module actions
	
	void ResetModule(UInt32 module_id);


	//connections

	void AddPin(UInt32 module_id, Scope scope, ClassID classid);	//module requires dynamic_pins
	void RemovePin(Pin & pin);
	void ReorderPin(UInt32 module_id, Scope scope, UInt from, UInt to);

	bool CanConnect(const Pin & from, const Pin & to) const;
	bool Connect(Pin & from, Pin & to);
	void Disconnect(Pin & pin);


	//iterations
	
	Itr begin();
	Itr end();
	ConstItr begin() const;
	ConstItr end() const;

	
	
	//links
	
	const ConstAlreadyRetained<Factory> factory;


private:

	struct Recycler;
	struct PendingSource
	{
		Reference<Pin> source;
		Array<Reference<Pin>> dests;
	};

	void OnAttachNetwork(Network & network) override;
	void OnDetachNetwork(Network & network) override;
	void OnStore(Data::Archive & stream) const override;
	void OnRestore(Data::Archive::View & stream, Key32 context) override;
	void OnStoreAction(Key32 action, Data::Archive::View param, Data::Archive & stream, bool redo) const override;
	void OnRestoreAction(Key32 action, Data::Archive::View param, Data::Archive::View & stream, bool redo) override;
	bool OnCommand(Key32 cmd, Data::PropertySet & args) override;

	void UpdateOutputModes();
	static bool ConnectInternal(Group & group, Pin & from, Pin & to);
	static void StoreConnections(const Group & group, Data::Archive & stream);
	static void RestoreConnections(Group & group, Data::Archive::View & stream);
	static void ApplyRemoveModule(Group & group, UInt32 uid);
	static void ApplyPendingConnections(Group & group);
	static void RemovePendingConnections(Group & group, const Pin & pin);
	static void RemovePendingConnections(Group & group, const Module & module);

	
	ConstReference<Data::PropertySet> m_default_args;
	ConstAlreadyRetained<Factory> m_current_factory;
	bool m_has_network;
	UInt32 m_module_uid_counter;
	Array <Reference<Module>> m_modules;
	Array <PendingSource> m_pending_connections;
	mutable UInt32 m_iterating;
};




//
//impl

template <bool CONST>
class Netx::Detail::Group::ItrImpl
{
public:

	using Type = ConditionalType <CONST, const Module, Module>;

	ItrImpl(ConditionalType <CONST, const Group, Group> & group, UInt idx)
		: iterating(group.m_iterating)
		, m_pointer(RemoveConst(group).m_modules.GetData() + idx)
	{
		iterating++;
	}

	ItrImpl(ItrImpl && t)
		: iterating(t.iterating)
		, m_pointer(t.m_pointer)
	{
		iterating++;
	}

	ItrImpl(const ItrImpl & t) = delete;
	~ItrImpl() { iterating--; }

	Type & operator*() const { return **m_pointer; }
	Type * operator->() const { return m_pointer->Adr(); }

	ItrImpl & operator++() { m_pointer++; return *this; }

	bool operator==(const ItrImpl & other) const { return m_pointer == other.m_pointer; }
	bool operator!=(const ItrImpl & other) const { return m_pointer != other.m_pointer; }

	ItrImpl & operator=(const ItrImpl & other) = delete;



private:

	UInt32 & iterating;

	Reference <Module> * m_pointer;
};

inline Netx::Detail::Group::Itr Netx::Detail::Group::begin()
{
	return { *this, 0 };
}

inline Netx::Detail::Group::Itr Netx::Detail::Group::end()
{
	return { *this, m_modules.GetSize() };
}

inline Netx::Detail::Group::ConstItr Netx::Detail::Group::begin() const
{
	return { *this, 0 };
}

inline Netx::Detail::Group::ConstItr Netx::Detail::Group::end() const
{
	return { *this, m_modules.GetSize() };
}
