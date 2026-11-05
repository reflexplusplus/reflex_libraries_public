#pragma once

#include "../../netx.h"
#include "../../../../dsp/include/dsp/core.h"
#include "reflex_ext/async.h"

#ifdef NETX_DISABLE_DEPRECATIONS	 //NSA transition
#define NETX_DEPRECATED
#else
#define NETX_DEPRECATED [[deprecated]]
#endif
