#pragma once

#include "../detail/constants.h"




//
//(@doc:group=Audio)

namespace DSP
{

	template <class TYPE> TYPE HyperbolicSat(const TYPE & value);

	template <class TYPE> TYPE ParabolicSat(const TYPE & value, const TYPE & level);

	template <class TYPE> TYPE ParabolicSatL2(const TYPE & value);

	template <class TYPE> TYPE ParabolicSatL4(const TYPE & value);

}




//
//implementation

template <class TYPE> REFLEX_INLINE TYPE DSP::HyperbolicSat(const TYPE & value)
{
	typedef Detail::Constants <TYPE> Constants;

	TYPE rcp = Reciprocal(Abs(value) + Constants::f_one);

	TYPE t = Constants::f_one - rcp;

	return Select(value > Constants::f_zero, t, -t);
}

template <class TYPE> REFLEX_INLINE TYPE DSP::ParabolicSat(const TYPE & value, const TYPE & level)
{
	typedef Detail::Constants <TYPE> Constants;

	TYPE t0 = level + level;

	TYPE t1 = Clip(value, -t0, t0);

	TYPE t2 = Constants::f_one - (Abs(t1) * (Constants::f_rcp_four / level));

	return t1 * t2;
}

template <class TYPE> REFLEX_INLINE TYPE DSP::ParabolicSatL2(const TYPE & value)
{
	typedef Detail::Constants <TYPE> Constants;

	TYPE t1 = Clip(value, Constants::f_minus_four, Constants::f_four);

	TYPE t2 = Constants::f_one - (Abs(t1) * Constants::f_rcp_eight);

	return t1 * t2;
}

template <class TYPE> REFLEX_INLINE TYPE DSP::ParabolicSatL4(const TYPE & value)
{
	typedef Detail::Constants <TYPE> Constants;

	TYPE t1 = Clip(value, Constants::f_minus_eight, Constants::f_eight);

	TYPE t2 = Constants::f_one - (Abs(t1) * Constants::f_rcp_sixteen);

	return t1 * t2;
}
