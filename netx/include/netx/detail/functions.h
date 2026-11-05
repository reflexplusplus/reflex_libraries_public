#pragma once

#include "../network.h"




REFLEX_NS(Netx::Detail)

inline UInt32 MakeHeader(UInt16 ncomponent)
{
	UInt8 platform = UInt8(System::kPlatform & 7);

	return Reinterpret<UInt32>(MakeTuple(BitSet<UInt16>(3 | (platform << 4), 7, sizeof(void*) == 8), ncomponent));
}

constexpr UInt64 MakeComponentAddress(ClassID classid, Key32 uid)
{
	return (static_cast<UInt64>(classid) << 32) | uid.value;
}

template <class CLIENT> inline void ScheduleCallback(Network & network, CLIENT * self, FunctionPointer <bool(CLIENT&)> callback)
{
	network.ScheduleCallback(self, Reflex::Detail::CastFunctionPointer<FunctionPointer<bool(void*)>>(callback));
}

REFLEX_END
