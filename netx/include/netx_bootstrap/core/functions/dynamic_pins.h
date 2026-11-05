#pragma once

#include "../components/dynamic_pins.h"




//
//Primary API

namespace Netx
{

	bool SetDynamicPins(DynamicPins & pins, ClassID classid, UInt count);

	bool SetDynamicPins(Module & module, Scope scope, ClassID classid, UInt count);

}




//
//impl

inline bool Netx::SetDynamicPins(Module & module, Scope scope, ClassID classid, UInt count)
{
	if (auto pins = DynamicPins::Query(module, scope))
	{
		return SetDynamicPins(*pins, classid, count);
	}

	return false;
}
