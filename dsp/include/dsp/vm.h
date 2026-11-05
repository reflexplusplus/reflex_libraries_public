#pragma once

#include "ui.h"
#include "reflex/vm.h"

#if __has_include("reflex/vm/[require].h")



REFLEX_NS(DSP)

extern const VM::Module g_sample, g_peaktable;

REFLEX_END

REFLEX_SET_TRAIT(DSP::PeakTable, IsThreadSafe);
REFLEX_SET_TRAIT(DSP::PeakTable, IsNonCircular);
REFLEX_SET_TRAIT(DSP::PeakTable::Writer, IsThreadSafe);
REFLEX_SET_TRAIT(DSP::PeakTable::Writer, IsNonCircular);

#endif
