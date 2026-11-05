#pragma once

#include "../module.h"




//
//Primary API

REFLEX_NS(Netx)

inline AlreadyRetained <Module> GetModule(Pin & pin)
{
	return Cast<Module>(pin.module);
}

inline ConstAlreadyRetained <Module> GetModule(const Pin & pin)
{
	return Cast<Module>(pin.module);
}


AlreadyRetained <Pin> AddPin(Access, Module & module, Scope scope, ClassID classid, Pin::Mode mode = Pin::kModeAuto);

template <class STREAM> AlreadyRetained <Pin> AddPin(Access, Module & module, Scope scope, Pin::Mode mode = Pin::kModeAuto);

void AllocatePins(Access, Module & module, Scope scope, const ArrayView <ClassID> & types, Pin::Mode mode = Pin::kModeAuto);

void AllocatePins(Access, Module & module, Scope scope, ClassID classid, UInt n, Pin::Mode mode = Pin::kModeAuto);

void DiscardPins(Access, Module & module, Scope scope);

void AutoConnect(Access access, Module & src, Module & dst, ClassID pintype);


inline Ref <Pin::List> GetPins(Core::Module & module, Scope scope)
{
	return scope ? module.outputs : module.inputs;
}

inline ConstRef <Pin::List> GetPins(const Core::Module & module, Scope scope)
{
	return GetPins(RemoveConst(module), scope);
}


AlreadyRetained <Pin> GetPin(Core::Module & module, Scope scope, UInt idx);

ConstAlreadyRetained <Pin> GetPin(const Core::Module & module, Scope scope, UInt idx);


Pin * QueryPin(Core::Module & module, Scope scope, ClassID classid);

const Pin * QueryPin(const Core::Module & module, Scope scope, ClassID classid);


Scope GetScope(const Pin & pin);

bool IsInput(const Pin & pin);

bool IsOutput(const Pin & pin);



//standard network properties

Float32 GetSampleRate(const Data::PropertySet & variables);

UInt32 GetMaxBufferSize(const Data::PropertySet & variables);

UInt32 GetPPQ(const Data::PropertySet & variables);



//streams

template <class TYPE> ConstAlreadyRetained <TYPE> GetNullStream(Network & network);


template <class TYPE> const TYPE * QueryInputStream(const Component::StreamsView & streams);

template <class TYPE> const TYPE * QueryInputStream(const Component::StreamsView & streams, UInt idx);

template <class TYPE> TYPE * QueryOutputStream(const Component::StreamsView & streams);

template <class TYPE> TYPE * QueryOutputStream(const Component::StreamsView & streams, UInt idx);



//cmd

WString GetWStringArg(const Data::PropertySet & args, Key32 id);

WString GetPathArg(const Data::PropertySet & args, Key32 id = K32("path"));

Float GetFloatArg(const Data::PropertySet & args, Key32 id, Float falback = 0.0f);	//Float32 or Int32

UInt GetUIntArg(const Data::PropertySet & args, Key32 id, UInt fallback = 0);	//Int32 or UInt32



//debug

CString::View GetModuleNameDebug(const Module & module);

REFLEX_END




//
//impl

REFLEX_NS(Netx::Detail)

template <class AUTO> auto TestVar(AUTO var)
{
	REFLEX_ASSERT(True(var));

	return var;
}

void EnumerateComponents(Network & network, ClassID classid, const Function <void(Component&)> & callback);

REFLEX_END

inline ConstAlreadyRetained <Netx::Pin> Netx::GetPin(const Core::Module & module, Scope scope, UInt idx)
{
	return GetPin(RemoveConst(module), scope, idx);
}

inline const Netx::Pin * Netx::QueryPin(const Core::Module & module, Scope scope, ClassID classid)
{
	return QueryPin(RemoveConst(module), scope, classid);
}

inline Float32 Netx::GetSampleRate(const Data::PropertySet & variables)
{
	return Detail::TestVar(Data::GetFloat32(variables, kSampleRate));
}

inline UInt32 Netx::GetMaxBufferSize(const Data::PropertySet & variables)
{
	return Detail::TestVar(Data::GetUInt32(variables, kBufferSize));
}

inline UInt32 Netx::GetPPQ(const Data::PropertySet & variables)
{
	return Detail::TestVar(Data::GetUInt32(variables, kPPQ));
}

template <class STREAM> AlreadyRetained <Netx::Pin> Netx::AddPin(Access access, Module & module, Scope scope, Pin::Mode mode)
{
	return AddPin(access, module, scope, STREAM::kClassID, mode);
}

inline Netx::Scope Netx::GetScope(const Pin & pin)
{
	return Scope(pin.GetList() == &pin.module->outputs);
}

inline bool Netx::IsInput(const Pin & pin)
{
	return pin.GetList() == &pin.module->inputs;
}

inline bool Netx::IsOutput(const Pin & pin)
{
	return pin.GetList() == &pin.module->outputs;
}

template <class TYPE> inline ConstAlreadyRetained <TYPE> Netx::GetNullStream(Network & network)
{
	return Cast<TYPE>(network.QueryNullStream(TYPE::kClassID));
}

template <class TYPE> inline const TYPE * Netx::QueryInputStream(const Component::StreamsView & streams)
{
	for (auto stream : streams.inputs)
	{
		if (stream->classid == TYPE::kClassID)
		{
			return Cast<TYPE>(stream).Adr();
		}
	}

	return nullptr;
}

template <class TYPE> inline const TYPE * Netx::QueryInputStream(const Component::StreamsView & streams, UInt idx)
{
	if (idx < streams.inputs.size)
	{
		auto stream = streams.inputs[idx];

		if (stream->classid == TYPE::kClassID)
		{
			return Cast<TYPE>(stream).Adr();
		}
	}

	return nullptr;
}

template <class TYPE> inline TYPE * Netx::QueryOutputStream(const Component::StreamsView & streams)
{
	for (auto stream : streams.outputs)
	{
		if (stream->classid == TYPE::kClassID)
		{
			return Cast<TYPE>(stream).Adr();
		}
	}

	return nullptr;
}

template <class TYPE> inline TYPE * Netx::QueryOutputStream(const Component::StreamsView & streams, UInt idx)
{
	if (idx < streams.outputs.size)
	{
		auto stream = streams.outputs[idx];

		if (stream->classid == TYPE::kClassID)
		{
			return Cast<TYPE>(stream).Adr();
		}
	}

	return nullptr;
}
