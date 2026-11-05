#pragma once

#include "../defines.h"
#include "../detail/constants.h"




//
//Public API

REFLEX_NS(DSP)


//parabolic

Float Parabolic(Float x);

FloatV4 Parabolic(const FloatV4 & x);



//gain

template <Precision PRECISION = kPrecisionHigh> Float NormalisedVolumeToAmp(Float normal);

template <Precision PRECISION = kPrecisionHigh> FloatV4 NormalisedVolumeToAmp(const FloatV4 & normal);


template <Precision PRECISION = kPrecisionHigh> Float AmpToNormalisedVolume(Float amp);

template <Precision PRECISION = kPrecisionHigh> FloatV4 AmpToNormalisedVolume(const FloatV4 & amp);


//envelopes

Float EnvTime(Float normal);

FloatV4 EnvTime(const FloatV4 & normal);


Float EnvTimeToNormal(Float ms);

FloatV4 EnvTimeToNormal(const FloatV4 & ms);


template <class TYPE> TYPE CosineBell(const TYPE & x);

REFLEX_END




//
//implementation

REFLEX_INLINE Float DSP::Parabolic(Float x)
{
	return (2.0f - x) * x;
}

REFLEX_INLINE FloatV4 DSP::Parabolic(const FloatV4 & x)
{
	return (Detail::Constants<FloatV4>::f_two - x) * x;
}

REFLEX_INLINE Float DSP::EnvTime(Float value)
{
	return Quartic(value) * 10000.0f;
}

template <DSP::Precision PRECISION> REFLEX_INLINE Float DSP::NormalisedVolumeToAmp(Float normal)
{
	auto exp2 = Exp2<PRECISION>(normal * 3.5f);

	return (exp2 - 1.0f) * 0.19349106f;
}

template <DSP::Precision PRECISION> REFLEX_INLINE FloatV4 DSP::NormalisedVolumeToAmp(const FloatV4 & normal)
{
	auto exp2 = Exp2<PRECISION>(normal * 3.5f);

	return (exp2 - FloatV4(1.0f)) * FloatV4(0.19349106f);
}

template <DSP::Precision PRECISION> REFLEX_INLINE Float DSP::AmpToNormalisedVolume(Float amp)
{
	REFLEX_ASSERT(amp >= 0.0f);

	constexpr Float kInvK = 1.0f / 0.19349106f; // ~5.168
	constexpr Float kInvSpan = 1.0f / 3.5f;

	return DSP::Log2<PRECISION>((amp * kInvK) + 1.0f) * kInvSpan;
}

//template <DSP::Precision PRECISION> REFLEX_INLINE FloatV4 DSP::AmpToNormalisedVolume(const FloatV4 & amp)
//{
//}

REFLEX_INLINE FloatV4 DSP::EnvTime(const FloatV4 & value)
{
	FloatV4 a = (value * value);

	return ((a * a)) * Detail::Constants<FloatV4>::f_tenthousand;
}

REFLEX_INLINE Float DSP::EnvTimeToNormal(Float ms)
{
	if (ms > 0.0f)
	{
		return SquareRoot(SquareRoot(Min(ms, 10000.0f) / 10000.0f));
	}
	else
	{
		return 0.0f;
	}
}

template <class TYPE> REFLEX_INLINE TYPE DSP::CosineBell(const TYPE & x)
{
	//typedef FloatV4 TYPE;

	TYPE t00000018;
	TYPE t00000020;
	TYPE t00000024;
	TYPE t00000028;
	TYPE t0000002C;
	TYPE t00000030;
	TYPE t00000034;
	TYPE t00000038;
	TYPE t0000003C;
	TYPE t00000040;
	TYPE t00000044;

	t00000018 = (x * TYPE(6.28318f)) - TYPE(3.14159f);
	t00000018 *= t00000018;

	t00000020 = t00000018 * TYPE(-2.605e-007f);
	t00000024 = t00000020 + TYPE(2.47609e-005f);
	t00000028 = t00000018 * t00000024;
	t0000002C = t00000028 + TYPE(-0.00138884f);
	t00000030 = t00000018 * t0000002C;
	t00000034 = t00000030 + TYPE(0.0416666f);
	t00000038 = t00000018 * t00000034;
	t0000003C = t00000038 + TYPE(-0.499923f);
	t00000040 = t00000018 * t0000003C;
	t00000044 = t00000040 + TYPE(2.0f);
	
	return (t00000044 * TYPE(0.5f));
}
