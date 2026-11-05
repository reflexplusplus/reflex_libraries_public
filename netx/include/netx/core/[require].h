#pragma once

#include "reflex_ext/async.h"
#include "reflex_ext/file.h"
#include "reflex_ext/bootstrap/audioplugin/parameter.h"	//for Value32

REFLEX_USE(Reflex);




//
//module

namespace Netx
{

	extern const Reflex::Detail::Module & module;	// <= File::module

}