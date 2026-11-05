#pragma once

#include "../../[require].h"




//
//declarations

namespace DSP::Detail
{

	struct BlackmanHarris;

	struct Hamming;


	// Shared windowed-sinc half-band coefficients. Exact half-band zeros and
	// symmetry leave only the unique side taps and the fixed 0.5 centre tap.

	template <UInt LENGTH, class WINDOW> class HalfBandTaps;


	template <class SAMPLE, class TAPS> class HalfBandUpsampler2;

	template <class SAMPLE, class TAPS> class HalfBandDownsampler2;

	template <class SAMPLE> void ResetHalfBandSample(SAMPLE & sample, UInt vidx);

	template <class SAMPLE> void ResetHalfBandSample(Stereo<SAMPLE> & sample, UInt vidx);

}




//
//HalfBand windows

struct DSP::Detail::BlackmanHarris
{
	static Float64 GetValue(Float64 position);
};

struct DSP::Detail::Hamming
{
	static Float64 GetValue(Float64 position);
};




//
//HalfBandTaps

template <UInt LENGTH, class WINDOW>
class DSP::Detail::HalfBandTaps
{
public:

	static_assert(LENGTH > 1 && (LENGTH & 1));



	//constants

	static constexpr UInt kNumTaps = (LENGTH + 1) / 4;

	static constexpr UInt kHistory = kNumTaps * 2;

	static_assert((kHistory & (kHistory - 1)) == 0);

	static constexpr UInt kGroupDelay = (LENGTH - 1) / 2;

	static constexpr UInt kDirectDelay = kGroupDelay / 2;

	static constexpr bool kCentrePhaseEven = !(kGroupDelay & 1);



	//shared instance

	static const HalfBandTaps & Get();



	//processing

	template <class SAMPLE> SAMPLE Convolve(const SAMPLE * history, UInt position) const;



private:

	HalfBandTaps();

	Float32 m_values[kNumTaps];

};




//
//HalfBandUpsampler2

template <class SAMPLE, class TAPS>
class DSP::Detail::HalfBandUpsampler2
{
public:

	HalfBandUpsampler2();

	void Reset();

	void Reset(UInt vidx);

	void Process(const TAPS & taps, const SAMPLE & input, SAMPLE & out_even, SAMPLE & out_odd);



private:

	SAMPLE m_history[TAPS::kHistory];

	UInt m_position = 0;

};




//
//HalfBandDownsampler2

template <class SAMPLE, class TAPS>
class DSP::Detail::HalfBandDownsampler2
{
public:

	HalfBandDownsampler2();

	void Reset();

	void Reset(UInt vidx);

	SAMPLE Process(const TAPS & taps, const SAMPLE & in_even, const SAMPLE & in_odd);



private:

	SAMPLE m_history[TAPS::kHistory];

	SAMPLE m_delay[TAPS::kHistory];

	UInt m_history_position = 0;

	UInt m_delay_position = 0;

};




//
//helpers

template <class SAMPLE> REFLEX_INLINE void DSP::Detail::ResetHalfBandSample(SAMPLE & sample, UInt vidx)
{
	sample[vidx] = 0.0f;
}

template <class SAMPLE> REFLEX_INLINE void DSP::Detail::ResetHalfBandSample(Stereo<SAMPLE> & sample, UInt vidx)
{
	sample.l[vidx] = 0.0f;
	sample.r[vidx] = 0.0f;
}




//
//impl

inline Float64 DSP::Detail::BlackmanHarris::GetValue(Float64 position)
{
	return 0.35875 - 0.48829 * Reflex::Cos(position) + 0.14128 * Reflex::Cos(2.0 * position) - 0.01168 * Reflex::Cos(3.0 * position);
}

inline Float64 DSP::Detail::Hamming::GetValue(Float64 position)
{
	return 0.54 - 0.46 * Reflex::Cos(position);
}

template <UInt LENGTH, class WINDOW> inline DSP::Detail::HalfBandTaps<LENGTH,WINDOW>::HalfBandTaps()
{
	Float64 raw[kNumTaps];

	Float64 sum = 0.0;

	REFLEX_LOOP(k, kNumTaps)
	{
		const UInt tap = 2 * k + (kCentrePhaseEven ? 1 : 0);

		const Float64 offset = Float64(tap) - Float64(kGroupDelay);

		const Float64 ideal = Reflex::Sin(0.5 * kPi * offset) / (kPi * offset);

		const Float64 window = WINDOW::GetValue(k2Pi * Float64(tap) / Float64(LENGTH - 1));

		raw[k] = ideal * window;

		sum += raw[k];
	}

	const Float64 normalise = 0.25 / sum;

	REFLEX_LOOP(k, kNumTaps)
	{
		m_values[k] = Float32(raw[k] * normalise);
	}
}

template <UInt LENGTH, class WINDOW> inline const DSP::Detail::HalfBandTaps<LENGTH,WINDOW> & DSP::Detail::HalfBandTaps<LENGTH,WINDOW>::Get()
{
	static const HalfBandTaps taps;

	return taps;
}

template <UInt LENGTH, class WINDOW> template <class SAMPLE> REFLEX_INLINE SAMPLE DSP::Detail::HalfBandTaps<LENGTH,WINDOW>::Convolve(const SAMPLE * history, UInt position) const
{
	SAMPLE sum = SAMPLE();

	REFLEX_LOOP(k, kNumTaps)
	{
		const UInt a = (position + k) & (kHistory - 1);

		const UInt b = (position + kHistory - 1 - k) & (kHistory - 1);

		sum += (history[a] + history[b]) * SAMPLE(m_values[k]);
	}

	return sum;
}

template <class SAMPLE, class TAPS> inline DSP::Detail::HalfBandUpsampler2<SAMPLE,TAPS>::HalfBandUpsampler2()
{
	Reset();
}

template <class SAMPLE, class TAPS> inline void DSP::Detail::HalfBandUpsampler2<SAMPLE,TAPS>::Reset()
{
	for (auto & sample : m_history) sample = SAMPLE();

	m_position = 0;
}

template <class SAMPLE, class TAPS> inline void DSP::Detail::HalfBandUpsampler2<SAMPLE,TAPS>::Reset(UInt vidx)
{
	for (auto & sample : m_history) ResetHalfBandSample(sample, vidx);
}

template <class SAMPLE, class TAPS> REFLEX_INLINE void DSP::Detail::HalfBandUpsampler2<SAMPLE,TAPS>::Process(const TAPS & taps, const SAMPLE & input, SAMPLE & out_even, SAMPLE & out_odd)
{
	m_position = (m_position + TAPS::kHistory - 1) & (TAPS::kHistory - 1);

	m_history[m_position] = input;

	const SAMPLE direct = m_history[(m_position + TAPS::kDirectDelay) & (TAPS::kHistory - 1)];

	const SAMPLE filtered = taps.Convolve(m_history, m_position) * SAMPLE(2.0f);

	if constexpr (TAPS::kCentrePhaseEven)
	{
		out_even = direct;
		out_odd = filtered;
	}
	else
	{
		out_even = filtered;
		out_odd = direct;
	}
}

template <class SAMPLE, class TAPS> inline DSP::Detail::HalfBandDownsampler2<SAMPLE,TAPS>::HalfBandDownsampler2()
{
	Reset();
}

template <class SAMPLE, class TAPS> inline void DSP::Detail::HalfBandDownsampler2<SAMPLE,TAPS>::Reset()
{
	for (auto & sample : m_history) sample = SAMPLE();

	for (auto & sample : m_delay) sample = SAMPLE();

	m_history_position = 0;

	m_delay_position = 0;
}

template <class SAMPLE, class TAPS> inline void DSP::Detail::HalfBandDownsampler2<SAMPLE,TAPS>::Reset(UInt vidx)
{
	for (auto & sample : m_history) ResetHalfBandSample(sample, vidx);

	for (auto & sample : m_delay) ResetHalfBandSample(sample, vidx);
}

template <class SAMPLE, class TAPS> REFLEX_INLINE SAMPLE DSP::Detail::HalfBandDownsampler2<SAMPLE,TAPS>::Process(const TAPS & taps, const SAMPLE & in_even, const SAMPLE & in_odd)
{
	m_history_position = (m_history_position + TAPS::kHistory - 1) & (TAPS::kHistory - 1);

	m_history[m_history_position] = TAPS::kCentrePhaseEven ? in_even : in_odd;

	m_delay_position = (m_delay_position + TAPS::kHistory - 1) & (TAPS::kHistory - 1);

	m_delay[m_delay_position] = TAPS::kCentrePhaseEven ? in_odd : in_even;

	const UInt delayed_position = (m_delay_position + TAPS::kDirectDelay) & (TAPS::kHistory - 1);

	return (m_delay[delayed_position] * SAMPLE(0.5f)) + taps.Convolve(m_history, m_history_position);
}
