#pragma once

#include "../defines.h"
#include "../detail/constants.h"




//
//Public API

REFLEX_NS(DSP)

//round

Float32 RoundNearest(Float32 value);

FloatV4 RoundNearest(const FloatV4 & value);



//truncate

Float32 IntToFloat(Int value);



//log/exp

template <Precision PRECISION = kPrecisionHigh> FloatV4 Exp(const FloatV4 & value);


template <Precision PRECISION = kPrecisionHigh> Float32 Log2(Float32 value);

template <Precision PRECISION = kPrecisionHigh> FloatV4 Log2(const FloatV4 & value);

	
template <Precision PRECISION = kPrecisionHigh> Float32 Exp2(Float32 value);

template <Precision PRECISION = kPrecisionHigh> FloatV4 Exp2(const FloatV4 & value);

	

//trig

template <Precision PRECISION = kPrecisionHigh> Float32 Sin(Float32 value);

template <Precision PRECISION = kPrecisionHigh> FloatV4 Sin(const FloatV4 & value);


template <Precision PRECISION = kPrecisionHigh> Float32 Cos(Float32 value);

template <Precision PRECISION = kPrecisionHigh> FloatV4 Cos(const FloatV4 & value);


template <Precision PRECISION = kPrecisionHigh> Pair <Float32> SinCos(Float32 value);

template <Precision PRECISION = kPrecisionHigh> Pair <FloatV4> SinCos(const FloatV4 & value);


template <Precision PRECISION = kPrecisionHigh> FloatV4 Atan(const FloatV4 & value);


template <Precision PRECISION = kPrecisionHigh> Float Atan2(Float y, Float x);

template <Precision PRECISION = kPrecisionHigh> FloatV4 Atan2(const FloatV4 & y, const FloatV4 & x);


template <Precision PRECISION = kPrecisionHigh> FloatV4 Tanh(const FloatV4 & value);


template <UInt BLOCK_SIZE> constexpr Pair <UInt> SplitBlocks(UInt samples);

REFLEX_END




//
//impl

template <class TYPE> const TYPE DSP::Detail::Constants<TYPE>::f_ms2mult = DSP::Log2(0.05f);

REFLEX_NS(DSP::Detail)

inline const Float32x4 kTrigSignMask = Reinterpret<Float32x4>(REFLEX_SET1_I32_X4(0x80000000));

inline const Float32x4 kTrigInvSignMask = Reinterpret<Float32x4>(REFLEX_SET1_I32_X4(~0x80000000));

inline const FloatV4 kSinCos_t1 = { -2.39e-008f, 2.605e-007f, 0.0f, 0.0f };
inline const FloatV4 kSinCos_t2 = { 2.7526e-006f, 2.47609e-005f, 0.0f, 0.0f };
inline const FloatV4 kSinCos_t4 = { 0.000198409f, 0.00138884f, 0.0f, 0.0f };
inline const FloatV4 kSinCos_t6 = { 0.00833333f, 0.0416666f, 0.0f, 0.0f };
inline const FloatV4 kSinCos_t8 = { 0.166667f, 0.499923f, 0.0f, 0.0f };

template <class TYPE> 
struct TrigConstants
{
	inline static const TYPE f_cephes_FOPI = 1.27323954473516f; // 4 / M_PI

	inline static const TYPE f_minus_cephes_DP1 = -0.78515625f;
	inline static const TYPE f_minus_cephes_DP2 = -2.4187564849853515625e-4f;
	inline static const TYPE f_minus_cephes_DP3 = -3.77489497744594108e-8f;

	inline static const TYPE f_coscof_p0 = 2.443315711809948E-005f;
	inline static const TYPE f_coscof_p1 = -1.388731625493765E-003f;
	inline static const TYPE f_coscof_p2 = 4.166664568298827E-002f;

	inline static const TYPE f_sincof_p0 = -1.9515295891E-4f;
	inline static const TYPE f_sincof_p1 = 8.3321608736E-3f;
	inline static const TYPE f_sincof_p2 = -1.6666654611E-1f;

	inline static const TYPE f_quarter_pi = Float(kPi / 4.0);
	inline static const TYPE f_half_pi = Float(kPi / 2.0);
	inline static const TYPE f_four_pi = Float(kPi * 4.0);

	inline static const TYPE f_cephes_PIO2F = 1.5707963267948966192f;
	inline static const TYPE f_cephes_PIO4F = 0.7853981633974483096f;

	inline static const TYPE f_atanrange_hi = 2.414213562373095f;
	inline static const TYPE f_atanrange_lo = 0.4142135623730950f;

	inline static const TYPE f_atancof_p0 = 8.05374449538e-2f;
	inline static const TYPE f_atancof_p1 = 1.38776856032E-1f;
	inline static const TYPE f_atancof_p2 = 1.99777106478E-1f;
	inline static const TYPE f_atancof_p3 = 3.33329491539E-1f;

	inline static const TYPE f_atan_blah1 = 0.2447f;
	inline static const TYPE f_atan_blah2 = 0.0663f;

	inline static const IntType <TYPE> i_pi32_inv1 = ~1;
	inline static const IntType <TYPE> i_two = 2;
	inline static const IntType <TYPE> i_four = 4;
};

template <class FLOAT> REFLEX_INLINE FLOAT SinApprox(const FLOAT & value)
{
	FLOAT t0 = value * value;
	FLOAT t1 = t0 * -2.39e-008f;
	FLOAT t2 = t1 + 2.7526e-006f;
	FLOAT t3 = t0 * t2;
	FLOAT t4 = t3 - 0.000198409f;
	FLOAT t5 = t0 * t4;
	FLOAT t6 = t5 + 0.00833333f;
	FLOAT t7 = t0 * t6;
	FLOAT t8 = t7 - 0.166667f;
	FLOAT t9 = t0 * t8;
	FLOAT t10 = t9 + Constants<FLOAT>::f_one;

	return value * t10;
}

template <class FLOAT> REFLEX_INLINE FLOAT CosApprox(const FLOAT & value)
{
	FLOAT t0 = value * value;
	FLOAT t1 = t0 * -2.605e-007f;
	FLOAT t2 = t1 + 2.47609e-005f;
	FLOAT t3 = t0 * t2;
	FLOAT t4 = t3 - 0.00138884f;
	FLOAT t5 = t0 * t4;
	FLOAT t6 = t5 + 0.0416666f;
	FLOAT t7 = t0 * t6;
	FLOAT t8 = t7 - 0.499923f;
	FLOAT t9 = t0 * t8;
	FLOAT t10 = t9 + Constants<FLOAT>::f_one;

	return t10;
}

extern const Float32 kExp2_32bit[1025];

REFLEX_END

REFLEX_INLINE Float32 DSP::RoundNearest(Float32 value)
{
	return Reflex::RoundNearest(value);
}

REFLEX_INLINE FloatV4 DSP::RoundNearest(const FloatV4 & value)
{
	return SIMD::RoundNearest(value);
}

REFLEX_INLINE Float32 DSP::IntToFloat(Reflex::Int value)
{
	return Float32(value);
}

template <DSP::Precision PRECISION> REFLEX_INLINE FloatV4 DSP::Exp(const FloatV4 & value)
{
	if constexpr (PRECISION == kPrecisionFast)
	{
		//not implemented

		return SIMD::Exp(value);
	}
	else
	{
		return SIMD::Exp(value);
	}
}

template <DSP::Precision PRECISION> REFLEX_INLINE Float32 DSP::Log2(Float32 value)
{
	if constexpr (PRECISION == kPrecisionFast)
	{
		Int i0 = reinterpret_cast<Int&>(value);
		Int i1 = i0;
		i0 &= 2139095040;

		i1 ^= i0;
		i1 |= 1065353216;
		Float32 fsign;
		reinterpret_cast<Int&>(fsign) = i1;

		i0 = Int(UInt(i0) >> 23);
		i0 -= 127;
		Int iexp = i0;

		Float32 f0 = fsign - 1;
		Float32 f1 = f0 * -0.346247f;
		f1 += 1.34625f;
		f0 *= f1;
		f0 += iexp;

		return f0;
	}
	else
	{
		return Reflex::Log2(value);
	}
}

template <DSP::Precision PRECISION> REFLEX_INLINE FloatV4 DSP::Log2(const FloatV4 & value)
{
	return SIMD::Log2(value);
}

template <DSP::Precision PRECISION> REFLEX_INLINE Float32 DSP::Exp2(Float32 value)
{
	if constexpr (PRECISION == kPrecisionFast)
	{
		Float32 fTemp1 = value;

		auto iTemp1 = ToInt32(fTemp1);

		Float32 fTemp2 = Float32(iTemp1);

		iTemp1 += 127;
		iTemp1 <<= 23;
		fTemp1 -= fTemp2;
		fTemp2 = fTemp1;
		fTemp2 *= 0.056f;
		fTemp2 += 0.242f;
		fTemp2 *= fTemp1;
		fTemp2 += 0.693f;
		fTemp1 *= fTemp2;
		fTemp1 += 1;
		fTemp1 *= Reinterpret<Float32>(iTemp1);

		return fTemp1;
	}
	//else if constexpr (PRECISION == old_PrecisionStandard)
	//{
	//	Float32 fTemp1 = value;

	//	auto iTemp1 = ToInt32(fTemp1);

	//	auto fTemp2 = ToFloat32(iTemp1);

	//	iTemp1 += 127;
	//	iTemp1 <<= 23;
	//	fTemp1 -= fTemp2;
	//	fTemp1 += 0.5f;
	//	fTemp1 *= 1021.f;
	//	fTemp2 = fTemp1;
	//	fTemp2 -= 0.5f;
	//	auto nTemp1 = ToInt32(fTemp2);
	//	fTemp2 = Float32(nTemp1);
	//	nTemp1 &= 1023;
	//	fTemp1 -= fTemp2;
	//	fTemp2 = 1 - fTemp1;
	//	REFLEX_ASSERT(UInt(nTemp1) < 1024);
	//	fTemp2 *= Detail::kExp2_32bit[nTemp1];
	//	fTemp1 *= Detail::kExp2_32bit[nTemp1 + 1];
	//	fTemp1 += fTemp2;
	//	fTemp1 *= Reinterpret<Float32>(iTemp1);

	//	return fTemp1;
	//}
	else
	{
		//benchmarked Feb 2026, on intel, std::exp2 was faster than commented out code

		return Reflex::Exp2(value);
	}
}

template <DSP::Precision PRECISION> REFLEX_INLINE FloatV4 DSP::Exp2(const FloatV4 & value)
{
	return SIMD::Exp2(value);
}

template <DSP::Precision PRECISION> REFLEX_INLINE Float32 DSP::Sin(Float32 value)
{
	if constexpr (PRECISION == kPrecisionFast)
	{
		return Detail::SinApprox<Float32>(value);
	}
	else
	{
		return Reflex::Sin(value);
	}
}

template <DSP::Precision PRECISION> REFLEX_INLINE FloatV4 DSP::Sin(const FloatV4 & value)
{
	if constexpr (PRECISION == kPrecisionFast)
	{
		return Detail::SinApprox<FloatV4>(value);
	}
	else
	{
		typedef Detail::TrigConstants <FloatV4> TrigConstants;

		if constexpr (PRECISION == DSP::kPrecisionHigh)
		{
			//TODO MOD INPUT
		}
		
		// *** FROM INTEL SIMD LIBRARY ***

		/* extract the sign bit (upper one) */
		auto sign_bit = REFLEX_AND_F32_X4(Cast<Float32x4>(value), Detail::kTrigSignMask);
		/* take the absolute value */
		auto x = REFLEX_XOR_F32_X4(Cast<Float32x4>(value), sign_bit);

		/* scale by 4/Pi */
		auto y = REFLEX_MUL_F32_X4(x, TrigConstants::f_cephes_FOPI);

		/* store the integer part of y in mm0 */
		auto emm2 = REFLEX_TRUNCATE_F32_X4(y);
		emm2 = REFLEX_ADD_I32_X4(emm2, Constants<FloatV4>::i_one);
		emm2 = REFLEX_AND_I32_X4(emm2, TrigConstants::i_pi32_inv1);
		y = REFLEX_I32_X4_TO_F32_X4(emm2);

		/* get the swap sign flag */
		auto emm0 = REFLEX_SHL1_I32_X4(REFLEX_AND_I32_X4(emm2, TrigConstants::i_four), 29);
		/* get the polynom selection mask
			there is one polynom for 0 <= x <= Pi/4
			and another one for Pi/4<x<=Pi/2
			Both branches will be computed.
		*/
		emm2 = REFLEX_AND_I32_X4(emm2, TrigConstants::i_two);
		emm2 = REFLEX_EQ_I32_X4(emm2, REFLEX_ZERO_I32_X4());

		auto swap_sign_bit = Reinterpret<Float32x4>(emm0);
		auto poly_mask = Reinterpret<Float32x4>(emm2);
		sign_bit = REFLEX_XOR_F32_X4(sign_bit, swap_sign_bit);

		/* The magic pass: "Extended precision modular arithmetic"
			x = ((x - y * DP1) - y * DP2) - y * DP3; */
		auto xmm1 = REFLEX_MUL_F32_X4(y, TrigConstants::f_minus_cephes_DP1);
		auto xmm2 = REFLEX_MUL_F32_X4(y, TrigConstants::f_minus_cephes_DP2);
		auto xmm3 = REFLEX_MUL_F32_X4(y, TrigConstants::f_minus_cephes_DP3);
		
		x = REFLEX_ADD_F32_X4(REFLEX_ADD_F32_X4(x, xmm1), REFLEX_ADD_F32_X4(xmm2, xmm3));

		/* Evaluate the first polynom  (0 <= x <= Pi/4) */
		y = TrigConstants::f_coscof_p0;
		auto z = REFLEX_MUL_F32_X4(x, x);

		y = REFLEX_MUL_F32_X4(y, z);
		y = REFLEX_ADD_F32_X4(y, TrigConstants::f_coscof_p1);
		y = REFLEX_MUL_F32_X4(y, z);
		y = REFLEX_ADD_F32_X4(y, TrigConstants::f_coscof_p2);
		y = REFLEX_MUL_F32_X4(y, REFLEX_MUL_F32_X4(z, z));
		auto tmp = REFLEX_MUL_F32_X4(z, Constants<FloatV4>::f_rcp_two);
		y = REFLEX_SUB_F32_X4(y, tmp);
		y = REFLEX_ADD_F32_X4(y, Constants<FloatV4>::f_one);

		/* Evaluate the second polynom  (Pi/4 <= x <= 0) */

		auto y2 = TrigConstants::f_sincof_p0;
		y2 = REFLEX_MUL_F32_X4(y2, z);
		y2 = REFLEX_ADD_F32_X4(y2, TrigConstants::f_sincof_p1);
		y2 = REFLEX_MUL_F32_X4(y2, z);
		y2 = REFLEX_ADD_F32_X4(y2, TrigConstants::f_sincof_p2);
		y2 = REFLEX_MUL_F32_X4(y2, REFLEX_MUL_F32_X4(z, x));
		y2 = REFLEX_ADD_F32_X4(y2, x);

		/* select the correct result from the two polynoms */
		xmm3 = poly_mask;
		y2 = REFLEX_AND_F32_X4(xmm3, y2); //, xmm3);
		y = REFLEX_ANDNOT_F32_X4(xmm3, y);
		y = REFLEX_ADD_F32_X4(y, y2);
		/* update the sign */
		y = REFLEX_XOR_F32_X4(y, sign_bit);

		return y;
	}
}

template <DSP::Precision PRECISION> REFLEX_INLINE Float32 DSP::Cos(Float32 value)
{
	if constexpr (PRECISION == kPrecisionFast)
	{
		return Detail::CosApprox<Float32>(value);
	}
	else
	{
		return Reflex::Cos(value);
	}
}

template <DSP::Precision PRECISION> REFLEX_INLINE FloatV4 DSP::Cos(const FloatV4 & value)
{
	if constexpr (PRECISION == kPrecisionFast)
	{
		return Detail::CosApprox<FloatV4>(value);
	}
	else
	{
		typedef Detail::TrigConstants <FloatV4> TrigConstants;

		/* take the absolute value */
		auto x = REFLEX_AND_F32_X4(Cast<Float32x4>(value), Detail::kTrigInvSignMask);
		/* scale by 4/Pi */
		auto y = REFLEX_MUL_F32_X4(x, TrigConstants::f_cephes_FOPI);

		/* store the integer part of y in mm0 */
		auto emm2 = REFLEX_TRUNCATE_F32_X4(y);
		emm2 = REFLEX_ADD_I32_X4(emm2, Constants<FloatV4>::i_one);
		emm2 = REFLEX_AND_I32_X4(emm2, TrigConstants::i_pi32_inv1);
		y = REFLEX_I32_X4_TO_F32_X4(emm2);

		emm2 = REFLEX_SUB_I32_X4(emm2, TrigConstants::i_two);

		/* get the swap sign flag */
		auto emm0 = REFLEX_ANDNOT_I32_X4(emm2, TrigConstants::i_four);
		emm0 = REFLEX_SHL1_I32_X4(emm0, 29);
		/* get the polynom selection mask */
		emm2 = REFLEX_AND_I32_X4(emm2, TrigConstants::i_two);
		emm2 = REFLEX_EQ_I32_X4(emm2, REFLEX_ZERO_I32_X4());

		auto sign_bit = Reinterpret<Float32x4>(emm0);
		auto poly_mask = Reinterpret<Float32x4>(emm2);
		/* magic pass: "Extended precision modular arithmetic"
			x = ((x - y * DP1) - y * DP2) - y * DP3; */
		auto xmm1 = REFLEX_MUL_F32_X4(y, TrigConstants::f_minus_cephes_DP1);
		auto xmm2 = REFLEX_MUL_F32_X4(y, TrigConstants::f_minus_cephes_DP2);
		auto xmm3 = REFLEX_MUL_F32_X4(y, TrigConstants::f_minus_cephes_DP3);
		x = REFLEX_ADD_F32_X4(REFLEX_ADD_F32_X4(x, xmm1), REFLEX_ADD_F32_X4(xmm2, xmm3));

		/* Evaluate the first polynom  (0 <= x <= Pi/4) */
		y = TrigConstants::f_coscof_p0;
		auto z = REFLEX_MUL_F32_X4(x, x);

		y = REFLEX_MUL_F32_X4(y, z);
		y = REFLEX_ADD_F32_X4(y, TrigConstants::f_coscof_p1);
		y = REFLEX_MUL_F32_X4(y, z);
		y = REFLEX_ADD_F32_X4(y, TrigConstants::f_coscof_p2);
		y = REFLEX_MUL_F32_X4(y, REFLEX_MUL_F32_X4(z, z));
		auto tmp = REFLEX_MUL_F32_X4(z, Constants<FloatV4>::f_rcp_two);
		y = REFLEX_SUB_F32_X4(y, tmp);
		y = REFLEX_ADD_F32_X4(y, Constants<FloatV4>::f_one);

		/* Evaluate the second polynom  (Pi/4 <= x <= 0) */

		auto y2 = TrigConstants::f_sincof_p0;
		y2 = REFLEX_MUL_F32_X4(y2, z);
		y2 = REFLEX_ADD_F32_X4(y2, TrigConstants::f_sincof_p1);
		y2 = REFLEX_MUL_F32_X4(y2, z);
		y2 = REFLEX_ADD_F32_X4(y2, TrigConstants::f_sincof_p2);
		y2 = REFLEX_MUL_F32_X4(y2, REFLEX_MUL_F32_X4(z, x));
		y2 = REFLEX_ADD_F32_X4(y2, x);

		/* select the correct result from the two polynoms */
		xmm3 = poly_mask;
		y2 = REFLEX_AND_F32_X4(xmm3, y2); //, xmm3);
		y = REFLEX_ANDNOT_F32_X4(xmm3, y);
		y = REFLEX_ADD_F32_X4(y, y2);
		/* update the sign */
		y = REFLEX_XOR_F32_X4(y, sign_bit);

		return y;
	}
}

template <DSP::Precision PRECISION> REFLEX_INLINE Pair <Float32> DSP::SinCos(Float32 value)
{
	if constexpr (PRECISION == kPrecisionFast)
	{
		FloatV4 mult = { value, 1.0f, 0.0f, 0.0f };

		FloatV4 t0 = value * value;
		FloatV4 t1 = t0 * Detail::kSinCos_t1;
		FloatV4 t2 = t1 + Detail::kSinCos_t2;
		FloatV4 t3 = t0 * t2;
		FloatV4 t4 = t3 - Detail::kSinCos_t4;
		FloatV4 t5 = t0 * t4;
		FloatV4 t6 = t5 + Detail::kSinCos_t6;
		FloatV4 t7 = t0 * t6;
		FloatV4 t8 = t7 - Detail::kSinCos_t8;
		FloatV4 t9 = t0 * t8;
		FloatV4 t10 = t9 + Constants<FloatV4>::f_one;
		FloatV4 t11 = mult * t10;

		return Reinterpret<Pair<Float32>>(t11);
	}
	else
	{
		return { Sin<PRECISION>(value), Cos<PRECISION>(value) };
	}
}

template <DSP::Precision PRECISION> REFLEX_INLINE Pair <FloatV4> DSP::SinCos(const FloatV4 & value)
{
	REFLEX_STATIC_ASSERT(PRECISION != kPrecisionFast);

	if constexpr (PRECISION == kPrecisionFast)
	{
		return {};
	}
	else
	{
		typedef Detail::TrigConstants <FloatV4> TrigConstants;

		/* extract the sign bit (upper one) */
		auto sign_bit_sin = REFLEX_AND_F32_X4(value, Detail::kTrigSignMask);
		/* take the absolute value */
		auto x = REFLEX_XOR_F32_X4(value, sign_bit_sin);

		/* scale by 4/Pi */
		auto y = REFLEX_MUL_F32_X4(x, TrigConstants::f_cephes_FOPI);

		/* store the integer part of y in emm2 */
		auto emm2 = REFLEX_TRUNCATE_F32_X4(y);

		/* j=(j+1) & (~1) (see the cephes sources) */
		emm2 = REFLEX_ADD_I32_X4(emm2, Constants<FloatV4>::i_one);
		emm2 = REFLEX_AND_I32_X4(emm2, TrigConstants::i_pi32_inv1);
		y = REFLEX_I32_X4_TO_F32_X4(emm2);

		auto emm4 = emm2;

		/* get the swap sign flag for the sine */
		auto emm0 = REFLEX_AND_I32_X4(emm2, TrigConstants::i_four);
		emm0 = REFLEX_SHL1_I32_X4(emm0, 29);
		auto swap_sign_bit_sin = Reinterpret<Float32x4>(emm0);

		/* get the polynom selection mask for the sine*/
		emm2 = REFLEX_AND_I32_X4(emm2, TrigConstants::i_two);
		emm2 = REFLEX_EQ_I32_X4(emm2, REFLEX_ZERO_I32_X4());
		auto poly_mask = Reinterpret<Float32x4>(emm2);
		/* magic pass: "Extended precision modular arithmetic"
			x = ((x - y * DP1) - y * DP2) - y * DP3; */
		auto xmm1 = REFLEX_MUL_F32_X4(y, TrigConstants::f_minus_cephes_DP1);
		auto xmm2 = REFLEX_MUL_F32_X4(y, TrigConstants::f_minus_cephes_DP2);
		auto xmm3 = REFLEX_MUL_F32_X4(y, TrigConstants::f_minus_cephes_DP3);
		x = REFLEX_ADD_F32_X4(REFLEX_ADD_F32_X4(x, xmm1), REFLEX_ADD_F32_X4(xmm2, xmm3));

		emm4 = REFLEX_SUB_I32_X4(emm4, TrigConstants::i_two);
		emm4 = REFLEX_ANDNOT_I32_X4(emm4, TrigConstants::i_four);
		emm4 = REFLEX_SHL1_I32_X4(emm4, 29);
		auto sign_bit_cos = Reinterpret<Float32x4>(emm4);

		sign_bit_sin = REFLEX_XOR_F32_X4(sign_bit_sin, swap_sign_bit_sin);

		/* Evaluate the first polynom  (0 <= x <= Pi/4) */
		auto z = REFLEX_MUL_F32_X4(x, x);
		y = TrigConstants::f_coscof_p0;

		y = REFLEX_MUL_F32_X4(y, z);
		y = REFLEX_ADD_F32_X4(y, TrigConstants::f_coscof_p1);
		y = REFLEX_MUL_F32_X4(y, z);
		y = REFLEX_ADD_F32_X4(y, TrigConstants::f_coscof_p2);
		y = REFLEX_MUL_F32_X4(y, REFLEX_MUL_F32_X4(z, z));
		auto tmp = REFLEX_MUL_F32_X4(z, Constants<FloatV4>::f_rcp_two);
		y = REFLEX_SUB_F32_X4(y, tmp);
		y = REFLEX_ADD_F32_X4(y, Constants<FloatV4>::f_one);

		/* Evaluate the second polynom  (Pi/4 <= x <= 0) */

		auto y2 = TrigConstants::f_sincof_p0;
		y2 = REFLEX_MUL_F32_X4(y2, z);
		y2 = REFLEX_ADD_F32_X4(y2, TrigConstants::f_sincof_p1);
		y2 = REFLEX_MUL_F32_X4(y2, z);
		y2 = REFLEX_ADD_F32_X4(y2, TrigConstants::f_sincof_p2);
		y2 = REFLEX_MUL_F32_X4(y2, REFLEX_MUL_F32_X4(z, x));
		y2 = REFLEX_ADD_F32_X4(y2, x);

		/* select the correct result from the two polynoms */
		xmm3 = poly_mask;
		auto ysin2 = REFLEX_AND_F32_X4(xmm3, y2);
		auto ysin1 = REFLEX_ANDNOT_F32_X4(xmm3, y);
		y2 = REFLEX_SUB_F32_X4(y2, ysin2);
		y = REFLEX_SUB_F32_X4(y, ysin1);

		xmm1 = REFLEX_ADD_F32_X4(ysin1, ysin2);
		xmm2 = REFLEX_ADD_F32_X4(y, y2);

		return { REFLEX_XOR_F32_X4(xmm1, sign_bit_sin), REFLEX_XOR_F32_X4(xmm2, sign_bit_cos) };
	}
}

template <DSP::Precision PRECISION> REFLEX_INLINE FloatV4 DSP::Atan(const FloatV4 & value)
{
	typedef Detail::TrigConstants <FloatV4> TrigConstants;

	if constexpr (PRECISION == kPrecisionFast)
	{
		auto t0 = REFLEX_ANDNOT_F32_X4(Detail::kTrigSignMask, value);
		auto t1 = REFLEX_MUL_F32_X4(TrigConstants::f_atan_blah2, t0);
		auto t3 = REFLEX_ADD_F32_X4(TrigConstants::f_atan_blah1, t1);
		auto t4 = REFLEX_ANDNOT_F32_X4(Detail::kTrigSignMask, value);
		auto t5 = REFLEX_SUB_F32_X4(t4, SIMD::kfOne);
		auto t6 = REFLEX_MUL_F32_X4(value, t5);
		auto t7 = REFLEX_MUL_F32_X4(t6, t3);
		auto t8 = REFLEX_MUL_F32_X4(TrigConstants::f_quarter_pi, value);

		return REFLEX_SUB_F32_X4(t8, t7);
	}
	else
	{
		/* take the absolute value */
		auto x = REFLEX_AND_F32_X4(value, Detail::kTrigInvSignMask);
		/* extract the sign bit (upper one) */
		auto sign_bit = REFLEX_AND_F32_X4(value, Detail::kTrigSignMask);

		auto cmp0 = REFLEX_GT_F32_X4(x, TrigConstants::f_atanrange_hi.data);

		auto cmp1 = Reinterpret<Float32x4>(REFLEX_GT_F32_X4(x, TrigConstants::f_atanrange_lo.data));

		auto cmp2 = REFLEX_ANDNOT_F32_X4(cmp0, cmp1);

		/* -( 1.0/x ) */
		auto y0 = REFLEX_AND_F32_X4(cmp0, TrigConstants::f_cephes_PIO2F);
		auto x0 = REFLEX_DIV_F32_X4(SIMD::kfOne, x);
		x0 = REFLEX_XOR_F32_X4(x0, Detail::kTrigSignMask);

		auto y1 = REFLEX_AND_F32_X4(cmp2, TrigConstants::f_cephes_PIO4F);
		/* (x-1.0)/(x+1.0) */
		auto x1_o = REFLEX_SUB_F32_X4(x, Detail::Constants<FloatV4>::f_one);
		auto x1_u = REFLEX_ADD_F32_X4(x, Detail::Constants<FloatV4>::f_one);
		auto x1 = REFLEX_DIV_F32_X4(x1_o, x1_u);

		auto x2 = REFLEX_AND_F32_X4(cmp2, x1);
		x0 = REFLEX_AND_F32_X4(cmp0, x0);
		x2 = REFLEX_OR_F32_X4(x2, x0);
		cmp1 = REFLEX_OR_F32_X4(cmp0, cmp2);
		x2 = REFLEX_AND_F32_X4(cmp1, x2);
		x = REFLEX_ANDNOT_F32_X4(cmp1, x);
		x = REFLEX_OR_F32_X4(x2, x);

		auto y = REFLEX_OR_F32_X4(y0, y1);

		auto zz = REFLEX_MUL_F32_X4(x, x);
		auto acc = TrigConstants::f_atancof_p0;
		acc = REFLEX_MUL_F32_X4(acc, zz);
		acc = REFLEX_SUB_F32_X4(acc, TrigConstants::f_atancof_p1);
		acc = REFLEX_MUL_F32_X4(acc, zz);
		acc = REFLEX_ADD_F32_X4(acc, TrigConstants::f_atancof_p2);
		acc = REFLEX_MUL_F32_X4(acc, zz);
		acc = REFLEX_SUB_F32_X4(acc, TrigConstants::f_atancof_p3);
		acc = REFLEX_MUL_F32_X4(acc, zz);
		acc = REFLEX_MUL_F32_X4(acc, x);
		acc = REFLEX_ADD_F32_X4(acc, x);
		y = REFLEX_ADD_F32_X4(y, acc);


		/* update the sign */
		return REFLEX_XOR_F32_X4(y, sign_bit);
	}
}

template <DSP::Precision PRECISION> REFLEX_INLINE Float DSP::Atan2(Float y, Float x)
{
	if constexpr (PRECISION == kPrecisionFast)
	{
		constexpr Float kPiDivideByFour = 0.78539816339f;   // pi / 4
		constexpr Float kThreeQuarterPi = 2.35619449019f; // pi * 0.75f

		Float abs_y = Abs(y) + 1e-10f; // kludge to prevent 0/0
		Float angle;

		if (x >= 0.0f)
		{
			Float r = (x - abs_y) / (x + abs_y);

			angle = kPiDivideByFour - kPiDivideByFour * r;
		}
		else
		{
			Float r = (x + abs_y) / (abs_y - x);

			angle = kThreeQuarterPi - kPiDivideByFour * r;
		}

		return (y < 0.0f ? -angle : angle);
	}
	else
	{
		return Reflex::Atan2(y, x);
	}
}

template <DSP::Precision PRECISION> REFLEX_INLINE FloatV4 DSP::Atan2(const FloatV4 & y, const FloatV4 & x)
{
	typedef Detail::TrigConstants <FloatV4> TrigConstants;

	if constexpr (PRECISION == kPrecisionFast)
	{
		auto zero = SIMD::kfZero;

		auto absx_gt_absy = REFLEX_GT_F32_X4(REFLEX_ANDNOT_F32_X4(Detail::kTrigSignMask, x), REFLEX_ANDNOT_F32_X4(Detail::kTrigSignMask, y));
		
		Float32x4 ratio = REFLEX_DIV_F32_X4(REFLEX_ADD_F32_X4(REFLEX_AND_F32_X4(absx_gt_absy, y), REFLEX_ANDNOT_F32_X4(absx_gt_absy, x)),
			REFLEX_ADD_F32_X4(REFLEX_AND_F32_X4(absx_gt_absy, x), REFLEX_ANDNOT_F32_X4(absx_gt_absy, y)));

		auto atan = Atan<PRECISION>(ratio);

		auto xgreaterthan0 = REFLEX_GT_F32_X4(x, zero);
		auto ygreaterthan0 = REFLEX_GT_F32_X4(y, zero);

		atan = REFLEX_XOR_F32_X4(atan, REFLEX_ANDNOT_F32_X4(absx_gt_absy, REFLEX_AND_F32_X4(xgreaterthan0, Detail::kTrigSignMask))); //negate atan if absx<=absy & x>0

		Float32x4 shift = TrigConstants::f_four_pi;
		shift = REFLEX_SUB_F32_X4(shift, REFLEX_ANDNOT_F32_X4(absx_gt_absy, TrigConstants::f_half_pi)); //substract halfpi if absx<=absy
		shift = REFLEX_XOR_F32_X4(shift, REFLEX_ANDNOT_F32_X4(ygreaterthan0, Detail::kTrigSignMask)); //negate shift if y<=0
		shift = REFLEX_ANDNOT_F32_X4(REFLEX_AND_F32_X4(absx_gt_absy, xgreaterthan0), shift); //null if abs>absy & x>0

		auto output = REFLEX_ADD_F32_X4(atan, shift);

		return SIMD::SelectNot(SIMD::Or(y == zero, x == zero), Reinterpret<FloatV4>(output));
	}
	else
	{
		auto zero = SIMD::kfZero.data;

		auto x_eq_0 = REFLEX_EQ_F32_X4(x.data, zero);
		auto x_gt_0 = REFLEX_GT_F32_X4(x.data, zero);
		auto x_le_0 = REFLEX_LTEQ_F32_X4(x.data, zero);
		auto y_eq_0 = REFLEX_EQ_F32_X4(y.data, zero);
		auto x_lt_0 = REFLEX_LT_F32_X4(x.data, zero);
		auto y_lt_0 = REFLEX_LT_F32_X4(y.data, zero);

		auto zero_mask = REFLEX_AND_F32_X4(x_eq_0, y_eq_0);
		auto zero_mask_other_case = REFLEX_AND_F32_X4(y_eq_0, x_gt_0);
		zero_mask = REFLEX_OR_F32_X4(zero_mask, zero_mask_other_case);

		auto pio2_mask = REFLEX_ANDNOT_F32_X4(y_eq_0, x_eq_0);
		auto pio2_mask_sign = REFLEX_AND_F32_X4(y_lt_0, Detail::kTrigSignMask);
		auto pio2_result = TrigConstants::f_cephes_PIO2F;
		pio2_result = REFLEX_XOR_F32_X4(pio2_result, pio2_mask_sign);
		pio2_result = REFLEX_AND_F32_X4(pio2_mask, pio2_result);

		auto pi_mask = REFLEX_AND_F32_X4(y_eq_0, x_le_0);

		auto pi_result = REFLEX_AND_F32_X4(pi_mask, Constants<FloatV4>::f_pi);

		auto swap_sign_mask_offset = REFLEX_AND_F32_X4(x_lt_0, y_lt_0);
		swap_sign_mask_offset = REFLEX_AND_F32_X4(swap_sign_mask_offset, Detail::kTrigSignMask);

		auto offset0 = REFLEX_ZERO_F32_X4();

		auto offset1 = REFLEX_XOR_F32_X4(Constants<FloatV4>::f_pi, swap_sign_mask_offset);

		auto offset = REFLEX_ANDNOT_F32_X4(x_lt_0, offset0);
		offset = REFLEX_AND_F32_X4(x_lt_0, offset1);

		auto arg = REFLEX_DIV_F32_X4(y, x);
		auto atan_result = Atan<PRECISION>(arg);
		atan_result = REFLEX_ADD_F32_X4(atan_result, offset);

		/* select between zero_result, pio2_result and atan_result */

		auto result = REFLEX_ANDNOT_F32_X4(zero_mask, pio2_result);
		atan_result = REFLEX_ANDNOT_F32_X4(pio2_mask, atan_result);
		//atan_result = REFLEX_ANDNOT_F32_X4( pio2_mask, atan_result);
		result = REFLEX_OR_F32_X4(result, atan_result);
		result = REFLEX_OR_F32_X4(result, pi_result);

		return SIMD::SelectNot(Reinterpret<BoolV4>(zero_mask), Reinterpret<FloatV4>(result));
	}
}

template <DSP::Precision PRECISION> REFLEX_INLINE FloatV4 DSP::Tanh(const FloatV4 & value)
{
	auto e = Exp<PRECISION>(Constants<FloatV4>::f_two * value);

	auto one = Constants<FloatV4>::f_one;

	return (e - one) / (e + one);
}

template <UInt BLOCK_SIZE> inline constexpr Pair <UInt> DSP::SplitBlocks(UInt samples)
{
	constexpr UInt kWrap = BLOCK_SIZE - 1;

	if constexpr (BLOCK_SIZE == 8)
	{
		return { samples >> 3, samples & 7 };
	}
	else if constexpr (BLOCK_SIZE == 4)
	{
		return { samples >> 2, samples & 3 };
	}
	else if constexpr (BLOCK_SIZE > 0 && (BLOCK_SIZE & (BLOCK_SIZE - 1)) == 0)
	{
		return { samples / BLOCK_SIZE, samples & kWrap };
	}
	else
	{
		REFLEX_STATIC_ASSERT(BLOCK_SIZE == 0);	//must be power of 2

		return {};
	}
}

//
///*------------------------------------------------------------------------------------
// fast sign, returns Float
// ------------------------------------------------------------------------------------*/
//
//// returns 1.0f for positive floats, -1.0f for negative floats
//// for zero it does not work (may gives 1.0f or -1.0f), but it's faster
//inline Float fast_sign_nozero(Float f) {
//    Float r = 1.0f;
//    (int&)r |= ((int&)f & 0x80000000); // mask sign bit in f, set it in r if necessary
//    return r;
//}
//
//// returns 1.0f for positive floats, -1.0f for negative floats, 0.0f for zero
//inline Float fast_sign(Float f) {
//    if (((int&)f & 0x7FFFFFFF)==0) return 0.f; // test exponent & mantissa bits: is input zero?
//    else {
//        Float r = 1.0f;
//        (int&)r |= ((int&)f & 0x80000000); // mask sign bit in f, set it in r if necessary
//        return r;
//    }
//}
//
///*------------------------------------------------------------------------------------
// fast sign, returns int
// ------------------------------------------------------------------------------------*/
//
//// returns 1 for positive floats, -1 for negative floats
//// for 0.0f input it does not work (may give 1 or -1), but it's faster
//inline int fast_sign_int_nozero(Float f) {
//	return (signed((int&)f & 0x80000000) >> 31 ) | 1;
//}
//
//// returns 1 for positive floats, -1 for negative floats, 0 for 0.0f
//inline int fast_sign_int(Float f) {
//    if (((int&)f & 0x7FFFFFFF)==0) return 0; // test exponent & mantissa bits: is input zero?
//    return (signed((int&)f & 0x80000000) >> 31 ) | 1;
//}
