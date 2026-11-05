#pragma once

#include "../component.h"




//
//Primary API

namespace Netx
{

	UInt64 MakeComponentAddress(const Component & component);


	Component * QueryComponent(Module & module, ClassID classid, Key32 uid, Component * fallback = nullptr);


	Array < Reference <Component> > GetComponents(Module & module);


	void EnumerateComponents(Module & module, ClassID classid, const Function <void(Component&)> & callback);

	void EnumerateComponents(const Module & module, ClassID classid, const Function <void(const Component&)> & callback);


	template <class TYPE, class ... VARGS> AlreadyRetained <TYPE> AddComponent(Access, Module & module, VARGS && ... vargs);

	template <class TYPE> TYPE * QueryComponent(Module & module, Key32 id = kNullKey, TYPE * fallback = nullptr);

	template <class TYPE> const TYPE * QueryComponent(const Module & module, Key32 id = kNullKey, const TYPE * fallback = nullptr);

	template <class TYPE> ConstAlreadyRetained <TYPE> GetComponent(const Module & module, Key32 id = kNullKey);


	template <class TYPE> void EnumerateComponents(Module & module, const Function <void(TYPE&)> & callback);

	template <class TYPE> void EnumerateComponents(const Module & module, const Function <void(const TYPE&)> & callback);

}




//
//impl

REFLEX_NS(Netx)
class Group;
REFLEX_END

inline UInt64 Netx::MakeComponentAddress(const Component & component)
{
	return Detail::MakeComponentAddress(component.classid, component.uid);
}

template <class TYPE, class ... VARGS> AlreadyRetained <TYPE> Netx::AddComponent(Access access, Module & module, VARGS && ... vargs)
{
	auto component = New<TYPE>(std::forward<VARGS>(vargs)...);

	component->Attach(access, module);

	return component;
}

template <class TYPE> REFLEX_INLINE ConstAlreadyRetained <TYPE> Netx::GetComponent(const Module & module, Key32 id)
{
	REFLEX_STATIC_ASSERT(InheritsFrom<TYPE,Component>::value);

	return Cast<TYPE>(QueryComponent(RemoveConst(module), TYPE::kClassID, id, &REFLEX_NULL(TYPE)));
}

template <class TYPE> REFLEX_INLINE TYPE * Netx::QueryComponent(Module & module, Key32 id, TYPE * fallback)
{
	REFLEX_STATIC_ASSERT(InheritsFrom<TYPE,Component>::value);

	return Cast<TYPE>(QueryComponent(module, TYPE::kClassID, id, fallback));
}

template <class TYPE> REFLEX_INLINE const TYPE * Netx::QueryComponent(const Module & module, Key32 id, const TYPE * fallback)
{
	REFLEX_STATIC_ASSERT(InheritsFrom<TYPE,Component>::value);

	return Cast<TYPE>(QueryComponent(RemoveConst(module), TYPE::kClassID, id, RemoveConst(fallback)));
}

inline void Netx::EnumerateComponents(const Module & module, ClassID classid, const Function <void(const Component&)> & callback)
{
	EnumerateComponents(RemoveConst(module), classid, Reinterpret<Function<void(Component&)>>(callback));
}

template <class TYPE> inline void Netx::EnumerateComponents(Module & module, const Function <void(TYPE&)> & callback)
{
	EnumerateComponents(module, TYPE::kClassID, Reinterpret<Function<void(Component&)>>(callback));
}

template <class TYPE> inline void Netx::EnumerateComponents(const Module & module, const Function <void(const TYPE&)> & callback)
{
	EnumerateComponents(module, TYPE::kClassID, Reinterpret<Function<void(const Component&)>>(callback));
}
