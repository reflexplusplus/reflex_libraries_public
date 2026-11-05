#pragma once

#ifndef DSP_INCLUDE_FFT
#define DSP_INCLUDE_FFT 1
#endif

#include "core/module.h"

#include "core/defines.h"
#include "core/stereo.h"

#include "core/functions.h"

#include "core/processors/crossfader.h"
#include "core/processors/delay.h"
#include "core/processors/smoother.h"
#include "core/processors/peakdetector.h"
#include "core/processors/envelopefollower.h"
#include "core/processors/envelopes.h"
#include "core/processors/sampleplayer.h"
#include "core/processors/noise.h"
#include "core/processors/osc.h"
#include "core/processors/lfo.h"
#include "core/processors/filters.h"
#include "core/processors/oversampler.h"
#include "core/processors/smoother_bank.h"
#include "core/processors/fx.h"
#include "core/processors/table.h"

#include "core/frequencyrange.h"
#include "core/processor.h"
#include "core/sample.h"

#if DSP_INCLUDE_FFT
#include "core/fft.h"
#endif

#include "core/detail.h"
