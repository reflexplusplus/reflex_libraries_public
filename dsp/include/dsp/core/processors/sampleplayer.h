#pragma once

#include "../detail/constants.h"
#include "../functions/interpolate.h"
#include "../functions/logic.h"




//
//declarations

REFLEX_NS(DSP)

class SamplePlayerV4;

REFLEX_END




//
//SamplePlayerV4

class DSP::SamplePlayerV4
{
public:

	REFLEX_NONCOPYABLE(SamplePlayerV4);



	//types

	class LoopInfo;

	//lifetime

	SamplePlayerV4();



	//setup
	
	void SetSampleRate(Float sr);


	
	//parameters
	
	void SetFrequency(const FloatV4 & frequency);	//->make static, use SetSpeed, GetSpeed

	void SetFrequency(UInt vidx, Float frequency);


	
	//transport
	
	void Start(UInt vidx, Float sample_sr, const Float * sample, Float start, Float end, Float frequency, bool reverse);

	void Start(UInt vidx, Float sample_sr, const Float * left, const Float * right, Float start, Float end, Float frequency, bool reverse);


	void ReleaseLoop(LoopInfo & loopinfo, UInt vidx);


	void Stop();

	void Stop(UInt vidx);



	//standard

	FloatV4 Process();

	StereoV4 ProcessStereo();


	
	//pos modulation
	
	//FloatV4 Process(const FloatV4 & offset);

	//StereoV4 ProcessStereo(const FloatV4 & offset);



	//looped
	
	template <bool FADE, bool REFLEX_LOOP, bool LOOP_FLIP, bool LOOP_FADE> FloatV4 Process(const LoopInfo & loopinfo);

	template <bool FADE, bool REFLEX_LOOP, bool LOOP_FLIP, bool LOOP_FADE> StereoV4 ProcessStereo(const LoopInfo & loopinfo);



	//info

	const FloatV4 & GetStart() const;

	const FloatV4 & GetEnd() const;

	FloatV4 GetPosition() const;

	BoolV4 Active() const;

	BoolV4 Active(const LoopInfo & loopinfo) const;



private:

	struct Processor;
	

	static FloatV4 ToSingle(const System::Float64x2 * sse);

	static void WriteDouble(System::Float64x2 * sse, const FloatV4 & value);

	static Float64 & GetVoice(System::Float64x2 * sse, UInt vidx);

	FloatV4 GetPos() const;

	void IncPos();


	static void ReadChannel(const Float ** channels, const IntV4 & pos, const FloatV4 & x, FloatV4 & value);

	static void ReadChannelLinear(const Float ** channels, const IntV4 & pos, const FloatV4 & x, FloatV4 & value);

	template <class SAMPLE, bool FADE_OUT, bool REFLEX_LOOP, bool LOOP_FLIP, bool LOOP_FADE> SAMPLE ProcessLooped(const LoopInfo & loopinfo);

	template <bool FLIP> void Inc(const LoopInfo & loopinfo);


	Float m_sr_rcp;


	const Float * m_left[4];

	const Float * m_right[4];


	FloatV4 m_sample_sr;

	bool m_reverse[4];

	FloatV4 m_start;

	FloatV4 m_end;


	BoolV4 m_fwdinc;

	System::Float64x2 m_inc[2];

	System::Float64x2 m_pos[2];


	alignas(16) static const Float st_null[16];

};




//
//SamplePlayerV4::LoopInfo

class DSP::SamplePlayerV4::LoopInfo 
{
public:

	LoopInfo();

	void Start(UInt vidx, Float fadeout, bool loop_enable, Float loop_start, Float loop_length, Float loop_fade);


	void SetRange(const FloatV4 & loop_start, const FloatV4 & loop_length);

	const FloatV4 & GetStart() const { return loop_start; }

	const FloatV4 & GetLength() const { return loop_length; }

	const FloatV4 & GetEnd() const { return loop_end; }



private:

	friend class SamplePlayerV4;


	BoolV4 m_active;


	FloatV4 fadeout;

	FloatV4 fadeout_rcp;


	FloatV4 loop_start;
	
	FloatV4 loop_length;

	FloatV4 loop_end;


	FloatV4 loop_fade;

	FloatV4 loop_fade_rcp;

};




//
//impl

struct DSP::SamplePlayerV4::Processor
{
	static void Read(SamplePlayerV4 & self, const IntV4 & ipos, const FloatV4 & x, FloatV4 & sample)
	{
		ReadChannel(self.m_left, ipos, x, sample);
	}

	static void Read(SamplePlayerV4 & self, const IntV4 & ipos, const FloatV4 & x, StereoV4 & sample)
	{
		ReadChannel(self.m_left, ipos, x, sample.l);
		
		ReadChannel(self.m_right, ipos, x, sample.r);
	}

	static void ReadLinear(SamplePlayerV4 & self, const IntV4 & ipos, const FloatV4 & x, FloatV4 & sample)
	{
		ReadChannelLinear(self.m_left, ipos, x, sample);
	}

	static void ReadLinear(SamplePlayerV4 & self, const IntV4 & ipos, const FloatV4 & x, StereoV4 & sample)
	{
		ReadChannelLinear(self.m_left, ipos, x, sample.l);

		ReadChannelLinear(self.m_right, ipos, x, sample.r);
	}

	template <class SAMPLE> REFLEX_INLINE static void Process(SamplePlayerV4 & self, const FloatV4 & pos, SAMPLE & sample)
	{
		IntV4 ipos = Truncate(pos);

		auto x = pos - ToFloatV4(ipos);

		ipos -= kiOne;

		Read(self, ipos, x, sample);

		self.IncPos();
	}

};

REFLEX_INLINE DSP::SamplePlayerV4::SamplePlayerV4()
	: m_sr_rcp(1.0f)
	, m_sample_sr(1.0f)
	, m_fwdinc(kTrueV4)
{
	Stop();
}

inline void DSP::SamplePlayerV4::SetSampleRate(Float sr)
{
	m_sr_rcp = Reciprocal(sr);

	//!dont call stop here, not thread safe
	//frequency will now be invalid, but up to client to redo Start
}

REFLEX_INLINE void DSP::SamplePlayerV4::SetFrequency(const FloatV4 & frequency)
{
	WriteDouble(m_inc, Sign(ToSingle(m_inc)) * frequency * m_sample_sr * FloatV4(m_sr_rcp));
}

REFLEX_INLINE void DSP::SamplePlayerV4::SetFrequency(UInt vidx, Float frequency)
{
	Float64 & inc = GetVoice(m_inc, vidx);

	inc = Sign(inc) * frequency * m_sample_sr[vidx] * m_sr_rcp;
}

REFLEX_INLINE void DSP::SamplePlayerV4::Start(UInt vidx, Float sample_sr, const Float * sample, Float start, Float end, Float frequency, bool reverse)
{
	Start(vidx, sample_sr, sample, sample, start, end, frequency, reverse);
}

REFLEX_INLINE void DSP::SamplePlayerV4::Start(UInt vidx, Float sample_sr, const Float * left, const Float * right, Float start, Float end, Float frequency, bool reverse)
{
	m_sample_sr[vidx] = sample_sr;

		
	m_left[vidx] = left;

	m_right[vidx] = right;


	m_start[vidx] = start;

	m_end[vidx] = end;

	
	m_reverse[vidx] = reverse;

	if (reverse)
	{
		m_fwdinc[vidx] = SIMD::kBooleanFalse;

		GetVoice(m_inc, vidx) = -frequency * m_sample_sr[vidx] * m_sr_rcp;
		
		GetVoice(m_pos, vidx) = m_end[vidx];
	}
	else
	{
		m_fwdinc[vidx] = SIMD::kBooleanTrue;

		GetVoice(m_inc, vidx) = frequency * m_sample_sr[vidx] * m_sr_rcp;

		GetVoice(m_pos, vidx) = start;
	}
}

REFLEX_INLINE void DSP::SamplePlayerV4::ReleaseLoop(LoopInfo & loopinfo, UInt vidx)
{	
	//bool to_end = loopinfo.loop_toend[vidx];

	bool reverse = m_reverse[vidx];

	//bool stop_start = Select(to_end, false, reverse);

	//bool stop_end = Select(to_end, false, !reverse);

	bool stop_start = reverse;

	bool stop_end = !reverse;


	//SSE_Boolean & active = loopinfo.active[vidx];

	//active = Select(to_end, active, SSE_False);


	Float & loop_start = loopinfo.loop_start[vidx];
	
	Float & loop_end = loopinfo.loop_end[vidx];

	loop_start = Select(stop_start, -158760000.0f, loop_start);

	loop_end = Select(stop_end, 158760000.0f, loop_end);

	loopinfo.loop_length = loopinfo.loop_end - loopinfo.loop_start + kfOne;

	loopinfo.m_active[vidx] = SIMD::kBooleanFalse;
}

REFLEX_INLINE void DSP::SamplePlayerV4::Stop()
{
	m_fwdinc = kFalseV4;

	m_start = kfZero;

	m_end = kfOne;


	REFLEX_LOOP(idx, 2)
	{
		m_pos[idx] = REFLEX_SET_F64_X2(0.0, 0.0);

		m_inc[idx] = REFLEX_SET_F64_X2(0.0, 0.0);
	}


	const Float * null = st_null + 4;

	REFLEX_LOOP(idx, 4)
	{
		m_left[idx] = null;

		m_right[idx] = null;
	}
}

REFLEX_INLINE void DSP::SamplePlayerV4::Stop(UInt vidx)
{
	const Float * null = st_null + 4;

	m_left[vidx] = null;

	m_right[vidx] = null;


	m_fwdinc[vidx] = SIMD::kBooleanFalse;

	m_start[vidx] = 0.0f;

	m_end[vidx] = 1.0f;

	
	GetVoice(m_inc, vidx) = 0.0f;

	GetVoice(m_pos, vidx) = 0.0f;
}

REFLEX_INLINE FloatV4 DSP::SamplePlayerV4::ToSingle(const System::Float64x2 * sse)
{
	FloatV4 lower(REFLEX_F64_X2_TO_F32_X4(sse[0]));
	FloatV4 upper(REFLEX_F64_X2_TO_F32_X4(sse[1]));
	return Shuffle<0,1,0,1>(lower, upper);
}

REFLEX_INLINE void DSP::SamplePlayerV4::WriteDouble(System::Float64x2 * sse, const FloatV4 & value)
{
	sse[0] = REFLEX_F32_X4_TO_F64_X2(value);

	sse[1] = REFLEX_F32_X4_TO_F64_X2((Shuffle<2,3,2,3,Float>(value)));
}

REFLEX_INLINE Float64 & DSP::SamplePlayerV4::GetVoice(System::Float64x2 * sse, UInt vidx)
{
	return Reinterpret<Float64>(sse)[vidx];
}

REFLEX_INLINE FloatV4 DSP::SamplePlayerV4::GetPos() const
{
	return Clip(ToSingle(m_pos), m_start, m_end);
}

REFLEX_INLINE void DSP::SamplePlayerV4::IncPos()
{
	m_pos[0] = REFLEX_ADD_F64_X2(m_pos[0], m_inc[0]);

	m_pos[1] = REFLEX_ADD_F64_X2(m_pos[1], m_inc[1]);
}

REFLEX_INLINE FloatV4 DSP::SamplePlayerV4::Process()
{
	FloatV4 value;

	Processor::Process(*this, GetPos(), value);

	return value;
}

REFLEX_INLINE DSP::StereoV4 DSP::SamplePlayerV4::ProcessStereo()
{
	StereoV4 value;

	Processor::Process(*this, GetPos(), value);

	return value;
}

template <bool FADE_OUT, bool REFLEX_LOOP, bool LOOP_FLIP, bool LOOP_FADE> REFLEX_INLINE FloatV4 DSP::SamplePlayerV4::Process(const LoopInfo & loopinfo)
{
	return ProcessLooped<FloatV4,FADE_OUT,REFLEX_LOOP,LOOP_FLIP,LOOP_FADE>(loopinfo);
}

template <bool FADE_OUT, bool REFLEX_LOOP, bool LOOP_FLIP, bool LOOP_FADE> REFLEX_INLINE DSP::StereoV4 DSP::SamplePlayerV4::ProcessStereo(const LoopInfo & loopinfo)
{
	return ProcessLooped<StereoV4,FADE_OUT,REFLEX_LOOP,LOOP_FLIP,LOOP_FADE>(loopinfo);
}

template <class SAMPLE, bool FADE_OUT, bool REFLEX_LOOP, bool LOOP_FLIP, bool LOOP_FADE> REFLEX_INLINE SAMPLE DSP::SamplePlayerV4::ProcessLooped(const LoopInfo & loopinfo)
{
	FloatV4 pos = GetPos();

	IntV4 ipos = Truncate(pos);

	auto x = pos - ToFloatV4(ipos);

	ipos -= kiOne;

	SAMPLE value;

	Processor::Read(*this, ipos, x, value);

	if constexpr (FADE_OUT)
	{
		FloatV4 ramp = Select(m_fwdinc, m_end - pos, pos - m_start);

		ramp = Min(ramp * loopinfo.fadeout_rcp, kfOne);

		value *= ramp;
	}

	if constexpr (REFLEX_LOOP)
	{
		if constexpr (LOOP_FADE)
		{
			FloatV4 ramp = Select(m_fwdinc, pos + loopinfo.loop_fade - loopinfo.loop_end, loopinfo.loop_start + loopinfo.loop_fade - pos);

			FloatV4 ramp_up = Clip(ramp * loopinfo.loop_fade_rcp, kfZero, kfOne);

			FloatV4 ramp_down = kfOne - ramp_up;

			value *= ramp_down;


			FloatV4 pos_fadein = pos - Select(m_fwdinc, loopinfo.loop_length);

			pos_fadein += SelectNot(m_fwdinc, loopinfo.loop_length);

			pos_fadein = Clip(pos_fadein, kfZero, m_end);

			SAMPLE read;

			Processor::ReadLinear(*this, Truncate(pos_fadein) - kiOne, x, read);

			value += (read * ramp_up);
		}

		Inc<LOOP_FLIP>(loopinfo);
	}
	else
	{
		IncPos();
	}

	return value;
}

//template <bool FADE_OUT, bool REFLEX_LOOP, bool LOOP_FLIP, bool LOOP_FADE> REFLEX_INLINE void DSP::SamplePlayerV4::Process(const LoopInfo & loopinfo, StereoV4 & value)
//{
//	FloatV4 pos = GetPos();
//
//	IntV4 ipos = Truncate(pos);
//
//	FloatV4 x = pos - IntToFloat(ipos);
//
//	ipos -= kiOne;
//
//	
//	
//	//read loopfade
//
//	ReadChannel(m_left, ipos, x, value.l);
//
//	ReadChannel(m_right, ipos, x, value.r);
//
//	if (FADE_OUT)
//	{
//		FloatV4 ramp = Select(m_fwdinc, m_end - pos, pos - m_start);
//
//		ramp = Min(ramp * loopinfo.fadeout_rcp, kfOne);
//
//		value *= ramp;
//	}
//
//	if (REFLEX_LOOP && LOOP_FADE)
//	{
//		FloatV4 ramp = Select(m_fwdinc, pos + loopinfo.loop_fade - loopinfo.loop_end, loopinfo.loop_start + loopinfo.loop_fade - pos);
//
//		FloatV4 ramp_up = Clip(ramp * loopinfo.loop_fade_rcp, kfZero, kfOne);
//
//		FloatV4 ramp_down = kfOne - ramp_up;
//
//		value.l *= ramp_down;
//
//		value.r *= ramp_down;
//
//
//		FloatV4 pos_fadein = pos - Select(m_fwdinc, loopinfo.loop_length);
//
//		pos_fadein += SelectNot(m_fwdinc, loopinfo.loop_length);
//
//		pos_fadein = Clip(pos_fadein, kfZero, m_end);
//		
//		IntV4 loop_pos = Truncate(pos_fadein) - kiOne;
//
//		FloatV4 read;
//
//		ReadChannelLinear(m_left, loop_pos, x, read);
//
//		value.l += (read * ramp_up);
//
//		ReadChannelLinear(m_right, loop_pos, x, read);
//
//		value.r += (read * ramp_up);
//	}
//
//
//	
//	//inc
//	
//	if (REFLEX_LOOP)
//	{
//		Inc<LOOP_FLIP>(loopinfo);
//	}
//	else
//	{
//		IncPos();
//	}
//}

REFLEX_INLINE BoolV4 DSP::SamplePlayerV4::Active() const
{
	FloatV4 pos = ToSingle(m_pos);

	return Select(m_fwdinc, pos < m_end, pos > m_start);
}

REFLEX_INLINE BoolV4 DSP::SamplePlayerV4::Active(const LoopInfo & loopinfo) const
{
	FloatV4 pos = ToSingle(m_pos);

	return DSP::Or(Select(m_fwdinc, pos < m_end, pos > m_start), loopinfo.m_active);
}

REFLEX_INLINE const FloatV4 & DSP::SamplePlayerV4::GetStart() const
{
	return m_start;
}

REFLEX_INLINE const FloatV4 & DSP::SamplePlayerV4::GetEnd() const
{
	return m_end;
}

REFLEX_INLINE FloatV4 DSP::SamplePlayerV4::GetPosition() const
{
	return ToSingle(m_pos);
}

REFLEX_INLINE void DSP::SamplePlayerV4::ReadChannel(const Float ** channels, const IntV4 & ipos, const FloatV4 & x, FloatV4 & value)
{
	FloatV4 l_1(LoadUnaligned((*channels) + ipos[0]));
	FloatV4 l0(LoadUnaligned((*++channels) + ipos[1]));
	FloatV4 l1(LoadUnaligned((*++channels) + ipos[2]));
	FloatV4 l2(LoadUnaligned((*++channels) + ipos[3]));

	Transpose(l_1, l0, l1, l2);

	value = Interpolate4P_RT(x, l_1, l0, l1, l2);
}

REFLEX_INLINE void DSP::SamplePlayerV4::ReadChannelLinear(const Float ** channels, const IntV4 & ipos, const FloatV4 & x, FloatV4 & value)
{
	const Float * ptr0 = (*channels) + ipos[0];
	const Float * ptr1 = (*++channels) + ipos[1];
	const Float * ptr2 = (*++channels) + ipos[2];
	const Float * ptr3 = (*++channels) + ipos[3];

	FloatV4 a = { *ptr0++, *ptr1++, *ptr2++, *ptr3++ };
	
	FloatV4 b = { *ptr0, *ptr1, *ptr2, *ptr3 };

	value = LinearInterpolate(x, a, b);
}

template <bool FLIP> REFLEX_INLINE void DSP::SamplePlayerV4::Inc(const LoopInfo & loopinfo)
{
	System::Float64x2 pos64[2] = {REFLEX_ADD_F64_X2(m_pos[0], m_inc[0]), REFLEX_ADD_F64_X2(m_pos[1], m_inc[1])};

	FloatV4 pos = ToSingle(pos64);

	FloatV4 inc = ToSingle(m_inc);
	
	m_fwdinc = inc > kfZero;

	BoolV4 wrap_end = Select(m_fwdinc, pos > loopinfo.loop_end);

	BoolV4 wrap_start = SelectNot(m_fwdinc, pos < loopinfo.loop_start);

	if (FLIP)
	{
		const FloatV4 flip = Detail::Constants<FloatV4>::f_minus_two;

		FloatV4 flipover = Select(wrap_end, pos - loopinfo.loop_end);

		FloatV4 flipunder = Select(wrap_start, pos - loopinfo.loop_start);


		System::Float64x2 shift[2];
		
		WriteDouble(shift, ((flipover + flipunder) * flip));

		m_pos[0] = REFLEX_SUB_F64_X2(pos64[0], shift[0]);
		
		m_pos[1] = REFLEX_SUB_F64_X2(pos64[1], shift[1]);


		inc += Select(SIMD::Or(wrap_end, wrap_start), inc * flip);

		WriteDouble(m_inc, inc);
	}
	else
	{
		pos = Select(SIMD::Or(wrap_start, wrap_end), loopinfo.loop_start + Modulo(pos - loopinfo.loop_start, loopinfo.loop_length), pos);

		WriteDouble(m_pos, pos);
	}
}

REFLEX_INLINE DSP::SamplePlayerV4::LoopInfo::LoopInfo()
{
	m_active = kFalseV4;


	fadeout = 0.0f;

	fadeout_rcp = 0.0f;


	loop_start = -158760000.0f;

	loop_length = 158760000.0f;	

	loop_end = 158760000.0f;	


	loop_fade = 0.0f;

	loop_fade_rcp = 0.0f;


	//REFLEX_LOOP(vidx, 4)
	//{
	//	loop_toend[vidx] = true;
	//}
}

REFLEX_INLINE void DSP::SamplePlayerV4::LoopInfo::Start(UInt vidx, Float fadeout, bool loop_enable, Float loop_start, Float loop_length, Float loop_fade)
{
	m_active[vidx] = loop_enable ? SIMD::kBooleanTrue : SIMD::kBooleanFalse;


	fadeout += 1.0f;

	this->fadeout[vidx] = fadeout;

	this->fadeout_rcp[vidx] = Reciprocal(fadeout);


	this->loop_start[vidx] = loop_start;

	this->loop_length[vidx] = loop_length;

	this->loop_end[vidx] = loop_start + loop_length - 1.0f;

	loop_fade += 1.0f;

	this->loop_fade[vidx] = loop_fade;

	this->loop_fade_rcp[vidx] = Reciprocal(loop_fade);

	//this->loop_toend[vidx] = loop_toend;
}

REFLEX_INLINE void DSP::SamplePlayerV4::LoopInfo::SetRange(const FloatV4 & loop_start, const FloatV4 & loop_length)
{
	this->loop_start = loop_start;

	this->loop_length = loop_length;

	this->loop_end = Select(m_active, loop_start + loop_length - kfOne, this->loop_end);
}

//REFLEX_INLINE const FloatV4 & DSP::SamplePlayerV4::LoopInfo::GetStart() const
//{
//	return loop_start;
//}
//
//REFLEX_INLINE const FloatV4 & DSP::SamplePlayerV4::LoopInfo::GetLength() const
//{
//	return loop_length;
//}
