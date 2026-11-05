#pragma once

#include "[require].h"




//
//(@doc:group=Audio)

REFLEX_NS(DSP)

template <class FLOAT> struct Complex;


template <class FLOAT> Complex <FLOAT> operator+(const Complex <FLOAT> & a, const Complex <FLOAT> & b);

template <class FLOAT> Complex <FLOAT> operator-(const Complex <FLOAT> & a, const Complex <FLOAT> & b);

template <class FLOAT> Complex <FLOAT> operator*(const Complex <FLOAT> & a, const Complex <FLOAT> & b);

template <class FLOAT> Complex <FLOAT> operator/(const Complex <FLOAT> & a, const Complex <FLOAT> & b);


template <class FLOAT> Complex <FLOAT> operator+(const Complex <FLOAT> & a, const FLOAT & b);

template <class FLOAT> Complex <FLOAT> operator-(const Complex <FLOAT> & a, const FLOAT & b);

template <class FLOAT> Complex <FLOAT> operator*(const Complex <FLOAT> & a, const FLOAT & b);

template <class FLOAT> Complex <FLOAT> operator/(const Complex <FLOAT> & a, const FLOAT & b);

REFLEX_END


		

//
//complex

template <class FLOAT>
struct DSP::Complex
{
	Complex & operator+=(const Complex & value);

	Complex & operator-=(const Complex & value);

	Complex & operator*=(const Complex & value);

	Complex & operator/=(const Complex & value);

	
	Complex & operator+=(const FLOAT & value);

	Complex & operator-=(const FLOAT & value);

	Complex & operator*=(const FLOAT & value);

	Complex & operator/=(const FLOAT & value);


	FLOAT real = 0.0f;
	FLOAT imag = 0.0f;
};




//
//impl

template <class FLOAT> inline DSP::Complex <FLOAT> & DSP::Complex<FLOAT>::operator+=(const Complex & value)
{ 
	*this = (*this + value);

	return *this;
}

template <class FLOAT> inline DSP::Complex <FLOAT> & DSP::Complex<FLOAT>::operator-=(const Complex & value)
{ 
	*this = (*this - value);

	return *this;
}

template <class FLOAT> inline DSP::Complex <FLOAT> & DSP::Complex<FLOAT>::operator*=(const Complex & value)
{ 
	*this = (*this * value);

	return *this;
}

template <class FLOAT> inline DSP::Complex <FLOAT> & DSP::Complex<FLOAT>::operator/=(const Complex & value)
{ 
	*this = (*this / value);

	return *this;
}

template <class FLOAT> inline DSP::Complex <FLOAT> & DSP::Complex<FLOAT>::operator+=(const FLOAT & value)
{ 
	*this = (*this + value);

	return *this;
}

template <class FLOAT> inline DSP::Complex <FLOAT> & DSP::Complex<FLOAT>::operator-=(const FLOAT & value)
{ 
	*this = (*this - value);

	return *this;
}

template <class FLOAT> inline DSP::Complex <FLOAT> & DSP::Complex<FLOAT>::operator*=(const FLOAT & value)
{ 
	*this = (*this * value);

	return *this;
}

template <class FLOAT> inline DSP::Complex <FLOAT> & DSP::Complex<FLOAT>::operator/=(const FLOAT & value)
{ 
	*this = (*this / value);

	return *this;
}

template <class FLOAT> inline DSP::Complex <FLOAT> DSP::operator+(const Complex <FLOAT> & a, const Complex <FLOAT> & b)
{
	return { a.real + b.real, a.imag + b.imag };
}

template <class FLOAT> inline DSP::Complex <FLOAT> DSP::operator+(const Complex <FLOAT> & a, const FLOAT & b)
{
	return { a.real + b, a.imag };
}

template <class FLOAT> inline DSP::Complex <FLOAT> DSP::operator-(const Complex <FLOAT> & a, const Complex <FLOAT> & b)
{
	return { a.real - b.real, a.imag - b.imag };
}

template <class FLOAT> inline DSP::Complex <FLOAT> DSP::operator*(const Complex <FLOAT> & a, const Complex <FLOAT> & b)
{
	return { (a.real * b.real) - (a.imag * b.imag), (a.real * b.imag) + (a.imag * b.real) };
}

template <class FLOAT> inline DSP::Complex <FLOAT> DSP::operator*(const Complex <FLOAT> & a, const FLOAT & b)
{
	return { a.real * b, a.imag * b };
}

template <class FLOAT> inline DSP::Complex <FLOAT> DSP::operator/(const Complex <FLOAT> & a, const Complex <FLOAT> & b)
{
	auto denominator = b.real * b.real + b.imag * b.imag;

	auto realPart = (a.real * b.real + a.imag * b.imag) / denominator;
	auto imagPart = (a.imag * b.real - a.real * b.imag) / denominator;

	return { realPart, imagPart };
}

template <class FLOAT> inline DSP::Complex <FLOAT> DSP::operator-(const Complex <FLOAT> & a, const FLOAT & b)
{
	return { a.real - b, a.imag };
}

//template <class FLOAT> inline DSP::Complex <FLOAT> DSP::operator/(const Complex <FLOAT> & a, const FLOAT & b)
//{
//	auto t0 = (a.real * b);
//	auto t1 = (a.imag * b.real);
//	auto t3 = Square(b.a);
//	auto t4 = Square(b.b);
//
//	return { t0 * t5, (-t1) / (t3 + t4) };
//}
