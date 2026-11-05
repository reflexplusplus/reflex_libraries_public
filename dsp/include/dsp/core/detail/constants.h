#pragma once

#include "../stereo.h"




//
//declarations

REFLEX_NS(DSP::Detail)

template <class TYPE> struct Constants;

REFLEX_END




//
//constants

template <class TYPE> 
struct DSP::Detail::Constants
{
	//types

	static const bool kV4 = IsType<TYPE,FloatV4>::value;

	typedef ConditionalType <kV4,BoolV4,bool> Bool;

	typedef ConditionalType <kV4,IntV4,Int32> Int;

	typedef TYPE Float;

	typedef Stereo <Float> Stereo;



	//constants

	static const UInt kWidth = sizeof(TYPE) / sizeof(Float32);


	inline static const Float f_pi = Reflex::kPif;

	inline static const Float f_2pi = Reflex::k2Pif;


	static const Float f_minus_eight;

	static const Float f_minus_four;

	static const Float f_minus_two;

	static const Float f_minus_one;

	static const Float f_minus_rcp_six;

	static const Float f_minus_rcp_two;


	static const Float f_rcp_tenthousand;

	static const Float f_rcp_onethousand;

	static const Float f_rcp_onehundred;

	static const Float f_rcp_sixteen;

	static const Float f_rcp_twelve;

	static const Float f_rcp_eight;

	static const Float f_rcp_sevenpointfive;

	static const Float f_rcp_six;

	static const Float f_rcp_four;

	static const Float f_rcp_three;

	static inline const Float f_rcp_two = 0.5f;


	static inline const Float f_zero = 0.0f;

	static inline const Float f_one = 1.0f;

	static inline const Float f_two = 2.0f;

	static inline const Float f_four = 4.0f;

	static inline const Float f_six = 6.0f;

	static inline const Float f_eight = 8.0f;

	static inline const Float f_nine = 9.0f;

	static inline const Float f_ten = 10.0f;

	static inline const Float f_sixteen = 16.0f;

	static inline const Float f_thirty = 30.0f;

	static inline const Float f_sixtynine = 69.0f;

	static const Float f_onehundred_twenty;

	static const Float f_fourhundred_forty;

	static inline const Float f_onethousand = 1000.0f;

	static inline const Float f_tenthousand = 10000.0f;



	//special

	static const Float f_ms2mult;



	//int

	static inline const Int i_zero = 0;

	static inline const Int i_one = 1;



	//bool

	static const Bool kTrue;

	static const Bool kFalse;
};




//
//implementation

template <class TYPE> const TYPE DSP::Detail::Constants<TYPE>::f_minus_eight(-8.0f);

template <class TYPE> const TYPE DSP::Detail::Constants<TYPE>::f_minus_four(-4.0f);

template <class TYPE> const TYPE DSP::Detail::Constants<TYPE>::f_minus_two(-2.0f);

template <class TYPE> const TYPE DSP::Detail::Constants<TYPE>::f_minus_one(-1.0f);

template <class TYPE> const TYPE DSP::Detail::Constants<TYPE>::f_minus_rcp_two(1.0f / -2.0f);

template <class TYPE> const TYPE DSP::Detail::Constants<TYPE>::f_minus_rcp_six(1.0f / -6.0f);

template <class TYPE> const TYPE DSP::Detail::Constants<TYPE>::f_rcp_tenthousand(1.0f / 10000.0f);

template <class TYPE> const TYPE DSP::Detail::Constants<TYPE>::f_rcp_onethousand(1.0f / 1000.0f);

template <class TYPE> const TYPE DSP::Detail::Constants<TYPE>::f_rcp_onehundred(1.0f / 100.0f);

template <class TYPE> const TYPE DSP::Detail::Constants<TYPE>::f_rcp_sixteen(1.0f / 16.0f);

template <class TYPE> const TYPE DSP::Detail::Constants<TYPE>::f_rcp_twelve(1.0f / 12.0f);

template <class TYPE> const TYPE DSP::Detail::Constants<TYPE>::f_rcp_eight(1.0f / 8.0f);

template <class TYPE> const TYPE DSP::Detail::Constants<TYPE>::f_rcp_sevenpointfive(1.0f / 7.5f);

template <class TYPE> const TYPE DSP::Detail::Constants<TYPE>::f_rcp_six(1.0f / 6.0f);

template <class TYPE> const TYPE DSP::Detail::Constants<TYPE>::f_rcp_four(0.25f);

template <class TYPE> const TYPE DSP::Detail::Constants<TYPE>::f_rcp_three(1.0f / 3.0f);

template <class TYPE> const TYPE DSP::Detail::Constants<TYPE>::f_onehundred_twenty(120.0f);

template <class TYPE> const TYPE DSP::Detail::Constants<TYPE>::f_fourhundred_forty(440.0f);

REFLEX_NS(DSP::Detail)

template <class> struct BooleanValues;

template <> struct BooleanValues <FloatV4>
{
	inline static const BoolV4 kTrue = SIMD::kBooleanTrue;
	inline static const BoolV4 kFalse= SIMD::kBooleanFalse;
};

template <> struct BooleanValues <Float32>
{
	static const bool kTrue = true;
	static const bool kFalse = false;
};

REFLEX_END

template <class TYPE> const typename DSP::Detail::Constants<TYPE>::Bool DSP::Detail::Constants<TYPE>::kTrue = DSP::Detail::BooleanValues<TYPE>::kTrue;

template <class TYPE> const typename DSP::Detail::Constants<TYPE>::Bool DSP::Detail::Constants<TYPE>::kFalse = DSP::Detail::BooleanValues<TYPE>::kFalse;