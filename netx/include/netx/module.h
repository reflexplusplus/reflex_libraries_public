#pragma once

#include "component.h"
#include "detail/functions.h"




//
//Module

class Netx::Module : 
	public Core::Module,
	public Core::iClass,
	public Detail::iUndoable,
	public Component::List,
	protected Protected
{
public:

	REFLEX_OBJECT(Netx::Module, Core::Module);

	static Module & null;



	//types
	
	struct TransactionScope;

	typedef AbstractProperty TransactionListener;


	
	//lifetime

	Module(ClassID classid);

	~Module();


	
	//location & id

	void SetAddress(Access access, Module * owner, UInt32 id);	//managed by owner, not persistent

	Module * GetOwner() const;



	UInt32 GetID() const;


	
	//resources

	Component::Resources CollectResources(File::ResourcePool::Lock & lock) const;

	void RelocateResources(File::ResourcePool::Lock & lock, const Map <Address, WString> & resources);



	//state

	void Reset(Key32 context = kStreamContextSession);

	void Deserialize(Data::Archive::View & stream, Key32 context = kStreamContextSession);

	void Serialize(Data::Archive & stream) const;

	Key32 GetStreamContext() const { return m_streamcontext; }



	//transaction

	void SetEditCount(Access access, UInt editcount);

	UInt32 GetEditCount() const { return m_editcount; }

	bool CheckTransacted() const;



	//links
	
	const AlreadyRetained <Network> network;



	//special

	void OnAttachNetwork() override;

	void OnDetachNetwork() override;



private:

	friend class Network;

	friend class Component;

		
	bool OnQueryAddress(Data::Archive & address) const override final;

	void OnStore(Data::Archive & stream) const override final;

	void OnRestore(Data::Archive::View & stream, Key32 context) override final;

	void OnStoreAction(Key32 action, Data::Archive::View param, Data::Archive & stream, bool redo) const override final;

	void OnRestoreAction(Key32 action, Data::Archive::View param, Data::Archive::View & stream, bool redo) override final;
	
	void OnComplete(bool recorded, bool redo) override final;


	void OnSetMainThread() override final;

	Unretained <Object> OnInitialiseProcessor() override final;
	
	Core::Module::RtConfig OnUpdate(const Data::PropertySet & properties, const StreamsView & streams, UInt8 flags, ConstAlreadyRetained <Object> previous_processor) override final;


	void ClearUninstantiatedComponentChunks();



	Module * m_owner;

	UInt32 m_uid;

	UInt32 m_editcount;

	Key32 m_streamcontext;

	UInt8 m_restore_tid;


	Array < Pair <UInt64, Data::Archive> > m_uninstantiated;

	Array < Pair <UInt64, Data::Archive> > * m_puninstantiated;

};




//
//Module::TransactionScope 

struct Netx::Module::TransactionScope : public Network::TransactionScope
{
	REFLEX_NONCOPYABLE(TransactionScope);

	TransactionScope(Module & module, const WString::View & desc)
		: Network::TransactionScope(module.network, desc)
		, module(module)
	{
	}

	const AlreadyRetained <Module> module;
};




//
//impl

inline void Netx::Module::SetAddress(Access access, Module * owner, UInt32 id)
{
	REFLEX_IF_DEBUG(for (auto ancestor = owner; ancestor; ancestor = ancestor->GetOwner()) REFLEX_ASSERT(ancestor != this));
	REFLEX_ASSERT(!owner || id != 0);

	m_owner = owner;
	m_uid = id;
}

inline Netx::Module * Netx::Module::GetOwner() const
{
	return m_owner;
}


inline UInt32 Netx::Module::GetID() const
{
	return m_uid;
}

inline void Netx::Module::Serialize(Data::Archive & stream) const
{
	OnStore(stream);
}

inline void Netx::Module::Deserialize(Data::Archive::View & stream, Key32 context)
{
	OnRestore(stream, context);
}

inline void Netx::Module::SetEditCount(Access access, UInt editcount) 
{
	m_editcount = editcount; 
	
	StateMt::Notify();
}
