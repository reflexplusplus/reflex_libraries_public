#pragma once

#include "core.h"




//
//Public API

namespace VTime
{

	Unretained <Analysis> AcquireAnalysis(File::ResourcePool::Lock & lock, const DSP::SampleRef & sampleref);

	template <class TYPE> void SetParameter(Player & player, Key32 uid, TYPE value);
}





//
//impl

REFLEX_NS(VTime::Detail)

template <class TYPE> struct TypeToDataType {};

template <> struct TypeToDataType <bool> { static const DataType value = kDataTypeBool8; };
template <> struct TypeToDataType <Quality> { static const DataType value = kDataTypeInt32; };
template <> struct TypeToDataType <Int32> { static const DataType value = kDataTypeInt32; };
template <> struct TypeToDataType <Float32> { static const DataType value = kDataTypeFloat32; };
template <> struct TypeToDataType <Float64> { static const DataType value = kDataTypeFloat64; };

REFLEX_END

template <class TYPE> inline void VTime::SetParameter(Player & player, Key32 uid, TYPE value)
{
	player.SetParameterRt(uid, Detail::TypeToDataType<TYPE>::value, &value);
}

