#pragma once

#include "external_component_handle.h"




//
//Primary API

namespace Netx
{

	template <class TYPE> class ExternalComponentRef;

	template <class TYPE, class ... ARGS> ExternalComponentRef <TYPE> AcquireExternalComponent(Module & module, ARGS && ... vargs);
	template <class TYPE, class ... ARGS> ExternalComponentRef <TYPE> MakeExternalComponentRef(Module & module, ARGS && ... vargs);

}




//
//ExternalComponentRef

template <class TYPE>
class Netx::ExternalComponentRef
{
public:

	ExternalComponentRef() = default;

	template <class ... ARGS> ExternalComponentRef(Module & module, ARGS && ... args);

	ExternalComponentRef(const ExternalComponentRef &) = default;


	void Clear();

	template <class ... ARGS> void Set(Module & module, ARGS && ... args);

	ExternalComponentRef & operator=(const ExternalComponentRef &) = default;


	operator TYPE &() const;

	TYPE & operator*() const;

	TYPE * operator->() const;



private:

	template <class ... ARGS> static ExternalComponentHandle::Args MakeArgs(ARGS && ... args);


	Reference <Object> m_handle;

	AlreadyRetained <TYPE> m_component;
};




//
//impl

REFLEX_NS(Reflex)

template <class TYPE> TYPE & Deref(Netx::ExternalComponentRef <TYPE> & ref) { return *ref; }

template <class TYPE> const TYPE & Deref(const Netx::ExternalComponentRef <TYPE> & ref) { return *ref; }

REFLEX_END

template <class TYPE> template <class ... ARGS> inline Netx::ExternalComponentHandle::Args Netx::ExternalComponentRef<TYPE>::MakeArgs(ARGS && ... args)
{
	return { TYPE::kClassID, Null<TYPE>(), &TYPE::CreateExternal, TYPE::MakeExternalID(std::forward<ARGS>(args)...)};
}

template <class TYPE> template <class ... ARGS> inline Netx::ExternalComponentRef <TYPE>::ExternalComponentRef(Module & module, ARGS && ... args)
	: m_handle(ExternalComponentHandle::Acquire(module, MakeArgs(std::forward<ARGS>(args)...)))
	, m_component(Cast<TYPE>(Cast<ExternalComponentHandle>(m_handle)->component))
{
}

template <class TYPE> template <class ... ARGS> inline void Netx::ExternalComponentRef <TYPE>::Set(Module & module, ARGS && ... args)
{
	auto handle = ExternalComponentHandle::Acquire(module, MakeArgs(std::forward<ARGS>(args)...));
		
	m_handle = handle;

	m_component = Cast<TYPE>(handle->component);
}

template <class TYPE> inline void Netx::ExternalComponentRef<TYPE>::Clear()
{
	m_handle.Clear();

	m_component = {};
}

template <class TYPE> inline Netx::ExternalComponentRef <TYPE>::operator TYPE &() const
{
	return *m_component;
}

template <class TYPE> inline TYPE & Netx::ExternalComponentRef <TYPE>::operator*() const
{
	return *m_component;
}

template <class TYPE> inline TYPE * Netx::ExternalComponentRef <TYPE>::operator->() const
{
	return m_component.Adr();
}

template <class TYPE, class ... ARGS> inline Netx::ExternalComponentRef <TYPE> Netx::MakeExternalComponentRef(Module & module, ARGS && ... vargs)
{
	return ExternalComponentRef<TYPE>(module, std::forward<ARGS>(vargs)...);
}

template <class TYPE, class ... ARGS> inline Netx::ExternalComponentRef <TYPE> Netx::AcquireExternalComponent(Module & module, ARGS && ... vargs)
{
	return MakeExternalComponentRef<TYPE>(module, std::forward<ARGS>(vargs)...);
}
