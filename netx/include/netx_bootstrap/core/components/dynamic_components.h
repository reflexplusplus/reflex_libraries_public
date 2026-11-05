#pragma once

#include "../[require].h"




//
//Primary API

namespace Netx
{

	class DynamicComponents;		//client configurable dynamic components

}




//
//DynamicComponents

class Netx::DynamicComponents : public Component
{
public:

	NETX_COMPONENT(Netx, DynamicComponents, K32("DynamicComponents"), Key32 id = kNullKey);

	static DynamicComponents & null;

	class RestoreScope;	//current-thread restore permission, ANDed with enclosing scopes

	using ComponentFactory = Function <Component *(ClassID classid, Key32 id, Data::Archive::View)>;

	static void PublishComponentFactory(Module & module, ComponentFactory factory);



	//components

	template <class TYPE> AlreadyRetained <TYPE> EnableComponent(Key32 id, Data::Archive::View params);

	template <class TYPE> bool DisableComponent(Key32 id);

	template <class TYPE> TYPE * QueryComponent(Key32 id);

	template <class TYPE> const TYPE * QueryComponent(Key32 id) const;


	Component * EnableComponent(ClassID classid, Key32 id, Data::Archive::View param, Component * fallback = nullptr);

	bool DisableComponent(ClassID classid, Key32 id);

	Component * QueryComponent(ClassID classid, Key32 id, Component * fallback = nullptr);

	const Component * QueryComponent(ClassID classid, Key32 id, Component * fallback = nullptr) const;

	void Enumerate(const Function <void(Component & component)> & callback) const;



protected:

	void OnAttachModule() override;

	void OnDetachModule() override;

	void OnRestore(Data::Archive::View & stream, Key32 context) override;

	void OnStore(Data::Archive & stream) const override;

	void OnStoreAction(Key32 action, Data::Archive::View param, Data::Archive & stream, bool redo) const override;

	void OnRestoreAction(Key32 action, Data::Archive::View param, Data::Archive::View & stream, bool redo) override;



private:

	using Entry = Tuple <UInt64, Data::Archive, Reference<Component>>;

	using StoredEntry = Tuple <UInt64, Data::Archive, Data::Archive>;

	
	void Reconcile(ArrayView <StoredEntry> entries, Key32 context);

	void StoreEntry(Data::Archive & stream, const Entry & entry) const;

	Component * RestoreEntry(const StoredEntry & entry, Key32 context);

	void RemoveComponent(UInt64 address);

	Component * CreateComponent(UInt64 address, Data::Archive::View params);


	ComponentFactory m_factory;

	Array <Entry> m_components;

	bool m_restore_enabled;

};




//
//DynamicComponents::RestoreScope

class Netx::DynamicComponents::RestoreScope
{
public:

	REFLEX_NONCOPYABLE(RestoreScope);

	RestoreScope(DynamicComponents & component, bool enabled);

	~RestoreScope();



private:

	bool & enabled;

	const bool m_previous;

};




//
//impl

inline Netx::DynamicComponents::RestoreScope::RestoreScope(DynamicComponents & component, bool enabled)
	: enabled(component.m_restore_enabled)
	, m_previous(component.m_restore_enabled)
{
	component.m_restore_enabled = m_previous && enabled;
}

inline Netx::DynamicComponents::RestoreScope::~RestoreScope()
{
	enabled = m_previous;
}

inline const Netx::Component * Netx::DynamicComponents::QueryComponent(ClassID classid, Key32 id, Component * fallback) const
{
	return RemoveConst(this)->QueryComponent(classid, id, fallback);
}

template <class TYPE> inline AlreadyRetained <TYPE> Netx::DynamicComponents::EnableComponent(Key32 id, Data::Archive::View params)
{
	REFLEX_STATIC_ASSERT(InheritsFrom<TYPE,Component>::value);

	if (auto p = EnableComponent(TYPE::kClassID, id, params))
	{
		return Cast<TYPE>(p);
	}

	return Null<TYPE>();
}

template <class TYPE> inline bool Netx::DynamicComponents::DisableComponent(Key32 id)
{
	REFLEX_STATIC_ASSERT(InheritsFrom<TYPE,Component>::value);

	return DisableComponent(TYPE::kClassID, id);
}

template <class TYPE> inline TYPE * Netx::DynamicComponents::QueryComponent(Key32 id)
{
	REFLEX_STATIC_ASSERT(InheritsFrom<TYPE,Component>::value);

	return Cast<TYPE>(QueryComponent(TYPE::kClassID, id));
}

template <class TYPE> inline const TYPE * Netx::DynamicComponents::QueryComponent(Key32 id) const
{
	REFLEX_STATIC_ASSERT(InheritsFrom<TYPE,Component>::value);

	return Cast<TYPE>(QueryComponent(TYPE::kClassID, id));
}
