#pragma once

#include "abstract_propertyset.h"




//
//Primary API

namespace Netx
{

	class ParameterSet;

}




//
//ParameterSet

class Netx::ParameterSet : public AbstractPropertySet
{
public:

	NETX_COMPONENT(Netx, ParameterSet, K32("ParameterSet"), Key32 id = kNullKey);

	static ParameterSet & null;


	
	//types

	class Allocation;

	class Processor;



	//lifetime

	virtual ~ParameterSet();



	//content
	
	virtual void Init(Access access, Module & module, WillRetain <Allocation> allocation);

	virtual void Populate(Access access, WillRetain <Allocation> allocation);

	void SetProperties(Access access, UInt idx, ConstWillRetain <PropertyDef> info);


	
	//transaction

	Unretained <Transaction> BeginEx(UInt32 idx, bool notify = true);	//for VM

	Reference <Transaction> Begin(UInt32 idx, bool notify = true) { return BeginEx(idx, notify); }



	//non-transacted

	void SetValueMt(Access access, UInt idx, Value32 value);		//engine access, ui should use Parameter or Transaction
 


	//links

	AlreadyRetained <Allocation> GetAllocation(Access access) const { return m_allocation.RemoveConst(); }

	ConstAlreadyRetained <Allocation> GetAllocation() const { return m_allocation; }

	const UInt editcount;	//parameter definitions/allocation revision, not value edits



protected:

	void OnRestore(Data::Archive::View & stream, Key32 context) override;

	void OnStore(Data::Archive & stream) const override;


	void OnStoreAction(Key32 action, Data::Archive::View param, Data::Archive & stream, bool redo) const override;

	void OnRestoreAction(Key32 action, Data::Archive::View param, Data::Archive::View & stream, bool redo) override;


	bool OnCommand(Key32 action, Data::PropertySet & args) override;


	void OnUnsetProperty(Address address) override;

	void OnSetProperty(Address address, Object & value) override;


	using StateMt::Notify;

	Reference <Allocation> m_allocation;

};




//
//ParameterSet::Allocation 

class Netx::ParameterSet::Allocation : public Object
{
public:

	REFLEX_OBJECT(Netx::ParameterSet::Allocation, Object);

	static Allocation & null;


	using Definition = Tuple <Key32, ConstWillRetain<PropertyDef>, UInt8>;
	
	template <class TYPE> using ConstPointer = TYPE * const;

	
	
	//lifetime

	[[nodiscard]] static Unretained <Allocation> Create(ArrayView <Definition> definitions, Allocator & allocator = g_default_allocator);

	~Allocation();



	//config

	void SetProperties(UInt idx, ConstWillRetain <PropertyDef> properties);



	//access

	bool SetValueMt(UInt idx, Value32 value);



	//SPECIAL
	
	void UpdateMt(UInt8 flags); //set change flags

	void UpdateMt();	//set all change flags

	UInt8 PollRt();		//only **1** audio engine client should do this



	const AlreadyRetained <ParameterSet> parameterset;

	const UInt size;

	ConstPointer <const Key32> ids;

	ConstPointer <const ConstAlreadyRetained <PropertyDef>> properties;

	ConstPointer <const UInt8> changeflags;

	ConstPointer <const Value32> values;



protected:

	struct Storage
	{
		ArrayRegion <ConstAlreadyRetained<PropertyDef>> properties;
		ArrayRegion <Key32> ids;
		ArrayRegion <Value32> values;
		ArrayRegion <UInt8> changeflags;
	};

	Allocation(UInt size);

	static Storage GetStorage(Allocation * self, UInt size);

	Storage GetStorage() { return GetStorage(this, size); }



private:

	friend Reflex::Detail::Constructor <Allocation>;

	friend ParameterSet;

	friend Processor;

	Allocation(UInt size, const Storage & storage);


	AtomicUInt32 m_changeflags;	//optimisation, *only* one client should do PollWord on this 

	UInt8 m_num_processor;

	UIntNative m_reserved;	//data starts here

};

REFLEX_SET_TRAIT(Netx::ParameterSet::Allocation, IsAbstract);




//	
//ParameterSet::Processor

class Netx::ParameterSet::Processor : public Object
{
public:

	static Processor & null;



	//lifetime

	Processor(Allocation & allocation);

	~Processor();



	//check (for optimisation, this is *not* per-processor.  Only create 1 active Processor at a time)

	UInt8 Poll();



	//allocation

	const AlreadyRetained <Allocation> allocation;

	const ArrayView <Value32> values;

};




//
//impl

inline Unretained <Netx::ParameterSet> Netx::ParameterSet::Create(Key32 id)
{
	return REFLEX_CREATE(ParameterSet, id);
}

REFLEX_INLINE void Netx::ParameterSet::SetProperties(Access access, UInt idx, ConstWillRetain <PropertyDef> info)
{
	m_allocation->SetProperties(idx, info);
	
	RemoveConst(editcount)++;
	
	Component::Notify();
}

REFLEX_INLINE void Netx::ParameterSet::SetValueMt(Access access, UInt idx, Value32 value)
{
	m_allocation->SetValueMt(idx, value);
}

REFLEX_INLINE UInt8 Netx::ParameterSet::Allocation::PollRt()
{
	return m_changeflags.exchange(0);
}

inline Netx::ParameterSet::Processor::Processor(Allocation & allocation)
	: allocation(allocation)
	, values({ allocation.values, allocation.size })
{
	Retain(allocation);
}

inline Netx::ParameterSet::Processor::~Processor()
{
	Release(allocation);
}
