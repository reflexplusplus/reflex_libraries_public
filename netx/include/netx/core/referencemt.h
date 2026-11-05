#pragma once

#include "[require].h"




//
//Secondary API

namespace Netx::Core
{

	template <class TYPE> class ReferenceMt;	//guarantees object deletion in owner thread

	template <class TYPE> using ConstReferenceMt = ReferenceMt <const TYPE>;

}




//
//Core::ReferenceMt

template <class TYPE>
class Netx::Core::ReferenceMt
{
public:

	REFLEX_NONCOPYABLE(ReferenceMt);



	//lifetime

	ReferenceMt(TYPE & object = Null<TYPE>());

	ReferenceMt(TYPE * object);

	ReferenceMt(ReferenceMt && lhs);



	//write (main/owner thread)

	void Store(TYPE & ref);	//owner thread

	void Store(TYPE * ref);	//owner thread

	void Purge();



	//read

	AlreadyRetained <TYPE> Load() const;	//owner thread

	Reference <TYPE> LoadMt() const;	//rt/any thread



	//special (main/owner thread)

	void Enumerate(const Function <void(TYPE&)> & callback);

	void Destroy();	//deletes all, can not be used after this



	//debug

	Reflex::Detail::ThreadValidator <REFLEX_DEBUG> threadvalidator;



private:

	Array < Reference <TYPE> > m_cache;

	AtomicPointer m_current_mt;		//std::atomic <void*>
};




//
//impl

REFLEX_NS(Reflex)
template <class TYPE> inline TYPE & Deref(const Netx::Core::ReferenceMt <TYPE> & ref);	//purposely not implemented, use explicit Load
REFLEX_END

template <class TYPE> inline Netx::Core::ReferenceMt<TYPE>::ReferenceMt(TYPE & top)
{
	m_cache.Allocate(4);
	
	m_cache.Push(top);

	REFLEX_ATOMIC_WRITE(m_current_mt, RemoveConst(&top));	//uses std::memory_order_release
}

template <class TYPE> inline Netx::Core::ReferenceMt<TYPE>::ReferenceMt(TYPE * p)
	: ReferenceMt(*p)
{
}

template <class TYPE> REFLEX_INLINE Netx::Core::ReferenceMt<TYPE>::ReferenceMt(ReferenceMt && lhs)
	: m_cache(std::move(lhs.m_cache))
{
	REFLEX_ATOMIC_WRITE(m_current_mt, RemoveConst(m_cache.GetLast().Adr()));	//uses std::memory_order_release
}

template <class TYPE> REFLEX_INLINE void Netx::Core::ReferenceMt<TYPE>::Store(TYPE & top)
{
	REFLEX_ASSERT(threadvalidator);

	Purge();	//!keep this first, for one tick delay. TODO this should only be done once per frame, so should be moved outside here

	Reference <TYPE> temp(top);

	if (auto idx = Search(m_cache, top))
	{
		m_cache.Remove(idx.value);
	}

	m_cache.Push(top);

	REFLEX_ATOMIC_WRITE(m_current_mt, RemoveConst(&top));	//!last as barrier for all previous writes, uses std::memory_order_release
}

template <class TYPE> REFLEX_INLINE void Netx::Core::ReferenceMt<TYPE>::Store(TYPE * ptr)
{
	REFLEX_ASSERT(ptr);

	Store(*ptr);
}

template <class TYPE> REFLEX_INLINE AlreadyRetained <TYPE> Netx::Core::ReferenceMt<TYPE>::Load() const
{
	REFLEX_ASSERT(threadvalidator);

	return m_cache.GetLast();
}

template <class TYPE> inline void Netx::Core::ReferenceMt<TYPE>::Purge()
{
	REFLEX_ASSERT(threadvalidator);

	REFLEX_RLOOP(idx, m_cache.GetSize() - 1)
	{
		TYPE & object = *m_cache[idx];

		if (REFLEX_ATOMIC_READ(object.GetActualRetainCount()) == 1) //if only retained by self, uses std::memory_order_acquire
		{
			m_cache.Remove(idx);
		}
	}
}

template <class TYPE> inline void Netx::Core::ReferenceMt<TYPE>::Enumerate(const Function <void(TYPE&)> & callback)
{
	REFLEX_ASSERT(threadvalidator);

	for (auto & i : m_cache) callback(*i);
}

template <class TYPE> inline void Netx::Core::ReferenceMt<TYPE>::Destroy()
{
	REFLEX_ASSERT(threadvalidator);

	m_cache.Clear();

	REFLEX_ATOMIC_WRITE(m_current_mt, nullptr);
}

template <class TYPE> inline Reference <TYPE> Netx::Core::ReferenceMt<TYPE>::LoadMt() const
{
	auto ptr = REFLEX_ATOMIC_READ(m_current_mt);	//uses std::memory_order_acquire

	return Cast<TYPE>(ptr);
}
