#pragma once

#include "../component.h"




//
//Secondary API

namespace Netx
{

	void ClearOutputsRt(UInt samples, Module::ProcessInfo & processinfo);	//default process fn for silence

	void BypassRt(UInt samples, Module::ProcessInfo & processinfo);			//default process fn for bypassing ins to outs

}