#pragma once

#include "../components/parameterset.h"
#include "../detail/property_normaliser.h"
#include "group.h"




//
//Primary API

namespace Netx
{

	template <UInt SIZE> [[nodiscard]] inline Unretained <ParameterSet::Allocation> CreateParameterAllocation(auto && callable);

	[[nodiscard]] Unretained <ParameterSet::Allocation> CreateParameterAllocation(UInt size, const Function <void(ArrayRegion<ParameterSet::Allocation::Definition>)> & initialise);


	inline ArrayView <Key32> GetIDs(const ParameterSet::Allocation & allocation) { return { allocation.ids, allocation.size }; }

	inline ArrayView <ConstAlreadyRetained<PropertyDef>> GetProperties(const ParameterSet::Allocation & allocation) { return { allocation.properties, allocation.size }; }

	inline ArrayView <UInt8> GetChangeFlags(const ParameterSet::Allocation & allocation) { return { allocation.changeflags, allocation.size }; }

	inline ArrayView <Value32> GetValues(const ParameterSet::Allocation & allocation) { return { allocation.values, allocation.size }; }


	using ParameterID = Pair <Key32>;

	ParameterID GetParameterID(const ParameterSet & parameterset, UInt parameter_index);


	Idx LookupParameter(const ParameterSet::Allocation & allocation, Key32 id);

	Optional < Pair < AlreadyRetained <ParameterSet>, UInt> > LookupParameter(Module & module, ParameterID parameter_id);

	Optional < Pair < AlreadyRetained <ParameterSet>, UInt> > LookupParameter(Group & root, const ModuleAddress & module_address, ParameterID parameter_id);


	void SetParameter(ParameterSet::Transaction & transction, Value32 value);

	void ResetParameter(ParameterSet & paramset, UInt paramidx);

	void ToggleBoolParameter(ParameterSet & paramset, UInt paramidx);


	void SetFlags(Access & access, ParameterSet & set, UInt idx, UInt8 flags);


	Float32 ValueToNormal(const PropertyDef & typeinfo, Value32 value);

	Value32 NormalToValue(const PropertyDef & typeinfo, Float32 value);


	WString ToString(const PropertyDef & typeinfo, Value32 value);

	Value32 FromString(const PropertyDef & typeinfo, const WString::View & string);


	extern const Float kDefaultNormalisedGain;	//0db

	[[nodiscard]] Unretained <Netx::PropertyDef> CreateGainParam(const WString::View & label = L"Gain", Float _default_normalised = kDefaultNormalisedGain);

}




//
//impl

REFLEX_NS(Netx::Detail)

inline UInt GetParameterOffset(const ParameterSet::Allocation & allocation, const Value32 * pvalue) 
{
	return UInt(pvalue - allocation.values); 
}

REFLEX_END

template <UInt SIZE> [[nodiscard]] inline Unretained <Netx::ParameterSet::Allocation> Netx::CreateParameterAllocation(auto && callable)
{
	using Definition = ParameterSet::Allocation::Definition;

	Definition data[SIZE];

	ArrayRegion <Definition> region = { data, SIZE };

	callable(region);

	return ParameterSet::Allocation::Create(ToView(region));
}

inline Netx::ParameterID Netx::GetParameterID(const ParameterSet & parameterset, UInt parameter_index)
{
	auto allocation = parameterset.GetAllocation();

	if (parameter_index < allocation->size)
	{
		return { parameterset.uid, allocation->ids[parameter_index] };
	}
	else
	{
		return {};
	}
}

inline Optional < Pair < AlreadyRetained <Netx::ParameterSet>, UInt > > Netx::LookupParameter(Group & root, const ModuleAddress & module_address, ParameterID parameter_id)
{
	if (auto module = LookupModule(root, module_address))
	{
		return LookupParameter(*module, parameter_id);
	}

	return {};
}

inline WString Netx::ToString(const PropertyDef & properties, Value32 value)
{
	return properties.string_convertor.a(properties, value);
}

inline Netx::Value32 Netx::FromString(const PropertyDef & properties, const WString::View & string)
{
	return properties.string_convertor.b(properties, string);
}

inline Float32 Netx::ValueToNormal(const PropertyDef & typeinfo, Value32 value)
{
	return Detail::kPropertyNormalisers[typeinfo.type]->ValueToNormal(typeinfo, value);
}

inline Netx::Value32 Netx::NormalToValue(const PropertyDef & typeinfo, Float32 value)
{
	return Detail::kPropertyNormalisers[typeinfo.type]->NormalToValue(typeinfo, value);
}
