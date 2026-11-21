#pragma once

#include "../core.h"




//
//declarations

REFLEX_NS(Netx::Detail)

struct iHistory;

struct iUndoable;

REFLEX_END




//
//Detail::iHistory

struct Netx::Detail::iHistory
{
public:

	UInt GetNumScope() const { return counts.nscope; }



protected:

	friend struct iUndoable;


	struct TransactionScope;
		
	struct UndoRedoState : public Object
	{
		UndoRedoState(Data::Archive && address, Key32 action, const Data::Archive::View & param)
			: action(action), 
			address(std::move(address)), 
			param(param)
		{
		}

		Key32 action;

		Data::Archive address, param, undo[2];
	};

	struct Records : public Array < Reference <UndoRedoState> >
	{
		Records(const WString::View & desc) : desc(desc) {}

		WString desc;
	};

	virtual void OnCommit(const Records & records) = 0;


	struct Counts
	{
		UInt16 nscope = 0;
		UInt16 nstateaction = 0;
	}
	counts;

	Records * m_records = nullptr;

}; 




//
//Detail::iUndoable

struct Netx::Detail::iUndoable
{
protected:

	struct Action;


	iUndoable()
		: ihistory(g_null)
	{
	}

	void Connect(iHistory & super)
	{
		RemoveConst(ihistory) = super;
	}

	void Disconnect()
	{
		RemoveConst(ihistory) = g_null;
	}

	
	virtual bool OnQueryAddress(Data::Archive & address) const = 0;

	virtual void OnStore(Data::Archive & stream) const = 0;

	virtual void OnRestore(Data::Archive::View & stream, Key32 context) = 0;

	virtual void OnStoreAction(Key32 action, Data::Archive::View param, Data::Archive & stream, bool redo) const = 0;

	virtual void OnRestoreAction(Key32 action, Data::Archive::View param, Data::Archive::View & stream, bool redo) = 0;

	virtual void OnComplete(bool recorded, bool redo) = 0;



	const Ref <iHistory> ihistory;



private:

	friend class Netx::Network;

	friend struct iHistory;

	struct NullHistory : public iHistory
	{
		void OnCommit(const Records & records) override {}
	};

	static NullHistory g_null;
};




//
//Detail::iUndoable::Action

struct Netx::Detail::iUndoable::Action
{
public:

	//lifetime

	Action(iUndoable & target, Key32 action, Data::Archive::View params);

	Action(iUndoable & target, Data::Archive::View params) : Action(target, kMaxUInt32, params) {};

	~Action();

	
	
	//access
	
	void Abort();



	//info

	const Ref <iUndoable> object;

	const bool state_action;	//true if entire state change


private:

	void * operator new(size_t) = delete;

	bool Init();

	void StoreUndoRedo(iHistory::UndoRedoState & state, bool redo);


	bool m_active;

	iHistory::UndoRedoState * m_state = nullptr;
};




//
//Detail::iHistory::TransactionScope

struct Netx::Detail::iHistory::TransactionScope
{
public:

	REFLEX_NONCOPYABLE(TransactionScope);

	TransactionScope(iHistory & network, const WString::View & desc);

	~TransactionScope();

	iHistory & network;



private:

	void * operator new(size_t) = delete;

	void Init(const WString::View & desc);

	void Deinit();

};





//
//impl

REFLEX_INLINE Netx::Detail::iHistory::TransactionScope::TransactionScope(iHistory & network, const WString::View & desc)
	: network(network)
{
	if (++(network.counts.nscope) == 1) Init(desc);
}

REFLEX_INLINE Netx::Detail::iHistory::TransactionScope::~TransactionScope()
{
	if (--(network.counts.nscope) == 0) Deinit();
}
