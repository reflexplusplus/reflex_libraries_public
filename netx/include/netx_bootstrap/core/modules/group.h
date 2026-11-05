#pragma once

#include "../components/detail/group.h"
#include "../components/dynamic_pins.h"




//
//Primary API

namespace Netx
{

	class Group;

}




//
//Group

class Netx::Group : public Module
{
public:

	REFLEX_OBJECT(Netx::Group, Module);

	template <bool CONST> using ItrImpl = Detail::Group::ItrImpl<CONST>;

	using Itr = ItrImpl <false>;

	using ConstItr = ItrImpl <true>;



	//lifetime

	Group(ClassID class_id, ConstWillRetain <Factory> factory);

	~Group();



	//setup

	void RegisterClassArgs(ClassID class_id, ConstWillRetain <Data::PropertySet> args) { SetProperty(class_id, args.RemoveConst()); }



	//content

	Reference <Object> Clear(bool recycle);

	AlreadyRetained <Module> AddModule(ClassID classid);

	UInt32 AddModule(WillRetain <Module> module);

	void RemoveModule(UInt32 uid);


	bool Empty() const;

	void Enumerate(const Function <void(Module&)> & callback) const;



	//module actions ('session' is managed by parent container)

	void ResetModule(UInt32 module_id);

	void AddPin(UInt32 module_id, Scope scope, ClassID classid);

	void RemovePin(Pin & pin);

	void ReorderPin(UInt32 module_id, Scope scope, UInt from, UInt to);



	//connections

	bool CanConnect(const Pin & from, const Pin & to) const;

	bool Connect(Pin & from, Pin & to);

	void Disconnect(Pin & pin);



	//iterate

	Itr begin();

	Itr end();

	ConstItr begin() const;

	ConstItr end() const;



	//legacy

	NETX_DEPRECATED void RemoveModule(Module & module) { RemoveModule(module.GetID()); }

	NETX_DEPRECATED void AddDestination(Pin & from, Pin & to) { Connect(from, to); }

	NETX_DEPRECATED void RemoveSource(Pin & pin) { Disconnect(pin); }

	NETX_DEPRECATED void RemoveDestinations(Pin & pin) { Disconnect(pin); }

	NETX_DEPRECATED inline void AddPin(Netx::Scope scope, ClassID classid)
	{
		Netx::EnumerateComponents<Netx::DynamicPins>(*this, [scope, classid](Netx::DynamicPins & pins)
		{
			if (pins.scope == scope)
			{
				pins.AddPin(classid);
			}
		});
	}

	NETX_DEPRECATED inline void RemovePin(Netx::Scope scope, UInt idx)
	{
		Netx::EnumerateComponents<Netx::DynamicPins>(*this, [scope, idx](Netx::DynamicPins & pins)
		{
			if (pins.scope == scope)
			{
				pins.RemovePin(idx);
			}
		});
	}



	//links

	const AlreadyRetained <Detail::Group> component;



protected:

	Group(ClassID class_id) : Group(class_id, Null<Factory>()) {}	//for null

};




//
//Detail

REFLEX_NS(Netx::Detail)

using ExportModuleFn = Function <bool(const Netx::Module & module, Data::KeyMap & keymap, Data::PropertySet & out)>;
using ImportModuleFn = Function <bool(Netx::Module & module, const Data::PropertySet & in)>;

bool ExportModule(const Data::PropertySet & type_handlers, const Netx::Module & module, Data::KeyMap & keymap, Data::PropertySet & out);
bool ImportModule(const Data::PropertySet & type_handlers, Netx::Module & module, const Data::PropertySet & in);

REFLEX_END




//
//impl

inline Reference<Object> Netx::Group::Clear(bool recycle)
{
	return component->Clear(recycle);
}

inline AlreadyRetained<Netx::Module> Netx::Group::AddModule(ClassID classid)
{
	return component->AddModule(classid);
}

inline UInt32 Netx::Group::AddModule(WillRetain<Module> module)
{
	return component->AddModule(module);
}

inline void Netx::Group::RemoveModule(UInt32 uid)
{
	component->RemoveModule(uid);
}

inline bool Netx::Group::Empty() const
{
	return component->Empty();
}

inline void Netx::Group::Enumerate(const Function<void(Module&)> & callback) const
{
	component->Enumerate(callback);
}

inline void Netx::Group::ResetModule(UInt32 module_id)
{
	component->ResetModule(module_id);
}

inline void Netx::Group::AddPin(UInt32 module_id, Scope scope, ClassID classid)
{
	component->AddPin(module_id, scope, classid);
}

inline void Netx::Group::RemovePin(Pin & pin)
{
	component->RemovePin(pin);
}

inline void Netx::Group::ReorderPin(UInt32 module_id, Scope scope, UInt from, UInt to)
{
	component->ReorderPin(module_id, scope, from, to);
}

inline bool Netx::Group::CanConnect(const Pin & from, const Pin & to) const
{
	return component->CanConnect(from, to);
}

inline bool Netx::Group::Connect(Pin & from, Pin & to)
{
	return component->Connect(from, to);
}

inline void Netx::Group::Disconnect(Pin & pin)
{
	component->Disconnect(pin);
}

inline Netx::Group::Itr Netx::Group::begin()
{
	return component->begin();
}

inline Netx::Group::Itr Netx::Group::end()
{
	return component->end();
}

inline Netx::Group::ConstItr Netx::Group::begin() const
{
	return static_cast<const Detail::Group&>(*component).begin();
}

inline Netx::Group::ConstItr Netx::Group::end() const
{
	return static_cast<const Detail::Group&>(*component).end();
}
