#pragma once

#include "[require].h"




//
//Public API

REFLEX_NS(DSP)

class SmootherBank;
	
REFLEX_END

	


//
//SmootherBank

class DSP::SmootherBank : public Object
{
public:

	static SmootherBank & null;


	enum LaneIdx : UInt8
	{
		kLanes0123 = 0,
		kLanes4567 = 1,
		kLanesEtc = 2,
	};

	struct LaneItr
	{
		Float Read();

		const Float * ptr;
	};



	//lifetime

	static Unretained <SmootherBank> Create(UInt num_lane, UInt max_buffer_size);


	
	//setup

	virtual void SetSampleRate(Float sr) = 0;


	virtual void SetTime(Float time, Float dynamic_range = 1.0f) = 0;

	virtual void SetTime(UInt idx, Float time, Float dynamic_range = 1.0f) = 0;


	virtual void SetState(Float value) = 0;	//set the inner state value, use to immediately jump

	virtual void SetState(UInt idx, Float value) = 0;	//set the inner state value, use to immediately jump



	//processing
	
	virtual void SetTargetValues(const FloatV4 * values) = 0;

	virtual void SetTargetValue(UInt idx, Float value) = 0;

	virtual void GetTargetValues(FloatV4 * values) const = 0;


	UInt64 Process(UInt samples);

	virtual const FloatV4 * ReadLanes(LaneIdx idx) const = 0;

	LaneItr ReadLane(UInt idx) const;


	
	//info

	const UInt16 num_lane;
	
	const UInt16 max_buffer_size = 0;



protected:

	SmootherBank(UInt num_lane, UInt max_buffer_size);


	virtual UInt64 OnProcess(UInt samples) = 0;

	UInt64 m_active_flags;

};




//
//impl

inline UInt64 DSP::SmootherBank::Process(UInt samples)
{
	if (m_active_flags)
	{
		m_active_flags = OnProcess(samples);
		
		return m_active_flags;
	}

	return 0;
}

inline Float DSP::SmootherBank::LaneItr::Read()
{
	auto r = *ptr;

	ptr += 4;

	return r;
}

inline DSP::SmootherBank::LaneItr DSP::SmootherBank::ReadLane(UInt idx) const
{
	auto block = idx / 4;

	auto vidx = idx & 3;

	auto ptr_v4 = ReadLanes(LaneIdx(block));

	return { Reinterpret<Float>(ptr_v4) + vidx };
}
