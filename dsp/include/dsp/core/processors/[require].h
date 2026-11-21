#pragma once

#include "../stereo.h"
#include "../detail/constants.h"
#include "../functions/logic.h"

#define DSP_PROCESSOR(SAMPLE_T) class Parameters; typedef SAMPLE_T SampleType; typedef DSP::Detail::Constants <SAMPLE_T> Constants;

#define DSP_SETVOICE(i, NAME, vidx, value) { auto t = (i).Get##NAME(); t[vidx] = value; (i).Set##NAME(t); }