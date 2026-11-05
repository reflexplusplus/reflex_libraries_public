#pragma once

#include "../[require].h"




//
//decl

REFLEX_NS(SamplerV3::Detail)

WString hz2string(Float freq, bool verbose);

Float string2hz(const WString::View & string);


WString ms2string(Float ms);

Float string2ms(const WString::View & seconds);


Unretained <Netx::PropertyDef> CreatePitchParam(const WString::View & label = L"Pitch", Float range = 36.0f);


template <class COMPONENT> inline Key32 MakeComponentUID(UInt16 uid)
{
	if constexpr (IsType<COMPONENT, Netx::PropertySet>::value) return Reinterpret<UInt32>(MakeTuple(UInt16(0), uid));
	if constexpr (IsType<COMPONENT, Netx::ParameterSet>::value) return Reinterpret<UInt32>(MakeTuple(UInt16(1), uid));
	if constexpr (IsType<COMPONENT, Netx::VoiceFx>::value) return Reinterpret<UInt32>(MakeTuple(UInt16(2), uid));
	if constexpr (IsType<COMPONENT, Netx::Matrix>::value) return Reinterpret<UInt32>(MakeTuple(UInt16(3), uid));

	return kZeroKey;
}

REFLEX_END

