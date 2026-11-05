#pragma once

#include "../[require].h"




//
//Primary API

namespace Netx
{

	struct Pins;


	Pins CountPins(const Module & module);

	void AllocatePins(Access access, Module & module, Pins io);

}




//
//Pins

struct Netx::Pins
{
	bool operator<(const Pins & io) const { return Reinterpret<UInt32>(*this) < Reinterpret<UInt32>(io); }
	bool operator==(const Pins & io) const { return Reinterpret<UInt32>(*this) == Reinterpret<UInt32>(io); }
	bool operator!=(const Pins & io) const { return Reinterpret<UInt32>(*this) != Reinterpret<UInt32>(io); }

	UInt8 event_ins = 0;
	UInt8 event_outs = 0;
	UInt8 audio_ins = 0;
	UInt8 audio_outs = 0;
};
