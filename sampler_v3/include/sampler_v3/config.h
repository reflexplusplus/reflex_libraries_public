#pragma once

#include "instrument.h"




//
//decl

struct SamplerV3::Config
{
	enum PersistenceMode : UInt8
	{
		kPersistenceModeNone,
		kPersistenceModeGroups,
		kPersistenceModeZones,
		kNumPersistenceModes
	};

	WString::View groupname = L"Group";

	UInt16 max_polyphony = 256;

	UInt16 default_polyphony = 32;

	bool ahdsr = true;

	bool xypad = true;

	UInt8 nmodenv = 3;

	UInt8 nlfo = 3;

	UInt8 ntable = 3;

	PersistenceMode persistence = kPersistenceModeZones;	//zone persistence includes ownership of zone sample resource collection and relocation

	UInt8 parameterset_update_flags = 0;

	Array <CString::View> polymods;

	Array < Pair <Key32, ConstReference <Netx::PropertyDef> > > custom_parameters;

	FunctionPointer <Unretained <Object>(Processor & state, Object & client)> compile_map = nullptr;	//!required...  processor is compiled, cant modify groups/zones here

	FunctionPointer <void(Netx::VoiceFx::Map&)> register_voicefx = [](Netx::VoiceFx::Map&) {};

	FunctionPointer <void(Instrument & instrument, Key32 context)> import = [](Instrument & instrument, Key32 context) {};
};

class SamplerV3::Class : public Netx::Registry::Class
{
public:

	Class(Netx::ClassID classid, const Info & info, FunctionPointer <void(Config&)> init, Ctr ctr);

	const FunctionPointer <void(Config&)> init;

	const UIntNative reserved;
};
