#pragma once

#include "[require].h"




//
//declarations

REFLEX_NS(DSP)

template <class TYPE> class Stereo;

typedef Stereo <FloatV4> StereoV4;


template <class TYPE> Stereo <TYPE> operator+(const Stereo <TYPE> & a, const Stereo <TYPE> & b);

template <class TYPE> Stereo <TYPE> operator-(const Stereo <TYPE> & a, const Stereo <TYPE> & b);

template <class TYPE> Stereo <TYPE> operator*(const Stereo <TYPE> & a, const Stereo <TYPE> & b);

template <class TYPE> Stereo <TYPE> operator/(const Stereo <TYPE> & a, const Stereo <TYPE> & b);


template <class TYPE> Stereo <TYPE> operator+(const Stereo <TYPE> & a, const TYPE & b);

template <class TYPE> Stereo <TYPE> operator-(const Stereo <TYPE> & a, const TYPE & b);

template <class TYPE> Stereo <TYPE> operator*(const Stereo <TYPE> & a, const TYPE & b);

template <class TYPE> Stereo <TYPE> operator/(const Stereo <TYPE> & a, const TYPE & b);


template <class TYPE> Stereo <TYPE> Abs(const Stereo <TYPE> & a);

template <class TYPE> Stereo <TYPE> Max(const Stereo <TYPE> & a, const Stereo <TYPE> & b);

template <class TYPE> Stereo <TYPE> Min(const Stereo <TYPE> & a, const Stereo <TYPE> & b);


template <class TYPE> TYPE Select(bool test, const TYPE & t, const TYPE & f)
{
	return test ? t : f;
}

template <class TYPE> TYPE Select(bool test, const TYPE & t)
{
	return test ? t : TYPE();
}

template <class TYPE> TYPE Select(bool test, const SIMD::TypeV4 <TYPE> & t, const SIMD::TypeV4 <TYPE> & f)
{
	return SIMD::Select(test, t, f);
}

template <class TYPE> TYPE Select(bool test, const SIMD::TypeV4 <TYPE> & t)
{
	return SIMD::Select(test, t);
}

template <class TYPE> Stereo < SIMD::TypeV4 <TYPE> > Select(const SIMD::BoolV4 & test, const Stereo < SIMD::TypeV4 <TYPE> > & t)
{
	return { Select(test, t.l), Select(test, t.r) };
}

template <class TYPE> Stereo < SIMD::TypeV4 <TYPE> > Select(const SIMD::BoolV4 & test, const Stereo < SIMD::TypeV4 <TYPE> > & t, const Stereo < SIMD::TypeV4 <TYPE> > & f)
{ 
	return { Select(test, t.l, f.l), Select(test, t.r, f.r) };
}

template <class TYPE> Stereo <TYPE> Select(bool test, const Stereo <TYPE> & t)
{
	return { Select(test, t.l), Select(test, t.r) };
}

template <class TYPE> Stereo <TYPE> Select(const SIMD::BoolV4 & test, const Stereo <TYPE> & t, const Stereo <TYPE> & f)
{
	return { Select(test, t.l, f.l), Select(test, t.r, f.r) };
}

template <class TYPE> TYPE Abs(const TYPE & a) { return Reflex::Abs(a); }

template <class TYPE> TYPE Max(const TYPE & a, const TYPE & b) { return Reflex::Max(a, b); }

template <class TYPE> TYPE Min(const TYPE & a, const TYPE & b) { return Reflex::Min(a, b); }

REFLEX_END


		

//
//Stereo

template <class TYPE>
class DSP::Stereo
{
public:

	Stereo();

	Stereo(const TYPE & value);

	Stereo(const TYPE & l, const TYPE & r);

	Stereo(const Stereo & value) = default;


	template <UInt CHANNEL> TYPE & GetChannel();

	template <UInt CHANNEL> const TYPE & GetChannel() const;


	Stereo & Set(const TYPE & l, const TYPE & r);

	Stereo & Set(const TYPE & value);


	TYPE & operator[](UInt idx);

	const TYPE & operator[](UInt idx) const;


	Stereo & operator+=(const Stereo & value);

	Stereo & operator-=(const Stereo & value);

	Stereo & operator*=(const Stereo & value);

	Stereo & operator/=(const Stereo & value);

	Stereo & operator=(const Stereo & value);


	Stereo & operator+=(const TYPE & value);

	Stereo & operator-=(const TYPE & value);

	Stereo & operator*=(const TYPE & value);

	Stereo & operator/=(const TYPE & value);

	Stereo & operator=(const TYPE & value);


	Stereo & operator++();



	TYPE l;
	
	TYPE r;
		
};




//
//impl

template <class TYPE> struct Reflex::IsRawCopyable < DSP::Stereo <TYPE> >
{
	static constexpr bool value = kIsRawCopyable <TYPE> && (sizeof(DSP::Stereo<TYPE>) == (sizeof(TYPE) * 2));
};

template <class TYPE> REFLEX_INLINE DSP::Stereo<TYPE>::Stereo()
	: l(TYPE())
	, r(TYPE())
{
}

template <class TYPE> REFLEX_INLINE DSP::Stereo<TYPE>::Stereo(const TYPE & value)
	: l(value)
	, r(value)
{
}

template <class TYPE> REFLEX_INLINE DSP::Stereo<TYPE>::Stereo(const TYPE & l, const TYPE & r)
	: l(l)
	, r(r)
{
}

template <class TYPE> template <UInt CHANNEL> REFLEX_INLINE TYPE & DSP::Stereo<TYPE>::GetChannel()
{
	return CHANNEL ? r : l;
}

template <class TYPE> template <UInt CHANNEL> REFLEX_INLINE const TYPE & DSP::Stereo<TYPE>::GetChannel() const
{
	return CHANNEL ? r : l;
}

template <class TYPE> REFLEX_INLINE TYPE & DSP::Stereo<TYPE>::operator[](UInt idx)
{
	REFLEX_ASSERT(idx < 2);

	return (&l)[idx];
}

template <class TYPE> REFLEX_INLINE const TYPE & DSP::Stereo<TYPE>::operator[](UInt idx) const
{
	REFLEX_ASSERT(idx < 2);

	return (&l)[idx];
}

template <class TYPE> REFLEX_INLINE DSP::Stereo <TYPE> & DSP::Stereo<TYPE>::Set(const TYPE & l, const TYPE & r)
{
	this->l = l;
	this->r = r;

	return *this;
}

template <class TYPE> REFLEX_INLINE DSP::Stereo <TYPE> & DSP::Stereo<TYPE>::Set(const TYPE & value)
{
	this->l = value;
	this->r = value;

	return *this;
}

template <class TYPE> REFLEX_INLINE DSP::Stereo <TYPE> & DSP::Stereo<TYPE>::operator+=(const Stereo & value)
{ 
	l += value.l;
	r += value.r;

	return *this;
}

template <class TYPE> REFLEX_INLINE DSP::Stereo <TYPE> & DSP::Stereo<TYPE>::operator-=(const Stereo & value)
{ 
	l -= value.l;
	r -= value.r;

	return *this;
}

template <class TYPE> REFLEX_INLINE DSP::Stereo <TYPE> & DSP::Stereo<TYPE>::operator*=(const Stereo & value)
{ 
	l *= value.l;
	r *= value.r;

	return *this;
}

template <class TYPE> REFLEX_INLINE DSP::Stereo <TYPE> & DSP::Stereo<TYPE>::operator/=(const Stereo & value)
{ 
	l /= value.l;
	r /= value.r;

	return *this;
}

template <class TYPE> REFLEX_INLINE DSP::Stereo <TYPE> & DSP::Stereo<TYPE>::operator=(const Stereo & value)
{ 
	l = value.l;
	r = value.r;

	return *this;
}

template <class TYPE> REFLEX_INLINE DSP::Stereo <TYPE> & DSP::Stereo<TYPE>::operator+=(const TYPE & value)
{ 
	l += value;
	r += value;

	return *this;
}

template <class TYPE> REFLEX_INLINE DSP::Stereo <TYPE> & DSP::Stereo<TYPE>::operator-=(const TYPE & value)
{ 
	l -= value;
	r -= value;

	return *this;
}

template <class TYPE> REFLEX_INLINE DSP::Stereo <TYPE> & DSP::Stereo<TYPE>::operator*=(const TYPE & value)
{ 
	l *= value;
	r *= value;

	return *this;
}

template <class TYPE> REFLEX_INLINE DSP::Stereo <TYPE> & DSP::Stereo<TYPE>::operator/=(const TYPE & value)
{ 
	l /= value;
	r /= value;

	return *this;
}

template <class TYPE> REFLEX_INLINE DSP::Stereo <TYPE> & DSP::Stereo<TYPE>::operator=(const TYPE & value)
{ 
	l = value;
	r = value;

	return *this;
}

template <class TYPE> REFLEX_INLINE DSP::Stereo <TYPE> & DSP::Stereo<TYPE>::operator++()
{ 
	l++;
	r++;

	return *this;
}




//
//functions

template <class TYPE> REFLEX_INLINE DSP::Stereo <TYPE> DSP::operator+(const Stereo <TYPE> & a, const Stereo <TYPE> & b)
{
	return Stereo<TYPE>(a.l + b.l, a.r + b.r);
}

template <class TYPE> REFLEX_INLINE DSP::Stereo <TYPE> DSP::operator-(const Stereo <TYPE> & a, const Stereo <TYPE> & b)
{
	return Stereo<TYPE>(a.l - b.l, a.r - b.r);
}

template <class TYPE> REFLEX_INLINE DSP::Stereo <TYPE> DSP::operator*(const Stereo <TYPE> & a, const Stereo <TYPE> & b)
{
	return DSP::Stereo <TYPE> (a.l * b.l, a.r * b.r);
}

template <class TYPE> REFLEX_INLINE DSP::Stereo <TYPE> DSP::operator/(const Stereo <TYPE> & a, const Stereo <TYPE> & b)
{
	return DSP::Stereo <TYPE>(a.l / b.l, a.r / b.r);
}

template <class TYPE> REFLEX_INLINE DSP::Stereo <TYPE> DSP::operator+(const Stereo <TYPE> & a, const TYPE & b)
{
	return Stereo<TYPE>(a.l + b, a.r + b);
}

template <class TYPE> REFLEX_INLINE DSP::Stereo <TYPE> DSP::operator-(const Stereo <TYPE> & a, const TYPE & b)
{
	return Stereo<TYPE>(a.l - b, a.r - b);
}

template <class TYPE> REFLEX_INLINE DSP::Stereo <TYPE> DSP::operator*(const Stereo <TYPE> & a, const TYPE & b)
{
	return DSP::Stereo <TYPE>(a.l * b, a.r * b);
}

template <class TYPE> REFLEX_INLINE DSP::Stereo <TYPE> DSP::operator/(const Stereo <TYPE> & a, const TYPE & b)
{
	return DSP::Stereo <TYPE>(a.l / b, a.r / b);
}

template <class TYPE> REFLEX_INLINE DSP::Stereo <TYPE> DSP::Abs(const Stereo <TYPE> & value)
{
	return Stereo<TYPE>(Abs(value.l), Abs(value.r));
}

template <class TYPE> REFLEX_INLINE DSP::Stereo <TYPE> DSP::Max(const Stereo <TYPE> & a, const Stereo <TYPE> & b)
{
	return Stereo<TYPE>(Max(a.l, b.l), Max(a.r, b.r));
}

template <class TYPE> REFLEX_INLINE DSP::Stereo <TYPE> DSP::Min(const Stereo <TYPE> & a, const Stereo <TYPE> & b)
{
	return Stereo<TYPE>(Min(a.l, b.l), Min(a.r, b.r));
}
