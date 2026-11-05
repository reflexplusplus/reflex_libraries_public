#pragma once

#include "../defines.h"




//
//Primary API

namespace Netx
{

	constexpr Value32 MakeValue32(bool value);

	constexpr Value32 MakeValue32(UInt8 value);

	constexpr Value32 MakeValue32(UInt32 value);

	constexpr Value32 MakeValue32(Int32 value);

	constexpr Value32 MakeValue32(Float32 value);

}




//
//impl

template <> inline bool Reflex::SetFiltered<Netx::Value32>(Netx::Value32 & value, const Netx::Value32 & set)
{
	return Reflex::SetFiltered(Reinterpret<Int32>(value), Reinterpret<Int32>(set));
}

inline constexpr Netx::Value32 Netx::MakeValue32(bool value)
{
	return { .uvalue = value };
}

inline constexpr Netx::Value32 Netx::MakeValue32(UInt8 value)
{
	return { .uvalue = value };
}

inline constexpr Netx::Value32 Netx::MakeValue32(UInt32 value)
{
	return { .uvalue = value };
}

inline constexpr Netx::Value32 Netx::MakeValue32(Int32 value)
{
	return { .ivalue = value };
}

inline constexpr Netx::Value32 Netx::MakeValue32(Float32 value)
{
	return { .fvalue = value };
}
