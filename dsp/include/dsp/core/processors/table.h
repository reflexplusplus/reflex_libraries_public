#pragma once

#include "noise.h"




//
//Public API

REFLEX_NS(DSP)

class Table;

void Slice(Table & table, Float32 start, Float32 range, Float32 step);

REFLEX_END
			



//
//Table

class DSP::Table : 
	public Object,
	public State
{
public:

	REFLEX_OBJECT(DSP::Table, Object);



	//types

	struct Value
	{
		Value(Float y = 0.0f);

		Value(Float curve, Float y);

		UInt16 type;

		Int16 shape;

		Float y;
	};

	class Processor;



	//lifetime

	Table();

	Table(const Table & value);

	Table(Table && temp);

	~Table();



	//access

	void SetLength(Float length);

	Float GetLength() const;


	void Clear() { Reset(GetLength()); }

	void ClearRange(Float32 position, Float32 length);

	void EnumerateRange(Float32 position, Float32 length, const Function <void(Float32,Value)> & visitor) const;

	void EditRange(Float32 position, Float32 length, const Function <void(Float32,Value&)> & visitor);


	void Insert(Float32 position, Value value);

	void RemoveValue(Float32 position);


	Pair <Float32,Value> LookupValue(Float32 position) const;

	const Pair <Float32> GetBounds(Float32 position) const;



	//assign
	
	Table & operator=(const Table & table);



	//stream

	void Reset(Float32 length);

	void Serialize(Data::Archive & stream) const;

	void Deserialize(Data::Archive::View & stream);



protected:

	using State::State;

	
	Float32 m_length;
	
	Sequence <Float32,Value> m_data;

};

REFLEX_STATIC_ASSERT(kIsRawCopyable<DSP::Table::Value>);




//
//processor

class DSP::Table::Processor : public Object
{
public:

	REFLEX_OBJECT(DSP::Table::Processor, Object);

	static Processor & null;



	//lifetime

	[[nodiscard]] static Unretained <Processor> Create(UInt ppq, const DSP::Table & table, bool wrap);



	//access

	virtual UInt GetLength() const = 0;

	virtual Float Read(UInt32 position) const = 0;

	virtual FloatV4 Read(const IntV4 & position) const = 0;

};




//
//impl

REFLEX_INLINE DSP::Table::Value::Value(Float y)
	: type(0)
	, shape(0)
	, y(y)
{
}

REFLEX_INLINE DSP::Table::Value::Value(Float curve, Float y)
	: type(1)
	, shape(Int16(Truncate(curve * kMaxInt16)))
	, y(y)
{
}

REFLEX_INLINE Float32 DSP::Table::GetLength() const 
{
	return m_length;
}
