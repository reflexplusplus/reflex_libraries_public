#pragma once

#include "../[require].h"




//
//(@doc:group=Interpolation)

namespace DSP
{

	//single
	
	Float Interpolate2P_Clip(const Float * data, UInt length, Float & readpos);		//NOTE data[length - 1] must be repeated on data[length]

	Float Interpolate2P_Wrap(const Float * data, UInt length, Float & readpos);		//NOTE data[0] must be repeated on data[length]


	Float Interpolate4P_RT(Float x, Float t_1, Float t0, Float t1, Float t2);



	//vector
	
	FloatV4 Interpolate4P_RT(const FloatV4 & x, const FloatV4 & t_1, const FloatV4 & t0, const FloatV4 & t1, const FloatV4 & t2);

	FloatV4 Interpolate4P_RT(const FloatV4 & x, const FloatV4 * t_1);

}


	
	
//
//

REFLEX_INLINE Float DSP::Interpolate2P_Clip(const Float * data, UInt length, Float & readpos)
{
	REFLEX_ASSERT(readpos >= 0.0f);

	REFLEX_ASSERT(data[length - 1] == data[length]);

	UInt posi = Truncate(readpos);
	
	Float posf = Float(posi);
	
	Float x = readpos - posf;

	readpos = posf + x;

	posi = Clip(posi, UInt(0), length - 1);

	return LinearInterpolate(x, data[posi], data[posi + 1]);
}

REFLEX_INLINE Float DSP::Interpolate2P_Wrap(const Float * data, UInt length, Float & readpos)
{
	REFLEX_ASSERT(data[0] == data[length]);

	//TODO surely this should use Int and Modulo (not UINt posi and %)

	UInt posi = Truncate(readpos);

	Float x = readpos - Float(posi);

	UInt posi_wrp = posi % length;

	Float posf_wrp = Float(posi_wrp);

	readpos = posf_wrp + x;

	const Float * ptr = data + posi_wrp;
	
	Float a = *ptr;

	Float b = *++ptr;

	return LinearInterpolate(x, a, b);
}




//
//interpolate4p

REFLEX_INLINE Float DSP::Interpolate4P_RT(Float x, Float t_1, Float t0, Float t1, Float t2)
{
	Float a = (t1 - t_1) * 0.5f;
	Float b = (t2 - t0) * 0.5f;
	Float c = t0 - t1;
	Float d = a + c;
	Float e = b + d;
	Float f = e + c;
	Float g = x * f;
	Float h = d + f;
	Float i = g - h;
	Float j = x * i;
	Float k = a + j;
	Float l = x * k;

	return t0 + l;
}

REFLEX_INLINE FloatV4 DSP::Interpolate4P_RT(const FloatV4 & x, const FloatV4 & t_1, const FloatV4 & t0, const FloatV4 & t1, const FloatV4 & t2)
{
	typedef Detail::Constants <FloatV4> Constants;

	FloatV4 a = (t1 - t_1) * Constants::f_rcp_two;
	FloatV4 b = (t2 - t0) * Constants::f_rcp_two;
	FloatV4 c = t0 - t1;
	FloatV4 d = a + c;
	FloatV4 e = b + d;
	FloatV4 f = e + c;
	FloatV4 g = x * f;
	FloatV4 h = d + f;
	FloatV4 i = g - h;
	FloatV4 j = x * i;
	FloatV4 k = a + j;
	FloatV4 l = x * k;

	return t0 + l;
}

REFLEX_INLINE FloatV4 DSP::Interpolate4P_RT(const FloatV4 & x, const FloatV4 * t_1)
{
	return Interpolate4P_RT(x, t_1[0], t_1[1], t_1[2], t_1[3]);
}

//REFLEX_INLINE FloatV4 DSP::Interpolate4P_Lagrange(const FloatV4 & x, const FloatV4 & t_1, const FloatV4 & t0, const FloatV4 & t1, const FloatV4 & t2)
//{
//	typedef Constants <true> Constants;
//
//	FloatV4 x1 = x + kfOne;
//	FloatV4 x_1 = x - kfOne;
//	FloatV4 x_2 = x - Constants::f_two;
//
//	FloatV4 temp_1 = t_1 * ((x * x_1) * x_2);
//	temp_1 *= Constants::f_minus_rcp_six;
//	
//	FloatV4 temp0 = t0 * ((x_1 * x1) * x_2);
//	temp0 *= Constants::f_rcp_two;
//
//	FloatV4 x1x = x1 * x;
//
//	FloatV4 temp1 = t1 * (x1x * x_2);
//	temp1 *= Constants::f_minus_rcp_two;
//
//	FloatV4 temp2 = t2 * (x1x * x_1);
//	temp2 *= Constants::f_rcp_six;
//
//	return temp_1 + temp0 + temp1 + temp2;
//}
