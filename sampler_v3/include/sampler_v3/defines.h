#pragma once

#include "[require].h"




//
//decl

REFLEX_NS(SamplerV3)

using State = Reflex::ChangeCount<false, true>;

extern const UInt32 kPitchParamID, kEngineXParamIDs[3], kMixParamIDs[4];

extern const UInt32 kAmpEnvParamIDs[6], kXyParamIDs[3], kModEnvParamIDs[3][6], kLfoParamIDs[3][4];

constexpr UInt32 kSrcPitchbend = K32("PB");
constexpr UInt32 kSrcModWheel = K32("CC1");
constexpr UInt32 kSrcChannelAT = K32("ChannelAT");
constexpr UInt32 kSrcVelocity = K32("Velocity");

extern const UInt32 kSrcRandom[2], kSrcXYZ[3], kSrcModEnv[3], kSrcLfo[3], kSrcTable[3];

extern const CString::View kFxParamIDs[8];

extern const UInt32 kDstStart, kDstLoop, kDstPitch, kDstAmp, kDstPan, kDstAmpEnvAtk, kDstAmpEnvDcy, kDstModEnvAtk[3], kDstModEnvDcy[3], kDstLfoAmp[3];

extern const Pair <Key32,WString::View> kGroupPages[12];

extern const Pair <Key32> kRemappedParameters[15];

extern const Pair <Key32> kRemappedModulationDests[10];

REFLEX_END
