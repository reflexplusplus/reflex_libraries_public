#pragma once

#include "[require].h"




//
//Public API

namespace VTime
{

	enum DataType : UInt8
	{
		kDataTypeFloat32,
		kDataTypeFloat64,
		kDataTypeInt32,
		kDataTypeBool8,
	};

	enum Quality : Int32
	{
		kQualityDraft = 0,
		kQualityStandard = 1,
		kQualityHigh = 2,

		kNumQuality
	};


	struct Analysis;

	struct Player;



	//global

	void EnableCaching(bool enable);

	WString::View GetCachePath();



	//debug

	extern Output output;

}




//
//Analysis

struct VTime::Analysis : public Object
{
	REFLEX_OBJECT(VTime::Analysis, Object);

	static Analysis & null;



	//lifetime

	[[nodiscard]] static Unretained <Analysis> Create(const ArrayView <const Float*> & channels, UInt length, Float32 sr, const Data::PropertySet & properties = REFLEX_NULL(Data::PropertySet));		//sample must exist in memory for duration of lifetime

	

	//progress
	
	virtual const Tuple <bool,Float> & GetProgress() const = 0;

	virtual void Cancel() = 0;


	
	//info
	
	struct Transient
	{
		Float fmin, fmax;

		Float delta;
	};

	virtual void GetTransients(UInt channel, Sequence <Float64,Transient> & values) const {}
};




//
//Player

struct VTime::Player : public Object
{
	REFLEX_OBJECT(VTime::Player, Object);

	static Player & null;


	typedef FunctionPointer <void(Player &,Float64)> SetPositionFn;
	typedef FunctionPointer <Float64(const Player &)> GetPositionFn;
	typedef FunctionPointer <bool(Player&,UInt,Float*)> RenderMonoFn;
	typedef FunctionPointer <bool(Player&,UInt,Float*,Float*)> RenderStereoFn;

	struct Bindings
	{
		SetPositionFn SetPosition;
		GetPositionFn GetPosition;
		RenderMonoFn RenderMono;
		RenderStereoFn RenderStereo;
	};



	//lifetime
	
	[[nodiscard]] static Unretained <Player> Create(const Data::PropertySet & properties = REFLEX_NULL(Data::PropertySet));



	//access
	
	virtual void ClearSample() = 0;

	virtual void SetSample(WillRetain <Analysis> analysis) = 0;

	virtual ConstAlreadyRetained <Analysis> GetSample() const = 0;


	
	// *** RT ***

	virtual void SetParameterRt(Key32 uid, DataType datatype, const void * value) = 0;

	virtual void BindRt(Bindings & bindings) = 0;

};
