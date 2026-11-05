#pragma once

#include "../[require].h"




//
//declarations

REFLEX_NS(DSP::Detail)

void GenerateWaveform(const Sample & sample, UInt channel, Float vo, Float vr, const System::Colour & fg, const System::Colour & bg, System::RawBitmap & bitmap);

REFLEX_END
