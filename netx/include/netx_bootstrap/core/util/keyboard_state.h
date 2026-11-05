#pragma once

#include "../[require].h"




//
//Primary API

namespace Netx
{

	template <class ... VARGS> class KeyboardState;

}




//
//KeyboardState 

template <class ... VARGS>
class Netx::KeyboardState : public StateMt
{
public:

	typedef Tuple <VARGS...> Tuple;

	typedef ConditionalType <sizeof...(VARGS) == 1,typename Reflex::TupleElement<Tuple,0>::Type,Tuple> Type;



	//declarations
	
	class Iterator;

	
	
	//lifetime
	
	KeyboardState();



	//read
	
	bool Empty() const;

	explicit operator bool() const;


	UInt8 GetLowest() const;

	UInt8 GetHighest() const;


	const Flags64 * GetState() const;

	bool CheckKey(UInt8 note) const;

	const Type & GetKeyValue(UInt8 note) const;


	
	//write
	
	void Clear();


	void Trigger(UInt8 note, const VARGS & ... values);

	void Release(UInt8 note);

	template <bool... TUPLE_FIELDS> void Release(UInt8 note);



protected:

	void SetKeyValue(UInt8 note, const VARGS & ... values);


	static void ClearState(Flags64 keys[2], UInt8 note);

	static void SetState(Flags64 keys[2], UInt8 note, bool value);

	static bool CheckState(const Flags64 keys[2], UInt8 note);



private:

	Flags64 m_keys[2];

	Type m_values[128];

};

REFLEX_SET_TRAIT_TEMPLATED(Netx::KeyboardState, IsBoolCastable);




//
//KeyboardState::Iterator

template <class ... VARGS>
class Netx::KeyboardState<VARGS...>::Iterator
{
public:

	//lifetime
	
	Iterator(const KeyboardState & keyboard);

	Iterator(const Flags64 state[2]);



	//read
	
	explicit operator bool() const;


	
	//traverse
	
	Idx GetNext();



private:

	Flags64 m_keys[2];

};




//
//implementation

template <class ... VARGS> inline Netx::KeyboardState<VARGS...>::KeyboardState()
{
	if constexpr ((sizeof...(VARGS) == 1) && kIsRawConstructible<Type>)	//dont need to init Tuple, does it self
	{
		MemClear(m_values, sizeof(m_values));
	}
}

template <class ... VARGS> REFLEX_INLINE bool Netx::KeyboardState<VARGS...>::Empty() const
{
	return Not(m_keys[0].GetWord() | m_keys[1].GetWord());
}

template <class ... VARGS> REFLEX_INLINE Netx::KeyboardState<VARGS...>::operator bool() const
{
	return True(m_keys[0].GetWord() | m_keys[1].GetWord());
}

template <class ... VARGS> REFLEX_INLINE UInt8 Netx::KeyboardState<VARGS...>::GetLowest() const
{
	REFLEX_LOOP(idx, 2)
	{
		auto & keys = m_keys[idx];

		if (Idx bit = keys.GetFirst())
		{
			return UInt8((idx * 64) + bit.value);
		}
	}

	return -1;
}

template <class ... VARGS> REFLEX_INLINE UInt8 Netx::KeyboardState<VARGS...>::GetHighest() const
{
	REFLEX_RLOOP(idx, 2)
	{
		auto & keys = m_keys[idx];

		if (Idx bit = keys.GetLast())
		{
			return UInt8((idx * 64) + bit.value);
		}
	}

	return -1;
}

template <class ... VARGS> REFLEX_INLINE const Flags64 * Netx::KeyboardState<VARGS...>::GetState() const
{
	return m_keys;
}

template <class ... VARGS> REFLEX_INLINE bool Netx::KeyboardState<VARGS...>::CheckKey(UInt8 note) const
{
	return m_keys[(note / 64) & 1].Check(note & 63);
}

template <class ... VARGS> REFLEX_INLINE const typename Netx::KeyboardState<VARGS...>::Type & Netx::KeyboardState<VARGS...>::GetKeyValue(UInt8 note) const
{
	return m_values[note];
}

template <class ... VARGS> REFLEX_INLINE void Netx::KeyboardState<VARGS...>::Clear()
{
	m_keys[0].Clear();
	
	m_keys[1].Clear();

	Notify();
}

template <class ... VARGS> REFLEX_INLINE void Netx::KeyboardState<VARGS...>::Trigger(UInt8 note, const VARGS & ... values)
{
	SetState(m_keys, note, true);

	SetKeyValue(note, values...);

	Notify();
}

template <class ... VARGS> REFLEX_INLINE void Netx::KeyboardState<VARGS...>::Release(UInt8 note)
{
	ClearState(m_keys, note);

	m_values[note] = {};

	Notify();
}

template <class ... VARGS> template <bool... TUPLE_FIELDS> REFLEX_INLINE void Netx::KeyboardState<VARGS...>::Release(UInt8 note)
{
	REFLEX_STATIC_ASSERT(sizeof...(TUPLE_FIELDS) == sizeof...(VARGS) && (sizeof...(TUPLE_FIELDS) < 5));

	ClearState(m_keys, note);

	auto & values = m_values[note];

	constexpr bool flags[] = { TUPLE_FIELDS... };

	if constexpr (sizeof...(TUPLE_FIELDS) >= 1)
	{
		if (flags[0]) values.a = {};
	}
	
	if constexpr (sizeof...(TUPLE_FIELDS) >= 2)
	{
		if (flags[1]) values.b = {};
	}

	if constexpr (sizeof...(TUPLE_FIELDS) >= 3)
	{
		if (flags[2]) values.c = {};
	}

	if constexpr (sizeof...(TUPLE_FIELDS) == 4)
	{
		if (flags[3]) values.d = {};
	}

	Notify();
}

template <class ... VARGS> REFLEX_INLINE void Netx::KeyboardState<VARGS...>::SetKeyValue(UInt8 note, const VARGS & ... values)
{
	m_values[note] = { values... };
}

template <class ... VARGS> REFLEX_INLINE void Netx::KeyboardState<VARGS...>::ClearState(Flags64 keys[2], UInt8 note)
{
	REFLEX_ASSERT(note < 128);

	keys[note >> 6].Clear(note & 63);
}

template <class ... VARGS> REFLEX_INLINE void Netx::KeyboardState<VARGS...>::SetState(Flags64 keys[2], UInt8 note, bool value)
{
	REFLEX_ASSERT(note < 128);

	keys[note >> 6].Set(note & 63, value);
}

template <class ... VARGS> REFLEX_INLINE bool Netx::KeyboardState<VARGS...>::CheckState(const Flags64 keys[2], UInt8 note)
{
	REFLEX_ASSERT(note < 128);

	return keys[note >> 6].Check(note & 63);
}




//
//implementation

template <class ... VARGS> REFLEX_INLINE Netx::KeyboardState<VARGS...>::Iterator::Iterator(const KeyboardState & keyboard)
	: Iterator(keyboard.m_keys)
{
}

template <class ... VARGS> REFLEX_INLINE Netx::KeyboardState<VARGS...>::Iterator::Iterator(const Flags64 state[2])
{
	m_keys[0] = state[0];

	m_keys[1] = state[1];
}

template <class ... VARGS> REFLEX_INLINE Netx::KeyboardState<VARGS...>::Iterator::operator bool() const
{
	return True(m_keys[0].GetWord() | m_keys[1].GetWord());
}

template <class ... VARGS> REFLEX_INLINE Idx Netx::KeyboardState<VARGS...>::Iterator::GetNext()
{
	REFLEX_LOOP (idx, 2)
	{
		auto & keys = m_keys[idx];

		if (Idx bit = keys.GetFirst())
		{
			keys.Clear(bit.value & 63);

			return (idx * 64) + bit.value;
		}
	}

	return -1;
}
