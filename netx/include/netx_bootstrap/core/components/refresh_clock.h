#pragma once

#include "../detail/clock.h"




//
//Primary API

namespace Netx
{

	class RefreshClock;

}




//
//RefreshClock

class Netx::RefreshClock :
	public Component,
	public Detail::RefreshClock
{
public:

	REFLEX_OBJECT(RefreshClock, Component);

	NETX_CLASSID(K32("Netx::RefreshClock"));

	static RefreshClock & null;

	using Item = Detail::RefreshClock::Item;


	RefreshClock();



protected:

	void OnAttachNetwork(Network & network) override { Detail::RefreshClock::AttachNetwork(network); }

	void OnDetachNetwork(Network &) override { Detail::RefreshClock::DetachNetwork(); }

};




//
//impl

inline Netx::RefreshClock::RefreshClock()
	: Component(kClassID, 0, {}, kFlagNetworkAttachment)
{
	SetUpdateFlags(GetAccess(), 0, 0);
}
