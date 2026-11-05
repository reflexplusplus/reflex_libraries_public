#pragma once

#include "[require].h"




//
//Primary API

namespace Netx
{

	class PropertyDef;


	using ToStringFn = FunctionPointer <WString(const PropertyDef&, Value32)>;

	using FromStringFn = FunctionPointer <Value32(const PropertyDef&, WString::View)>;

	using StringConvertor = Tuple <ToStringFn,FromStringFn>;

	extern const StringConvertor kAsContinuous;

	extern const StringConvertor kAsPercent;

	extern const StringConvertor kAsBool;

	extern const StringConvertor kAsInt;
}




//
//PropertyDef

class Netx::PropertyDef : public Reflex::Bootstrap::ParameterDefinition
{
public:
	
	REFLEX_OBJECT(Netx::PropertyDef,Reflex::Bootstrap::ParameterDefinition);

	static PropertyDef & null;



	//types

	REFLEX_USE_ENUM(Bootstrap::ParameterDefinition, Type);

	enum Flags : UInt8
	{
		kFlagUsed = 1,
		kFlagRealtime = 2,

		kFlagsDefault = kFlagUsed | kFlagRealtime,
	};



	//lifetime

	static Unretained <PropertyDef> Create(WString && name, Float min, Float max, Float step, Float defaultvalue, const StringConvertor & convertor = kAsPercent, UInt8 flags = kFlagsDefault);	//float

	static Unretained <PropertyDef> Create(WString && name, Int min, Int max, Int defaultvalue, UInt8 flags = kFlagsDefault);	//int

	static Unretained <PropertyDef> Create(WString && name, Array<WString> && labels, UInt defaultvalue, UInt8 flags = kFlagsDefault);	//enum

	static Unretained <PropertyDef> Create(WString && name, bool defaultvalue, UInt8 flags = kFlagsDefault);	//bool

	static Unretained <PropertyDef> Create(const PropertyDef & def, WString && name, UInt8 flags);	//clone with different name & flags


	const Type type;

	const WString name;

	const Value32 min, max;

	const Value32 default_value;

	const Value32 step;

	const StringConvertor string_convertor;

	const UInt8 group_flags;

	const UInt8 flags;



protected:

	PropertyDef(Type type, WString && name, Value32 min, Value32 max, Value32 default_value, Value32 step, StringConvertor string_convertor, UInt8 group_flags, UInt8 flags);

	virtual Unretained <PropertyDef> Clone(WString && name, UInt8 flags) const;

	Type GetType() const final { return type; }
	Pair <Value32> GetRange() const final { return { min, max }; }
	Value32 GetStep() const final { return step; }
	WString ToString(Value32 value) const final { return string_convertor.a(*this, value); }
	Value32 FromString(WString::View value) const final { return string_convertor.b(*this, value); }
	WString::View GetName() const final { return name; }
	Value32 GetDefaultValue() const final { return default_value; }
	UInt8 GetGroupFlags() const final { return group_flags; }
	UInt8 GetFlags() const final { return flags; }

};

REFLEX_SET_TRAIT(Netx::PropertyDef, IsAbstract);




//
//impl

inline Reflex::Unretained <Netx::PropertyDef> Netx::PropertyDef::Create(const PropertyDef & definition, WString && name, UInt8 flags)
{
	return definition.Clone(std::move(name), flags);
}
