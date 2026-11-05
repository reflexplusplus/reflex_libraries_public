#pragma once

#include "../../[require].h"

//TODO clean up and move into DSP library





//
//decl

Float32x4 cos_ps(Float32x4 x);

void sincos_ps(Float32x4 x, Float32x4 *s, Float32x4 *c);

inline void SinCos(const FloatV4 & in, FloatV4 & sin, FloatV4 & cos)
{
	//cos = SIMD::FloatV4(Cos(in[0]), Cos(in[1]), Cos(in[2]), Cos(in[3]));

	//sin = SIMD::FloatV4(Sin(in[0]), Sin(in[1]), Sin(in[2]), Sin(in[3]));

	sincos_ps(in, &Reinterpret<Float32x4>(sin), &Reinterpret<Float32x4>(cos));
}





//
//impl

#define set1_ps_hex(x) Reinterpret<Float32x4>(REFLEX_SET1_I32_X4(x))

inline const Float32x4 _ps_1 = REFLEX_SET1_F32_X4(1.0f);
inline const Float32x4 _ps_0p5 = REFLEX_SET1_F32_X4(0.5f);
inline const Float32x4 _ps_min_norm_pos = set1_ps_hex(0x00800000);
inline const Float32x4 _ps_mant_mask = set1_ps_hex(0x7f800000);
inline const Float32x4 _ps_inv_mant_mask = set1_ps_hex(~0x7f800000);
inline const Float32x4 _ps_sign_mask = set1_ps_hex(0x80000000);
inline const Float32x4 _ps_inv_sign_mask = set1_ps_hex(~0x80000000);

inline const Int32x4 _pi32_1 = REFLEX_SET1_I32_X4(1);
inline const Int32x4 _pi32_2 = REFLEX_SET1_I32_X4(2);
inline const Int32x4 _pi32_4 = REFLEX_SET1_I32_X4(4);

inline const Int32x4 _pi32_inv1 = REFLEX_SET1_I32_X4(~1);

inline const Float32x4 _ps_minus_cephes_DP1 = REFLEX_SET1_F32_X4(-0.78515625f);
inline const Float32x4 _ps_minus_cephes_DP2 = REFLEX_SET1_F32_X4(-2.4187564849853515625e-4f);
inline const Float32x4 _ps_minus_cephes_DP3 = REFLEX_SET1_F32_X4(-3.77489497744594108e-8f);

inline const Float32x4 _ps_sincof_p0 = REFLEX_SET1_F32_X4(-1.9515295891E-4f);
inline const Float32x4 _ps_sincof_p1 = REFLEX_SET1_F32_X4( 8.3321608736E-3f);
inline const Float32x4 _ps_sincof_p2 = REFLEX_SET1_F32_X4(-1.6666654611E-1f);
inline const Float32x4 _ps_coscof_p0 = REFLEX_SET1_F32_X4( 2.443315711809948E-005f);
inline const Float32x4 _ps_coscof_p1 = REFLEX_SET1_F32_X4(-1.388731625493765E-003f);
inline const Float32x4 _ps_coscof_p2 = REFLEX_SET1_F32_X4( 4.166664568298827E-002f);
inline const Float32x4 _ps_cephes_FOPI = REFLEX_SET1_F32_X4(1.27323954473516f); // 4 / M_PI

inline const Float32x4 _ps_atanrange_hi = REFLEX_SET1_F32_X4(2.414213562373095f );
inline const Float32x4 _ps_atanrange_lo = REFLEX_SET1_F32_X4( 0.4142135623730950f );

inline const Float32x4 _ps_atancof_p0= REFLEX_SET1_F32_X4( 8.05374449538e-2f );
inline const Float32x4 _ps_atancof_p1= REFLEX_SET1_F32_X4( 1.38776856032E-1f );
inline const Float32x4 _ps_atancof_p2= REFLEX_SET1_F32_X4( 1.99777106478E-1f );
inline const Float32x4 _ps_atancof_p3= REFLEX_SET1_F32_X4( 3.33329491539E-1f );

inline const Float32x4 _ps_cephes_PIF = REFLEX_SET1_F32_X4( 3.141592653589793238f );
inline const Float32x4 _ps_cephes_PIO2F = REFLEX_SET1_F32_X4( 1.5707963267948966192f );
inline const Float32x4 _ps_cephes_PIO4F  = REFLEX_SET1_F32_X4( 0.7853981633974483096f );

REFLEX_INLINE Float32x4 sin_ps(Float32x4 x)
{
	#if 0 // Currently expect user to round manually if he knows input can be unbounded.
	#ifdef MATH_SSE41 // _mm_round_ps is SSE4.1
	// XXX Added in MathGeoLib: Take a modulo of the input in 2pi to try to enhance the precision with large input values.
	x = modf_ps(x, REFLEX_SET1_F32_X4(2.f*pi));
	#endif
	#endif

	/* extract the sign bit (upper one) */
	auto sign_bit = REFLEX_AND_F32_X4(x, _ps_sign_mask);
	/* take the absolute value */
	x = REFLEX_XOR_F32_X4(x, sign_bit);

	/* scale by 4/Pi */
	auto y = REFLEX_MUL_F32_X4(x, _ps_cephes_FOPI);

	/* store the integer part of y in mm0 */
	auto emm2 = REFLEX_TRUNCATE_F32_X4(y);
	emm2 = REFLEX_ADD_I32_X4(emm2, _pi32_1);
	emm2 = REFLEX_AND_I32_X4(emm2, _pi32_inv1);
	y = REFLEX_I32_X4_TO_F32_X4(emm2);

	/* get the swap sign flag */
	auto emm0 = REFLEX_SHL1_I32_X4(REFLEX_AND_I32_X4(emm2, _pi32_4), 29);
	/* get the polynom selection mask 
		there is one polynom for 0 <= x <= Pi/4
		and another one for Pi/4<x<=Pi/2
		Both branches will be computed.
	*/
	emm2 = REFLEX_AND_I32_X4(emm2, _pi32_2);
	emm2 = REFLEX_EQ_I32_X4(emm2, REFLEX_ZERO_I32_X4());
  
	const auto & swap_sign_bit = Reinterpret<Float32x4>(emm0);
	const auto & poly_mask = Reinterpret<Float32x4>(emm2);
	sign_bit = REFLEX_XOR_F32_X4(sign_bit, swap_sign_bit);
  
	/* The magic pass: "Extended precision modular arithmetic" 
		x = ((x - y * DP1) - y * DP2) - y * DP3; */
	auto xmm1 = REFLEX_MUL_F32_X4(y, _ps_minus_cephes_DP1);
	auto xmm2 = REFLEX_MUL_F32_X4(y, _ps_minus_cephes_DP2);
	auto xmm3 = REFLEX_MUL_F32_X4(y, _ps_minus_cephes_DP3);
	x = REFLEX_ADD_F32_X4(REFLEX_ADD_F32_X4(x, xmm1), REFLEX_ADD_F32_X4(xmm2, xmm3));

	/* Evaluate the first polynom  (0 <= x <= Pi/4) */
	y = _ps_coscof_p0;
	auto z = REFLEX_MUL_F32_X4(x,x);

	y = REFLEX_MUL_F32_X4(y, z);
	y = REFLEX_ADD_F32_X4(y, _ps_coscof_p1);
	y = REFLEX_MUL_F32_X4(y, z);
	y = REFLEX_ADD_F32_X4(y, _ps_coscof_p2);
	y = REFLEX_MUL_F32_X4(y, REFLEX_MUL_F32_X4(z, z));
	auto tmp = REFLEX_MUL_F32_X4(z, _ps_0p5);
	y = REFLEX_SUB_F32_X4(y, tmp);
	y = REFLEX_ADD_F32_X4(y, _ps_1);
  
	/* Evaluate the second polynom  (Pi/4 <= x <= 0) */

	auto y2 = _ps_sincof_p0;
	y2 = REFLEX_MUL_F32_X4(y2, z);
	y2 = REFLEX_ADD_F32_X4(y2, _ps_sincof_p1);
	y2 = REFLEX_MUL_F32_X4(y2, z);
	y2 = REFLEX_ADD_F32_X4(y2, _ps_sincof_p2);
	y2 = REFLEX_MUL_F32_X4(y2, REFLEX_MUL_F32_X4(z, x));
	y2 = REFLEX_ADD_F32_X4(y2, x);

	/* select the correct result from the two polynoms */  
	xmm3 = poly_mask;
	y2 = REFLEX_AND_F32_X4(xmm3, y2); //, xmm3);
	y = REFLEX_ANDNOT_F32_X4(xmm3, y);
	y = REFLEX_ADD_F32_X4(y,y2);
	/* update the sign */
	y = REFLEX_XOR_F32_X4(y, sign_bit);

	return y;
}

REFLEX_INLINE Float32x4 cos_ps(Float32x4 x)
{
	#if 0 // Currently expect user to round manually if he knows input can be unbounded.
	#ifdef MATH_SSE41 // _mm_round_ps is SSE4.1
	// XXX Added in MathGeoLib: Take a modulo of the input in 2pi to try to enhance the precision with large input values.
	x = modf_ps(x, REFLEX_SET1_F32_X4(2.f*pi));
	#endif
	#endif

	/* take the absolute value */
	x = REFLEX_AND_F32_X4(x, _ps_inv_sign_mask);
	/* scale by 4/Pi */
	auto y = REFLEX_MUL_F32_X4(x, _ps_cephes_FOPI);
  
	/* store the integer part of y in mm0 */
	auto emm2 = REFLEX_TRUNCATE_F32_X4(y);
	emm2 = REFLEX_ADD_I32_X4(emm2, _pi32_1);
	emm2 = REFLEX_AND_I32_X4(emm2, _pi32_inv1);
	y = REFLEX_I32_X4_TO_F32_X4(emm2);

	emm2 = REFLEX_SUB_I32_X4(emm2, _pi32_2);
  
	/* get the swap sign flag */
	auto emm0 = REFLEX_ANDNOT_I32_X4(emm2, _pi32_4);
	emm0 = REFLEX_SHL1_I32_X4(emm0, 29);
	/* get the polynom selection mask */
	emm2 = REFLEX_AND_I32_X4(emm2, _pi32_2);
	emm2 = REFLEX_EQ_I32_X4(emm2, REFLEX_ZERO_I32_X4());
  
	const auto & sign_bit = Reinterpret<Float32x4>(emm0);
	const auto & poly_mask = Reinterpret<Float32x4>(emm2);
	/* magic pass: "Extended precision modular arithmetic" 
		x = ((x - y * DP1) - y * DP2) - y * DP3; */
	auto xmm1 = REFLEX_MUL_F32_X4(y, _ps_minus_cephes_DP1);
	auto xmm2 = REFLEX_MUL_F32_X4(y, _ps_minus_cephes_DP2);
	auto xmm3 = REFLEX_MUL_F32_X4(y, _ps_minus_cephes_DP3);
	x = REFLEX_ADD_F32_X4(REFLEX_ADD_F32_X4(x, xmm1), REFLEX_ADD_F32_X4(xmm2, xmm3));
  
	/* Evaluate the first polynom  (0 <= x <= Pi/4) */
	y = _ps_coscof_p0;
	auto z = REFLEX_MUL_F32_X4(x,x);

	y = REFLEX_MUL_F32_X4(y, z);
	y = REFLEX_ADD_F32_X4(y, _ps_coscof_p1);
	y = REFLEX_MUL_F32_X4(y, z);
	y = REFLEX_ADD_F32_X4(y, _ps_coscof_p2);
	y = REFLEX_MUL_F32_X4(y, REFLEX_MUL_F32_X4(z, z));
	auto tmp = REFLEX_MUL_F32_X4(z, _ps_0p5);
	y = REFLEX_SUB_F32_X4(y, tmp);
	y = REFLEX_ADD_F32_X4(y, _ps_1);
  
	/* Evaluate the second polynom  (Pi/4 <= x <= 0) */

	auto y2 = _ps_sincof_p0;
	y2 = REFLEX_MUL_F32_X4(y2, z);
	y2 = REFLEX_ADD_F32_X4(y2, _ps_sincof_p1);
	y2 = REFLEX_MUL_F32_X4(y2, z);
	y2 = REFLEX_ADD_F32_X4(y2, _ps_sincof_p2);
	y2 = REFLEX_MUL_F32_X4(y2, REFLEX_MUL_F32_X4(z, x));
	y2 = REFLEX_ADD_F32_X4(y2, x);

	/* select the correct result from the two polynoms */  
	xmm3 = poly_mask;
	y2 = REFLEX_AND_F32_X4(xmm3, y2); //, xmm3);
	y = REFLEX_ANDNOT_F32_X4(xmm3, y);
	y = REFLEX_ADD_F32_X4(y,y2);
	/* update the sign */
	y = REFLEX_XOR_F32_X4(y, sign_bit);

	return y;
}

REFLEX_INLINE void sincos_ps(Float32x4 x, Float32x4 * s, Float32x4 * c)
{
	#if 0
	#ifdef MATH_SSE41 // _mm_round_ps is SSE4.1
	// XXX Added in MathGeoLib: Take a modulo of the input in 2pi to try to enhance the precision with large input values.
	x = modf_ps(x, REFLEX_SET1_F32_X4(2.f*3.141592654f));
	#endif
	#endif

	/* extract the sign bit (upper one) */
	auto sign_bit_sin = REFLEX_AND_F32_X4(x, _ps_sign_mask);
	/* take the absolute value */
	x = REFLEX_XOR_F32_X4(x, sign_bit_sin);
  
	/* scale by 4/Pi */
	auto y = REFLEX_MUL_F32_X4(x, _ps_cephes_FOPI);
    
	/* store the integer part of y in emm2 */
	auto emm2 = REFLEX_TRUNCATE_F32_X4(y);

	/* j=(j+1) & (~1) (see the cephes sources) */
	emm2 = REFLEX_ADD_I32_X4(emm2, _pi32_1);
	emm2 = REFLEX_AND_I32_X4(emm2, _pi32_inv1);
	y = REFLEX_I32_X4_TO_F32_X4(emm2);

	auto emm4 = emm2;

	/* get the swap sign flag for the sine */
	auto emm0 = REFLEX_AND_I32_X4(emm2, _pi32_4);
	emm0 = REFLEX_SHL1_I32_X4(emm0, 29);
	const auto & swap_sign_bit_sin = Reinterpret<Float32x4>(emm0);

	/* get the polynom selection mask for the sine*/
	emm2 = REFLEX_AND_I32_X4(emm2, _pi32_2);
	emm2 = REFLEX_EQ_I32_X4(emm2, REFLEX_ZERO_I32_X4());
	const auto & poly_mask = Reinterpret<Float32x4>(emm2);
	/* magic pass: "Extended precision modular arithmetic" 
		x = ((x - y * DP1) - y * DP2) - y * DP3; */
	auto xmm1 = REFLEX_MUL_F32_X4(y, _ps_minus_cephes_DP1);
	auto xmm2 = REFLEX_MUL_F32_X4(y, _ps_minus_cephes_DP2);
	auto xmm3 = REFLEX_MUL_F32_X4(y, _ps_minus_cephes_DP3);
	x = REFLEX_ADD_F32_X4(REFLEX_ADD_F32_X4(x, xmm1), REFLEX_ADD_F32_X4(xmm2, xmm3));

	emm4 = REFLEX_SUB_I32_X4(emm4, _pi32_2);
	emm4 = REFLEX_ANDNOT_I32_X4(emm4, _pi32_4);
	emm4 = REFLEX_SHL1_I32_X4(emm4, 29);
	const auto & sign_bit_cos = Reinterpret<Float32x4>(emm4);

	sign_bit_sin = REFLEX_XOR_F32_X4(sign_bit_sin, swap_sign_bit_sin);
  
	/* Evaluate the first polynom  (0 <= x <= Pi/4) */
	auto z = REFLEX_MUL_F32_X4(x,x);
	y = _ps_coscof_p0;

	y = REFLEX_MUL_F32_X4(y, z);
	y = REFLEX_ADD_F32_X4(y, _ps_coscof_p1);
	y = REFLEX_MUL_F32_X4(y, z);
	y = REFLEX_ADD_F32_X4(y, _ps_coscof_p2);
	y = REFLEX_MUL_F32_X4(y, REFLEX_MUL_F32_X4(z, z));
	auto tmp = REFLEX_MUL_F32_X4(z, _ps_0p5);
	y = REFLEX_SUB_F32_X4(y, tmp);
	y = REFLEX_ADD_F32_X4(y, _ps_1);
  
	/* Evaluate the second polynom  (Pi/4 <= x <= 0) */

	auto y2 = _ps_sincof_p0;
	y2 = REFLEX_MUL_F32_X4(y2, z);
	y2 = REFLEX_ADD_F32_X4(y2, _ps_sincof_p1);
	y2 = REFLEX_MUL_F32_X4(y2, z);
	y2 = REFLEX_ADD_F32_X4(y2, _ps_sincof_p2);
	y2 = REFLEX_MUL_F32_X4(y2, REFLEX_MUL_F32_X4(z, x));
	y2 = REFLEX_ADD_F32_X4(y2, x);

	/* select the correct result from the two polynoms */  
	xmm3 = poly_mask;
	auto ysin2 = REFLEX_AND_F32_X4(xmm3, y2);
	auto ysin1 = REFLEX_ANDNOT_F32_X4(xmm3, y);
	y2 = REFLEX_SUB_F32_X4(y2,ysin2);
	y = REFLEX_SUB_F32_X4(y, ysin1);

	xmm1 = REFLEX_ADD_F32_X4(ysin1,ysin2);
	xmm2 = REFLEX_ADD_F32_X4(y,y2);
 
	/* update the sign */
	*s = REFLEX_XOR_F32_X4(xmm1, sign_bit_sin);
	*c = REFLEX_XOR_F32_X4(xmm2, sign_bit_cos);
}

REFLEX_INLINE Float32x4 atan_ps(Float32x4 x)
{
	Float32x4 sign_bit, y;

	sign_bit = x;
	/* take the absolute value */
	x = REFLEX_AND_F32_X4( x, _ps_inv_sign_mask );
	/* extract the sign bit (upper one) */
	sign_bit = REFLEX_AND_F32_X4( sign_bit, _ps_sign_mask );

/* range reduction, init x and y depending on range */
	/* x > 2.414213562373095 */
	auto cmp0 = REFLEX_GT_F32_X4(x, _ps_atanrange_hi);
	/* x > 0.4142135623730950 */
	auto cmp1 = Reinterpret<Float32x4>(REFLEX_GT_F32_X4(x, _ps_atanrange_lo));

	/* x > 0.4142135623730950 && !( x > 2.414213562373095 ) */
	auto cmp2 = REFLEX_ANDNOT_F32_X4(cmp0, cmp1);

	/* -( 1.0/x ) */
	auto y0 = REFLEX_AND_F32_X4( cmp0, _ps_cephes_PIO2F );
	auto x0 = REFLEX_DIV_F32_X4( SIMD::kfOne, x );
	x0 = REFLEX_XOR_F32_X4( x0, _ps_sign_mask );

	auto y1 = REFLEX_AND_F32_X4( cmp2, _ps_cephes_PIO4F );
	/* (x-1.0)/(x+1.0) */
	auto x1_o = REFLEX_SUB_F32_X4( x, _ps_1 );
	auto x1_u = REFLEX_ADD_F32_X4( x, _ps_1 );
	auto x1 = REFLEX_DIV_F32_X4( x1_o, x1_u );

	auto x2 = REFLEX_AND_F32_X4( cmp2, x1 );
	x0 = REFLEX_AND_F32_X4( cmp0, x0 );
	x2 = REFLEX_OR_F32_X4( x2, x0 );
	cmp1 = REFLEX_OR_F32_X4( cmp0, cmp2 );
	x2 = REFLEX_AND_F32_X4( cmp1, x2 );
	x = REFLEX_ANDNOT_F32_X4( cmp1, x );
	x = REFLEX_OR_F32_X4( x2, x );

	y = REFLEX_OR_F32_X4( y0, y1 );

	auto zz = REFLEX_MUL_F32_X4( x, x );
	auto acc = _ps_atancof_p0;
	acc = REFLEX_MUL_F32_X4( acc, zz );
	acc = REFLEX_SUB_F32_X4( acc, _ps_atancof_p1 );
	acc = REFLEX_MUL_F32_X4( acc, zz );
	acc = REFLEX_ADD_F32_X4( acc, _ps_atancof_p2 );
	acc = REFLEX_MUL_F32_X4( acc, zz );
	acc = REFLEX_SUB_F32_X4( acc, _ps_atancof_p3 );
	acc = REFLEX_MUL_F32_X4( acc, zz );
	acc = REFLEX_MUL_F32_X4( acc, x );
	acc = REFLEX_ADD_F32_X4( acc, x );
	y = REFLEX_ADD_F32_X4( y, acc );

	/* update the sign */
	y = REFLEX_XOR_F32_X4( y, sign_bit );

	return y;
}

REFLEX_INLINE FloatV4 atan2_ps(const FloatV4 & y, const FloatV4 & x)
{
	//return { atan2(y[0], x[0]), atan2(y[1], x[1]), atan2(y[2], x[2]), atan2(y[3], x[3]) };

	auto x_eq_0 = REFLEX_EQ_F32_X4( x, SIMD::kfZero );
	auto x_gt_0 = REFLEX_GT_F32_X4( x, SIMD::kfZero  );
	auto x_le_0 = REFLEX_LTEQ_F32_X4( x, SIMD::kfZero  );
	auto y_eq_0 = REFLEX_EQ_F32_X4( y, SIMD::kfZero  );
	auto x_lt_0 = REFLEX_LT_F32_X4( x, SIMD::kfZero );
	auto y_lt_0 = REFLEX_LT_F32_X4( y, SIMD::kfZero );

	auto zero_mask = REFLEX_AND_F32_X4( x_eq_0, y_eq_0 );
	auto zero_mask_other_case = REFLEX_AND_F32_X4( y_eq_0, x_gt_0 );
	zero_mask = REFLEX_OR_F32_X4( zero_mask, zero_mask_other_case );

	auto pio2_mask = REFLEX_ANDNOT_F32_X4( y_eq_0, x_eq_0 );
	auto pio2_mask_sign = REFLEX_AND_F32_X4( y_lt_0, _ps_sign_mask );
	auto pio2_result = _ps_cephes_PIO2F;
	pio2_result = REFLEX_XOR_F32_X4( pio2_result, pio2_mask_sign );
	pio2_result = REFLEX_AND_F32_X4( pio2_mask, pio2_result );

	auto pi_mask = REFLEX_AND_F32_X4( y_eq_0, x_le_0 );
	auto pi = _ps_cephes_PIF;
	auto pi_result = REFLEX_AND_F32_X4( pi_mask, pi );

	auto swap_sign_mask_offset = REFLEX_AND_F32_X4( x_lt_0, y_lt_0 );
	swap_sign_mask_offset = REFLEX_AND_F32_X4( swap_sign_mask_offset, _ps_sign_mask );

	auto offset0 = REFLEX_ZERO_F32_X4();
	auto offset1 = _ps_cephes_PIF;
	offset1 = REFLEX_XOR_F32_X4( offset1, swap_sign_mask_offset );

	auto offset = REFLEX_ANDNOT_F32_X4( x_lt_0, offset0 );
	offset = REFLEX_AND_F32_X4( x_lt_0, offset1 );

	auto arg = REFLEX_DIV_F32_X4( y, x );
	auto atan_result = atan_ps( arg );
	atan_result = REFLEX_ADD_F32_X4( atan_result, offset );

	/* select between zero_result, pio2_result and atan_result */

	auto result = REFLEX_ANDNOT_F32_X4( zero_mask, pio2_result );
	atan_result = REFLEX_ANDNOT_F32_X4( pio2_mask, atan_result );
	//atan_result = REFLEX_ANDNOT_F32_X4( pio2_mask, atan_result);
	result = REFLEX_OR_F32_X4( result, atan_result );
	result = REFLEX_OR_F32_X4( result, pi_result );

	return SIMD::SelectNot(Reinterpret<BoolV4>(zero_mask), Reinterpret<FloatV4>(result));
}

inline const Float32x4 kHalfPi = REFLEX_SET1_F32_X4(Float(kPi / 2.0));
inline const Float32x4 kQuarterPi = REFLEX_SET1_F32_X4(Float(kPi / 4.0));
inline const Float32x4 kSignMask = Reinterpret<Float32x4>(REFLEX_SET1_I32_X4(0x80000000));
inline const Float32x4 kPiV4 = REFLEX_SET1_F32_X4(kPif);
inline const Float32x4 blah1 = REFLEX_SET1_F32_X4(0.2447f);
inline const Float32x4 blah2 = REFLEX_SET1_F32_X4(0.0663f);

REFLEX_INLINE Float32x4 atan_ps_fast(Float32x4 x)
{
	//                                      quarterpi*x
	//                                      - x*(fabs(x) - 1)
	//                                      *(0.2447f+0.0663f*fabs(x));

	return REFLEX_SUB_F32_X4(REFLEX_MUL_F32_X4(kQuarterPi, x), REFLEX_MUL_F32_X4(REFLEX_MUL_F32_X4(x, REFLEX_SUB_F32_X4(REFLEX_ANDNOT_F32_X4(kSignMask, x), SIMD::kfOne)), (REFLEX_ADD_F32_X4(blah1, REFLEX_MUL_F32_X4(blah2, REFLEX_ANDNOT_F32_X4(kSignMask, x))))));
}

REFLEX_INLINE FloatV4 atan2_ps_fast(const FloatV4 & y, const FloatV4 & x)
{
	auto absxgreaterthanabsy = REFLEX_GT_F32_X4(REFLEX_ANDNOT_F32_X4(kSignMask, x), REFLEX_ANDNOT_F32_X4(kSignMask, y));
	Float32x4 ratio = REFLEX_DIV_F32_X4(REFLEX_ADD_F32_X4(REFLEX_AND_F32_X4(absxgreaterthanabsy, y), REFLEX_ANDNOT_F32_X4(absxgreaterthanabsy, x)),
		REFLEX_ADD_F32_X4(REFLEX_AND_F32_X4(absxgreaterthanabsy, x), REFLEX_ANDNOT_F32_X4(absxgreaterthanabsy, y)));
	
	Float32x4 atan = atan_ps_fast(ratio);

	auto xgreaterthan0 = REFLEX_GT_F32_X4(x, SIMD::kfZero);
	auto ygreaterthan0 = REFLEX_GT_F32_X4(y, SIMD::kfZero);

	atan = REFLEX_XOR_F32_X4(atan, REFLEX_ANDNOT_F32_X4(absxgreaterthanabsy, REFLEX_AND_F32_X4(xgreaterthan0, kSignMask))); //negate atan if absx<=absy & x>0

	Float32x4 shift = kPiV4;
	shift = REFLEX_SUB_F32_X4(shift, REFLEX_ANDNOT_F32_X4(absxgreaterthanabsy, kHalfPi)); //substract halfpi if absx<=absy
	shift = REFLEX_XOR_F32_X4(shift, REFLEX_ANDNOT_F32_X4(ygreaterthan0, kSignMask)); //negate shift if y<=0
	shift = REFLEX_ANDNOT_F32_X4(REFLEX_AND_F32_X4(absxgreaterthanabsy, xgreaterthan0), shift); //null if abs>absy & x>0

	auto output = REFLEX_ADD_F32_X4(atan, shift);
	
	return SIMD::SelectNot(SIMD::Or(y == SIMD::kfZero, x == SIMD::kfZero), Reinterpret<FloatV4>(output));
}
