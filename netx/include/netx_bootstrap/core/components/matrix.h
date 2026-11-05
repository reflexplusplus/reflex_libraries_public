#pragma once

#include "../[require].h"




//
//Primary API

namespace Netx
{

	class Matrix;

}




//
//Matrix

class Netx::Matrix : public Component
{
public:

	//types

	typedef SIMD::FloatV4 FloatV4;

	class Map;

	struct Connection;

	struct Transaction;

	class Processor;



	NETX_COMPONENT(Netx, Matrix, 3264812282ul, const Map & map, Key32 id = kNullKey);

	static Matrix & null;



	//transaction

	Reference <Transaction> BeginTransaction(UInt slot);

	virtual void SetConnection(Transaction & t, const Connection & connection) = 0;



	//access

	virtual void Clear() = 0;

	virtual void AddConnection(const Connection & connection) = 0;

	virtual void RemoveConnection(UInt slotidx) = 0;


	virtual ArrayView <Connection> GetConnections() const = 0;


	
	//links

	const ConstAlreadyRetained <Map> map;

};




//
//Matrix::Map

class Netx::Matrix::Map : public Object
{
public:

	static Map & null;

	//types
	
	struct Source;
	
	struct Dest;



	//lifetime
	
	[[nodiscard]] static Unretained <Map> Create();



	//setup

	virtual void RegisterSource(Key32 id, WString && label, bool dynamic, bool bipolar, UIntNative offset) = 0;

	virtual void RegisterDest(Key32 id, WString && label, bool dynamic, Float range, UIntNative offset) = 0;

	virtual void RegisterDest(Key32 id, WString && label, bool dynamic, UIntNative offset) = 0;



	//access

	virtual ArrayView <Source> GetSources() const = 0;

	virtual ArrayView <Dest> GetDests() const = 0;

};




//
//Matrix::Map::Source

struct Netx::Matrix::Map::Source
{
	Key32 id;

	WString label;

	bool dynamic = false;

	bool bipolar = false;

	UInt data_offset = 0;
};




//
//Matrix::Map::Dest

struct Netx::Matrix::Map::Dest
{
	Key32 id;

	WString label;

	bool dynamic = false;

	bool multiplicative = false;

	Float range = 0.0f;

	UInt data_offset = 0;
};




//
//Matrix::Connection

struct Netx::Matrix::Connection
{
	Connection();

	Connection(Key32 sourceid, Key32 destid, Float amount = 0.0f);

	bool enable;

	Key32 src, dst;

	Float amount;
};




//
//Matrix::Transaction

struct Netx::Matrix::Transaction : 
	public Object,
	public Module::TransactionScope
{
	REFLEX_OBJECT(Transaction, Object);

	Transaction(Matrix & matrix, UInt idx);

	~Transaction();

	Matrix & matrix;

	Component::Record record;

	const UInt idx;
};




//
//Matrix::Processor

class Netx::Matrix::Processor : public Object
{
public:

	//lifetime
	
	Processor(Access access, const Matrix & matrix);
	

	
	//apply

	template <class VOICEBLOCK> void Prepare(VOICEBLOCK & voiceblock) const;			//resets all dests

	template <class VOICEBLOCK> void Init(VOICEBLOCK & voiceblock, UInt vidx) const;	//on start voice, initalises statics & dynamics

	template <class VOICEBLOCK> void Apply(VOICEBLOCK & voiceblock) const;				//on process, sets dynamics



private:

	struct Adder
	{
		UInt source_offset;

		UInt dest_offset;

		FloatV4 amount;
	};

	struct Mult
	{
		UInt source_offset;

		UInt dest_offset;

		SIMD::BoolV4 bipolar;

		SIMD::BoolV4 inverse;

		FloatV4 amount;
	};

	void Init_(UInt8 * voiceblock, UInt vidx) const;

	void Apply_(UInt8 * voiceblock) const;


	typedef Array < Pair<FloatV4,UInt> > Initalisers;

	Initalisers m_all, m_active_dynamic;		//initalisation

	Array <Adder> m_static_adders, m_dynamic_adders;

	Array <Mult> m_static_mults, m_dynamic_mults;


	static const SIMD::FloatV4 st_rcp_two;

};




//
//helpers

namespace Netx
{

	const Matrix::Map::Source * GetSourceByID(const Matrix::Map & map, Key32 id);

	const Matrix::Map::Dest * GetDestByID(const Matrix::Map & map, Key32 id);

}




//
//impl

template <class VOICEBLOCK> REFLEX_INLINE void Netx::Matrix::Processor::Prepare(VOICEBLOCK & voiceblock) const
{
	Prepare_(Reinterpret<UInt8>(&voiceblock));
}

template <class VOICEBLOCK> REFLEX_INLINE void Netx::Matrix::Processor::Init(VOICEBLOCK & voiceblock, UInt vidx) const
{
	Init_(Reinterpret<UInt8>(&voiceblock), vidx);
}

template <class VOICEBLOCK> REFLEX_INLINE void Netx::Matrix::Processor::Apply(VOICEBLOCK & voiceblock) const
{
	Apply_(Reinterpret<UInt8>(&voiceblock));
}

REFLEX_INLINE Netx::Matrix::Connection::Connection()
	: enable(false)
	, amount(0.0f)
{
}

REFLEX_INLINE Netx::Matrix::Connection::Connection(Key32 src, Key32 dst, Float amount)
	: enable(true)
	, src(src)
	, dst(dst)
	, amount(amount)
{
}
