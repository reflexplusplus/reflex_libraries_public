#pragma once

#include "../[require].h"




//
//Secondary API

namespace Netx
{

	template <class CLIENT> struct NetworkClock;

}




//
//NetworkClock

template <class CLIENT>
struct Netx::NetworkClock
{
	NetworkClock(Network & network, CLIENT * self, FunctionPointer <bool(CLIENT&)> callback);

	~NetworkClock();


	Network & network;

	CLIENT & self;
};




//
//impl

template <class CLIENT> inline Netx::NetworkClock<CLIENT>::NetworkClock(Network & network, CLIENT * self, FunctionPointer <bool(CLIENT&)> callback)
	: network(network)
	, self(*self)
{
	Detail::ScheduleCallback(network, self, callback);
}

template <class CLIENT> inline Netx::NetworkClock<CLIENT>::~NetworkClock()
{
	network.CancelCallback(&self);
}
