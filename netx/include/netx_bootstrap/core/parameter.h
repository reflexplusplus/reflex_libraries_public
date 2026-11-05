#pragma once

#include "components/parameterset.h"
#include "detail/property_normaliser.h"




//
//Primary API

namespace Netx
{

	class Parameter;

}




//
//Parameter

class Netx::Parameter : public Object
{
public:

	REFLEX_OBJECT(Netx::Parameter, Object);

	static Parameter & null;



	//lifetime

	[[nodiscard]] static Unretained <Parameter> Create();

	[[nodiscard]] static Unretained <Parameter> Create(ParameterSet & parameterset, UInt paramidx);

	[[nodiscard]] static Unretained <Parameter> Create(const Parameter & ref);

	~Parameter();



	//access

	bool SetValue(Value32 value);


	void SetNormalised(Float value);

	Float GetNormalised() const;



	//transform

	Value32 NormalToValue(Float value) const;

	Float ValueToNormal(Value32 normal) const;



	//info

	bool operator==(const Parameter & ref) const;

	operator bool() const { return True(allocation); }


	
	//direct access (for automation)
	
	const ConstAlreadyRetained <ParameterSet::Allocation> allocation;	

	const ConstAlreadyRetained <PropertyDef> properties;
	
	const ConstRef <Detail::PropertyNormaliser> normaliser;

	const UInt16 index;

	const Value32 & value;



protected:

	[[nodiscard]] static Unretained <Parameter> Create(ConstWillRetain <ParameterSet::Allocation> allocation, UInt paramidx, ConstWillRetain <PropertyDef> properties);

	Parameter();

	Parameter(ConstWillRetain <ParameterSet::Allocation> allocation, UInt paramidx, ConstWillRetain <PropertyDef> properties);

};

REFLEX_SET_TRAIT(Netx::Parameter,IsAbstract);




//
//impl

inline Unretained <Netx::Parameter> Netx::Parameter::Create()
{
	return Create(ParameterSet::Allocation::null, 0, PropertyDef::null);
}

inline Unretained <Netx::Parameter> Netx::Parameter::Create(const Parameter & paramref)
{
	return Create(paramref.allocation, paramref.index, paramref.properties);
}

inline Unretained <Netx::Parameter> Netx::Parameter::Create(ParameterSet & paramset, UInt paramidx)
{
	auto allocation = paramset.GetAllocation();

	if (paramidx < allocation->size)
	{
		return Create(allocation, paramidx, allocation->properties[paramidx]);
	}
	else
	{
		return {};
	}
}

REFLEX_INLINE Netx::Value32 Netx::Parameter::NormalToValue(Float value) const
{
	return normaliser->NormalToValue(properties, value);
}

REFLEX_INLINE Float Netx::Parameter::ValueToNormal(Value32 normal) const
{
	return normaliser->ValueToNormal(properties, normal);
}

REFLEX_INLINE bool Netx::Parameter::SetValue(Value32 value)
{
	return allocation.RemoveConst()->SetValueMt(index, value);
}

REFLEX_INLINE void Netx::Parameter::SetNormalised(Float normal)
{
	REFLEX_ASSERT(Inside(normal, 0.0f, 1.0001f));

	auto value = normaliser->NormalToValue(properties, normal);

	SetValue(value);
}

REFLEX_INLINE Float Netx::Parameter::GetNormalised() const
{
	auto normal = normaliser->ValueToNormal(properties, value);

	REFLEX_ASSERT(Inside(normal, 0.0f, 1.0001f));

	return normal;
}
