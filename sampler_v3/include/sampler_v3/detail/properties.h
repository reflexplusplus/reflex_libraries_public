#pragma once

#include "../[require].h"




//
//TODO move to impl, public for legacy import

REFLEX_NS(SamplerV3::Detail)

struct SampleProperties
{
	REFLEX_DECLARE_KEY32(MissingTrimStart);
	REFLEX_DECLARE_KEY32(MissingTrimLength);
	REFLEX_DECLARE_KEY32(MissingLoopStart);
	REFLEX_DECLARE_KEY32(MissingLoopLength);

	static constexpr Pair <Key32> kMissingIds[] = { { kMissingTrimStart, kMissingTrimLength }, { kMissingLoopStart, kMissingLoopLength } };

	enum LoopFlags : UInt8
	{
		kLoopFlagEnabled,
		kLoopFlagFlip,
		kLoopFlagDuringRelease,
	};

	Pair <Int> trim;		//8

	Float fade_out;			//4

	Flags32 loop_flags;		//4

	Pair <Int> loop;		//8

	Float loop_xfade;		//4


	static constexpr decltype(&Detail::SampleProperties::trim) kTrimLoop[] = { &Detail::SampleProperties::trim, &Detail::SampleProperties::loop };
};

struct GroupProperties
{
	struct AuxData
	{
		AuxData()
			: enabled_mask(kMaxUInt8)
			, enabled_flags(0)
		{
			outputs[0] = 6;
			outputs[1] = 7;
		}

		UInt8 enabled_mask;
		UInt8 enabled_flags;
		UInt8 outputs[2];
	};

	typedef Map <Key32, Netx::Value32, StandardCompare, true> PropertySetProxy;


	void Reset(const Config & config, const DSP::Table & default_table);

	void Store(const PropertySetProxy & propertysetproxy, Data::Archive & stream) const;

	Key32 engine;
	UInt8 envelope_flags = 0;
	UInt8 lfo_flags = 0;
	UInt8 table_flags = 0;
	UInt8 output = 0;
	AuxData auxes;
	UInt32 pad;

	WString label;

	Pair <DSP::Table, Float> tables[3];

	bool edit_tables_z;
};

REFLEX_END

REFLEX_SET_TRAIT(SamplerV3::Detail::SampleProperties, IsRawCopyable);
REFLEX_SET_TRAIT(SamplerV3::Detail::GroupProperties::AuxData, IsRawCopyable);
