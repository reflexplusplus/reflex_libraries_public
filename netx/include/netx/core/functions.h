#pragma once

#include "network.h"




//
//Secondary API

namespace Netx::Core
{

	inline Array < Reference <Module> > CollectModules(Network & network)
	{
		Array < Reference <Module> > rtn;

		rtn.Allocate(network.GetNumItem());

		for (auto & i : network) rtn.Push<kAllocateNone>(i);

		return rtn;
	}

}
