#pragma once

#include "../defines.h"
#include "../detail/constants.h"
#include "math.h"




//
//(@doc:group=Convert)

REFLEX_NS(DSP)

//single

constexpr Float ms2secs(Float seconds);


constexpr Float hz2ms(Float hz);

constexpr Float ms2hz(Float ms);


constexpr Float ms2samples(Float ms, Float sr);

constexpr Float samples2ms(Float n, Float sr);


template <Precision PRECISION = kPrecisionHigh, class FLOAT> FLOAT p2f(const FLOAT & p);

template <Precision PRECISION> Float p2f_LEGACY(Float p);

template <Precision PRECISION> Float p2f_LEGACY(Float p, Float base);

template <Precision PRECISION = kPrecisionHigh> Float p2ff(Float p);

template <Precision PRECISION = kPrecisionHigh> Float p2ff(Float p, Float base);


template <Precision PRECISION = kPrecisionHigh> Float f2p(Float f);

template <Precision PRECISION = kPrecisionHigh> Float f2p(Float f, Float base);


template <Precision PRECISION = kPrecisionHigh> Float db2a(Float db);

template <Precision PRECISION = kPrecisionHigh> Float a2db(Float a);


constexpr Float32 bpm2bps(Float32 bpm) { return bpm / 60.0f; }

constexpr Float32 bps2bpm(Float32 bpm) { return bpm * 60.0f; }


constexpr Float beats2hz(Float bps, Float beats);


constexpr Float64 beats2bps(Float64 sr, Float64 length, Float64 nbeats);

constexpr Float64 bps2beats(Float64 sr, Float64 length, Float64 bps);


template <Precision PRECISION> Float samples2decay(Float ms, Float floor = 0.01f);


	
//vector

FloatV4 hz2ms(const FloatV4 & hz);

FloatV4 ms2hz(const FloatV4 & ms);


FloatV4 ms2samples(const FloatV4 & ms, const FloatV4 & sr);

FloatV4 samples2ms(const FloatV4 & n, const FloatV4 & sr);


template <Precision PRECISION> FloatV4 p2f_LEGACY(const FloatV4 & p);

template <Precision PRECISION> FloatV4 p2f_LEGACY(const FloatV4 & p, const FloatV4 & base);

template <Precision PRECISION = kPrecisionHigh> FloatV4 p2ff(const FloatV4 & p);

template <Precision PRECISION = kPrecisionHigh> FloatV4 p2ff(const FloatV4 & p, const FloatV4 & base);

template <Precision PRECISION = kPrecisionHigh> FloatV4 f2p(const FloatV4 & f);

template <Precision PRECISION = kPrecisionHigh> FloatV4 f2p(const FloatV4 & f, const FloatV4 & base);


template <Precision PRECISION = kPrecisionHigh> FloatV4 db2a(const FloatV4 & db);

template <Precision PRECISION = kPrecisionHigh> FloatV4 a2db(const FloatV4 & a);

REFLEX_END




//
//implementation

REFLEX_NS(DSP::Detail)

template <class TYPE>
struct ConvertConstants
{
	static const TYPE f_db2a_mult, f_a2db_mult;

	static const TYPE f_log2_rcp_onehundred;
};

REFLEX_END

template <class TYPE> const TYPE DSP::Detail::ConvertConstants<TYPE>::f_db2a_mult(Log2(1.12202f));

template <class TYPE> const TYPE DSP::Detail::ConvertConstants<TYPE>::f_a2db_mult(1.0f / Log2(1.12202f));

template <class TYPE> const TYPE DSP::Detail::ConvertConstants<TYPE>::f_log2_rcp_onehundred(Log2(1.0f / 100.0f));

REFLEX_INLINE constexpr Float64 DSP::beats2bps(Float64 sr, Float64 length, Float64 nbeats)
{
	return (nbeats * sr) / length;
}

REFLEX_INLINE constexpr Float64 DSP::bps2beats(Float64 sr, Float64 length, Float64 bps)
{
	return (bps * length) / sr;
}

REFLEX_INLINE constexpr Float DSP::ms2secs(Float seconds)
{
	return seconds * 0.001f;
}

REFLEX_INLINE constexpr Float DSP::hz2ms(Float hz)
{
	return 1000.0f / hz;
}

REFLEX_INLINE constexpr Float DSP::ms2hz(Float ms)
{
	return 1000.0f / ms;
}

REFLEX_INLINE constexpr Float DSP::ms2samples(Float ms, Float sr)
{
	return ms * (sr * 0.001f);
}

REFLEX_INLINE constexpr Float DSP::samples2ms(Float n, Float sr)
{
	return n / (sr * 0.001f);
}

template <DSP::Precision PRECISION, class FLOAT> REFLEX_INLINE FLOAT DSP::p2f(const FLOAT & p)
{
	typedef Detail::Constants <FLOAT> Constants;

	return Exp2<PRECISION>((p - Constants::f_sixtynine) * Constants::f_rcp_twelve) * Constants::f_fourhundred_forty;
}

template <DSP::Precision PRECISION> REFLEX_INLINE Float DSP::p2f_LEGACY(Float p)
{
	return Exp2<PRECISION>((p - 9.0f) * (1.0f / 12.0f)) * 440.0f;
}

template <DSP::Precision PRECISION> REFLEX_INLINE Float DSP::p2f_LEGACY(Float p, Float base)
{
	return Exp2<PRECISION>((p - (base + 9.0f)) * (1.0f / 12.0f)) * 440.0f;
}

template <DSP::Precision PRECISION> REFLEX_INLINE Float DSP::p2ff(Float p)
{
	return Exp2<PRECISION>(p * (1.0f / 12.0f));
}

template <DSP::Precision PRECISION> REFLEX_INLINE Float DSP::p2ff(Float p, Float base)
{
	return Exp2<PRECISION>((p - base) * (1.0f / 12.0f));
}

template <DSP::Precision PRECISION> REFLEX_INLINE Float DSP::f2p(Float f)
{
	return (Log2<PRECISION>(f * (1.0f / 440.0f)) * 12.0f) + 9.0f;
}

template <DSP::Precision PRECISION> REFLEX_INLINE Float DSP::f2p(Float f, Float base)
{
	return (Log2<PRECISION>(f * (1.0f / 440.0f)) * 12.0f) + (9.0f + base);
}

template <DSP::Precision PRECISION> REFLEX_INLINE Float DSP::db2a(Float db)
{
	return Exp2<PRECISION>(db * 0.166098392206649f);
}

template <DSP::Precision PRECISION> REFLEX_INLINE Float DSP::a2db(Float a)
{
	return Log2<PRECISION>(a) * Detail::ConvertConstants<Float32>::f_a2db_mult;
}

inline constexpr Float DSP::beats2hz(Float bps, Float beats)
{
	return bps / beats;
}

template <DSP::Precision PRECISION> REFLEX_INLINE Float DSP::samples2decay(Float n, Float floor)
{
	return Exp2<PRECISION>(Log2<PRECISION>(floor) / Max(n - 1.0f, 1.0f));
}




//
//v4

REFLEX_INLINE FloatV4 DSP::hz2ms(const FloatV4 & hz)
{
	return Detail::Constants<FloatV4>::f_onethousand / hz;
}

REFLEX_INLINE FloatV4 DSP::ms2hz(const FloatV4 & ms)
{
	return Detail::Constants<FloatV4>::f_onethousand / ms;
}

REFLEX_INLINE FloatV4 DSP::ms2samples(const FloatV4 & ms, const FloatV4 & sr)
{
	return ms * (sr * Detail::Constants<FloatV4>::f_rcp_onethousand);
}

REFLEX_INLINE FloatV4 DSP::samples2ms(const FloatV4 & n, const FloatV4 & sr)
{
	return n / (sr * Detail::Constants<FloatV4>::f_rcp_onethousand);
}	

template <DSP::Precision PRECISION> REFLEX_INLINE FloatV4 DSP::p2f_LEGACY(const FloatV4 & p)
{
	return Exp2<PRECISION>((p - Detail::Constants<FloatV4>::f_nine) * Detail::Constants<FloatV4>::f_rcp_twelve) * Detail::Constants<FloatV4>::f_fourhundred_forty;
}

template <DSP::Precision PRECISION> REFLEX_INLINE FloatV4 DSP::p2f_LEGACY(const FloatV4 & p, const FloatV4 & base)
{
	return Exp2<PRECISION>((p - (base + Detail::Constants<FloatV4>::f_nine)) * Detail::Constants<FloatV4>::f_rcp_twelve) * Detail::Constants<FloatV4>::f_fourhundred_forty;
}

template <DSP::Precision PRECISION> REFLEX_INLINE FloatV4 DSP::p2ff(const FloatV4 & p)
{
	return Exp2<PRECISION>(p * Detail::Constants<FloatV4>::f_rcp_twelve);
}

template <DSP::Precision PRECISION> REFLEX_INLINE FloatV4 DSP::p2ff(const FloatV4 & p, const FloatV4 & base)
{
	return Exp2<PRECISION>((p - base) * Detail::Constants<FloatV4>::f_rcp_twelve);
}

//template <DSP::Precision PRECISION> REFLEX_INLINE FloatV4 DSP::f2p(const FloatV4 & f)
//{
//	//return (DSP::Log2<PRECISION>(f * (1.0f / 440.0f)) * 12.0f) + 69.0f;
//
//	REFLEX_ASSERT(false);
//
//	return 0.0f;
//}

template <DSP::Precision PRECISION> REFLEX_INLINE FloatV4 DSP::db2a(const FloatV4 & db)
{
	return Exp2<PRECISION>(db * Detail::ConvertConstants<FloatV4>::f_db2a_mult);
}

template <DSP::Precision PRECISION> REFLEX_INLINE FloatV4 DSP::a2db(const FloatV4 & a)
{
	return Log2<PRECISION>(a) * Detail::ConvertConstants<FloatV4>::f_a2db_mult;
}
