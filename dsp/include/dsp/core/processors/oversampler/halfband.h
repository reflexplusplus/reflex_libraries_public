#pragma once

#include "detail/halfband.h"




//
//declarations

namespace DSP
{

	// Linear-phase 2x/4x half-band oversampling algorithm. SAMPLE may be scalar,
	// Stereo<T>, or SIMD. Public aliases select the filter length and factor.

	template <class SAMPLE, UInt FACTOR, class TAPS> class HalfBand;


	template <class SAMPLE> using HalfBand65x2 = HalfBand <SAMPLE, 2, Detail::HalfBandTaps<65, Detail::BlackmanHarris>>;

	template <class SAMPLE> using HalfBand65x4 = HalfBand <SAMPLE, 4, Detail::HalfBandTaps<65, Detail::BlackmanHarris>>;

	template <class SAMPLE> using HalfBand31x2 = HalfBand <SAMPLE, 2, Detail::HalfBandTaps<31, Detail::Hamming>>;

}




//
//HalfBand

template <class SAMPLE, UInt FACTOR, class TAPS>
class DSP::HalfBand
{
public:

	static_assert(FACTOR == 2 || FACTOR == 4);



	//constants

	static constexpr UInt kFactor = FACTOR;

	static constexpr UInt kLatency = TAPS::kGroupDelay + (FACTOR == 4 ? TAPS::kGroupDelay / 2 : 0);



	//upsampler

	struct Upsampler
	{
		Upsampler()
			: m_taps(&TAPS::Get())
		{
		}

		void SetSampleRate(Float /*sr*/)
		{
		}

		void Reset()
		{
			for (auto & stage : m_stages) stage.Reset();
		}

		void Reset(UInt vidx)
		{
			for (auto & stage : m_stages) stage.Reset(vidx);
		}

		REFLEX_INLINE void Process(const SAMPLE & sample, SAMPLE (&values)[FACTOR])
		{
			m_stages[0].Process(*m_taps, sample, values[0], values[1]);

			if constexpr (FACTOR == 4)
			{
				const SAMPLE first = values[0];
				const SAMPLE second = values[1];

				m_stages[1].Process(*m_taps, first, values[0], values[1]);
				m_stages[1].Process(*m_taps, second, values[2], values[3]);
			}
		}

	private:

		const TAPS * m_taps;

		Detail::HalfBandUpsampler2 <SAMPLE,TAPS> m_stages[FACTOR == 4 ? 2 : 1];
	};



	//downsampler

	struct Downsampler
	{
		Downsampler()
			: m_taps(&TAPS::Get())
		{
		}

		void SetSampleRate(Float /*sr*/)
		{
		}

		void Reset()
		{
			for (auto & stage : m_stages) stage.Reset();
		}

		void Reset(UInt vidx)
		{
			for (auto & stage : m_stages) stage.Reset(vidx);
		}

		REFLEX_INLINE SAMPLE Process(const SAMPLE (&values)[FACTOR])
		{
			if constexpr (FACTOR == 2)
			{
				return m_stages[0].Process(*m_taps, values[0], values[1]);
			}
			else
			{
				const SAMPLE first = m_stages[1].Process(*m_taps, values[0], values[1]);
				const SAMPLE second = m_stages[1].Process(*m_taps, values[2], values[3]);

				return m_stages[0].Process(*m_taps, first, second);
			}
		}

	private:

		const TAPS * m_taps;

		Detail::HalfBandDownsampler2 <SAMPLE,TAPS> m_stages[FACTOR == 4 ? 2 : 1];
	};
};

