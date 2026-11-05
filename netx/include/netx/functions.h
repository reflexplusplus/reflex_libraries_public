#pragma once

#include "functions/value.h"
#include "functions/component.h"
#include "functions/event.h"
#include "functions/misc.h"
#include "functions/rt.h"




//PRESET STUFF

namespace Netx
{

	Pair <WString::View,bool> GetPresetInfo(const Module & module);

	WString::View GetFilename(const Module & module);

}

inline WString::View Netx::GetFilename(const Module & module)
{
	return GetPresetInfo(module).a;
}
