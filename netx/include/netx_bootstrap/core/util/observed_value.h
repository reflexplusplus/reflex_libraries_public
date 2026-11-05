#pragma once

#include "../[require].h"




//
//Secondary API

namespace Netx
{

	template <class TYPE> class ObservedValue;

}




//
//ObservedValue 

template <class TYPE>
class Netx::ObservedValue
{
public:

	static constexpr bool kAtomic = Reflex::kIsRawCopyable <TYPE> && (sizeof(TYPE) == 4 || sizeof(TYPE) == 8);


	
	//lifetime

	ObservedValue() = default;

	ObservedValue(const TYPE & value);



	//writer thread (i.e. audio)

	void Write(Access, const TYPE & value);

	decltype(auto) Read(Access) const;



	//reader thread (i.e. UI)

	TYPE Read() const;	//
	


private:

	struct SeqLock
	{
		SeqLock() : seq(0) {}

		SeqLock(const TYPE & value) : seq(0), value(value) {}

		mutable std::atomic <UInt32> seq;
	
		TYPE value = {};
	};

	ConditionalType <kAtomic, std::atomic<TYPE>, SeqLock> m_value;

};




//
//impl

template <class TYPE> inline Netx::ObservedValue<TYPE>::ObservedValue(const TYPE & value)
	: m_value(value)
{
}

template <class TYPE> inline void Netx::ObservedValue<TYPE>::Write(Access, const TYPE & value)
{
	if constexpr (kAtomic)
	{
		m_value.store(value, std::memory_order_relaxed);
	}
	else
	{
		m_value.seq.fetch_add(1, std::memory_order_relaxed);	// odd = writing

		std::atomic_thread_fence(std::memory_order_release);

		m_value.value = value;

		std::atomic_thread_fence(std::memory_order_release);

		m_value.seq.fetch_add(1, std::memory_order_release);	// even = published

	}
}

template <class TYPE> REFLEX_INLINE decltype(auto) Netx::ObservedValue<TYPE>::Read(Access) const
{
	if constexpr (kAtomic)
	{
		return m_value.load(std::memory_order_relaxed);
	}
	else
	{
		return m_value.value;
	}
}

template <class TYPE> inline TYPE Netx::ObservedValue<TYPE>::Read() const
{
	if constexpr (kAtomic)
	{
		return m_value.load(std::memory_order_relaxed);
	}
	else
	{
		for (;;)
		{
			auto a = m_value.seq.load(std::memory_order_acquire);

			if (a & 1u) continue;

			auto value = m_value.value;

			auto b = m_value.seq.load(std::memory_order_acquire);

			if (a == b) return value;
		}
	}
}