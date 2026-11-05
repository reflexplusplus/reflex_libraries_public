#pragma once

#include "../[require].h"




//
//Primary API

namespace Netx
{

	constexpr Key32 EncodePinAddress(Scope scope, UInt16 idx);

	constexpr Pair<Scope, UInt16> DecodePinAddress(Key32 address);

}




//
//impl

inline constexpr Key32 Netx::EncodePinAddress(Scope scope, UInt16 idx)
{
	return Key32(UInt32(UInt16(scope)) | (UInt32(idx) << 16));
}

inline constexpr Pair<Netx::Scope, UInt16> Netx::DecodePinAddress(Key32 address)
{
	const auto value = address.value;

	return { Scope(UInt16(value)), UInt16(value >> 16) };
}
