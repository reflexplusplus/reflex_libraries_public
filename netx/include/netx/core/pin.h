#pragma once

#include "stream.h"




//
//Core::Pin

class Netx::Core::Pin : 
	public Reflex::Item <Pin>,
	public Identity,
	public iClass,
	public Protected
{
public:

	REFLEX_OBJECT(Pin,Object);

	static Pin & null;

	

	//types

	enum Mode : UInt8
	{
		kModeAuto,
		kModeRead,
		kModeWrite,
		kModeAlwaysWrite
	};



	//lifetime

	Pin();	//for VMNULL
	
	Pin(ClassID classid, Scope scope, Mode mode = kModeAuto);

	virtual ~Pin();



	//location

	void Attach(Access access, Module & module);

	void Detach(Access access);



	//position

	void SendBottom(Access);

	void SendTop(Access);

	void SetIndex(Access, UInt position);

	UInt GetIndex() const;

	void SetMode(Access, Mode mode);

	bool IsWriteable() const { REFLEX_ASSERT(mode != kModeAuto); return mode >= kModeWrite; }



	//connect

	void RemoveSource(Access);

	void RemoveDestinations(Access);

	void Disconnect(Access);


	bool AddDestination(Access, Pin & destination);



	//info

	Pin * GetSource();

	const Pin * GetSource() const;


	ArrayView <AlreadyRetained<Pin>> GetDestinations();

	ArrayView <ConstAlreadyRetained<Pin>> GetDestinations() const;



	//links

	const AlreadyRetained <Module> module;

	const Scope scope;

	const Mode mode;



private:

	friend Item;

	friend class Module;

	friend class Network;


	typedef Tuple < FunctionPointer <Unretained<Stream>(const Data::PropertySet&,bool)>, Reference <Stream> > StreamType;

	
	void QueueUpdate();

	void OnAttach(List & list);

	void OnDetach(List & list);


	StreamType * m_streamtype;

	Pair <ClassID,UInt32> m_validator;


	Pin * m_source;

	Array <Pin*> m_destinations;


	Reference <Stream> m_stream_cache;

};




//
//impl

inline Netx::Core::Pin::Pin()
	: Pin(0, kScopeInput)
{
}

REFLEX_INLINE Netx::Core::Pin * Netx::Core::Pin::GetSource()
{	
	return m_source;
}

REFLEX_INLINE const Netx::Core::Pin * Netx::Core::Pin::GetSource() const
{	
	return m_source;
}

REFLEX_INLINE ArrayView <AlreadyRetained<Netx::Core::Pin>> Netx::Core::Pin::GetDestinations()
{
	return Reinterpret<Array<AlreadyRetained<Netx::Core::Pin>>>(m_destinations);
}

REFLEX_INLINE ArrayView <ConstAlreadyRetained<Netx::Core::Pin>> Netx::Core::Pin::GetDestinations() const
{
	return Reinterpret<Array<ConstAlreadyRetained<Netx::Core::Pin>>>(m_destinations);
}

inline void Netx::Core::Pin::Disconnect(Access access)
{
	RemoveSource(access);

	RemoveDestinations(access);
}
