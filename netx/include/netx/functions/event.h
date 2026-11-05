#pragma once

#include "../[require].h"




//
//midi conversion

REFLEX_NS(Netx)

UInt16 AcquirePacketID(bool liveplay);

bool WasLivePlayed(UInt16 packetid);

REFLEX_END




//
//impl

REFLEX_NS(Netx::Detail)

extern UInt32 g_packetid;

REFLEX_END

REFLEX_INLINE UInt16 Netx::AcquirePacketID(bool liveplay)
{
	Detail::g_packetid = (Detail::g_packetid + 1) & 32767;

	return UInt16(Detail::g_packetid | (liveplay ? 32768 : 0));
}

REFLEX_INLINE bool Netx::WasLivePlayed(UInt16 packetid)
{
	return BitCheck(packetid, 15);
}
