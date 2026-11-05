#pragma once

#include "ui.h"

#if __has_include("reflex/vm/[require].h")




#define NETXVM_BIND_CLASSID(TYPE, env, ns, name) env.RegisterConstant(env.bindings->key32_t, ns, name, Data::Pack(TYPE::kClassID))

REFLEX_NS(Netx)

template <class TYPE> VM::TypeRef RegisterComponent(VM::Bindings & bindings, Key32 ns, const CString::View & name);

extern const VM::Module g_main, g_events, g_context, g_transport, g_propertyset, g_group, g_cmajor, g_ui;

extern const VM::Module g_parameterset, g_dynamic_components, g_dynamic_pins, g_abstract_sequencer, g_virtual_keyboard;

REFLEX_END

REFLEX_NS(Netx::VMi)

struct EventHeader
{
	UInt8 size = 16;
	UInt8 type = kNumEventType;
	UInt8 value1 = 0;
	UInt8 value2 = 0;
	Int32 position = 0;
};

struct Event : EventHeader
{
	UInt32 payload[2] = {};
};

struct ClockEvent : EventHeader
{
	Float32 bps;
	UInt32 songpos;
};

struct GateEvent : EventHeader
{
	Int32 packetid;
	Float32 velocity;
};

struct ControllerEvent : EventHeader
{
	Float32 value;
	UInt32 reserved_tail;
};

struct ProgramChangeEvent : EventHeader
{
	UInt8 reserved[7];
};

static_assert(sizeof(EventHeader) == 8);
static_assert(sizeof(Event) == 16);
static_assert(sizeof(ClockEvent) == sizeof(Event));
static_assert(sizeof(GateEvent) == sizeof(Event));
static_assert(sizeof(ControllerEvent) == sizeof(Event));
static_assert(sizeof(ProgramChangeEvent) == sizeof(Event));

REFLEX_END




//
//

REFLEX_SET_TRAIT(Netx::PropertyDef, IsNonCircular);
REFLEX_SET_TRAIT(Netx::ParameterSet, IsNonCircular);
REFLEX_SET_TRAIT(Netx::Parameter, IsNonCircular);

template <class TYPE> inline VM::TypeRef Netx::RegisterComponent(VM::Bindings & bindings, Key32 ns, const CString::View & name)
{
	auto component_t = VM::RegisterObject<TYPE>(bindings, ns, name);

	if constexpr (Reflex::Detail::kIsNullable<TYPE> && (kIsSingleThreadExclusive<TYPE> || kIsThreadSafe<TYPE>))
	{
		//global null was used already
	}
	else
	{
		component_t->ctr = [](VM_CTR_PARAMS) -> Object&
		{
			auto null = New<TYPE>();

			RemoveConst(null->GetContextID()) = context.GetContextID();

			return null;
		};
	}

	REFLEX_ASSERT(!component_t->flags.Check(VM::Type::kFlagDefaultConstructable));

	VM::Detail::SetTypeFlag(component_t, VM::Type::kFlagExplicitNullable);

	return component_t;
}

#endif
