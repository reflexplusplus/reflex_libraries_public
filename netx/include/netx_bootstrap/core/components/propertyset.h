#pragma once

#include "abstract_propertyset.h"




//
//Primary API

namespace Netx
{

	class PropertySet;


	Array <Key32> QueryKeys(const Netx::PropertySet & properties, TypeID type_id);

}




//
//PropertySet

class Netx::PropertySet : public AbstractPropertySet
{
public:

	class Map;

	NETX_COMPONENT(Netx, PropertySet, K32("ClientData"));


	static PropertySet & null;
	

	static Key32 MakeExternalID(Key32 id = kNullKey) { return id; }

	static Unretained <Component> CreateExternal(Key32 id);


	static Unretained <PropertySet> Create(WillRetain <Map> map, Key32 id = kNullKey);


	
	//transaction

	virtual Unretained <Transaction> Begin(Address adr) = 0;



	//ex

	virtual Unretained <Object> CreateListener(const Function <void(File::PersistentPropertySet::Notification action, Key32 context)> & callback) = 0;



	//info

	Array <Address> QueryProperties() const { Array <Address> rtn; QueryProperties(rtn); return rtn; }

	virtual void QueryProperties(Array <Address> & out) const = 0;

	virtual ConstAlreadyRetained <Map> GetMap() const = 0;



	//legacy

	virtual void ImportLegacy(Data::Archive::View & stream) = 0;	//import old dsp::PropertySet / Array < Pair<Key32,Value32> > 

};




//
//PropertySet::Map

class Netx::PropertySet::Map : public Object
{
public:
	
	struct Property;

	static Map & null;


	[[nodiscard]] static Unretained <Map> Create();

	virtual void RegisterProperty(Key32 id, ConstWillRetain <PropertyDef> definition) = 0;

	virtual ArrayView <Property> GetProperties() const = 0;


	void Finalise(Access access) { m_finalised = true; }

	bool IsFinalised() const { return m_finalised; }



private:

	bool m_finalised = false;

};

REFLEX_SET_TRAIT(Netx::PropertySet::Map, IsAbstract);




//
//PropertySet::Map::Property

struct Netx::PropertySet::Map::Property
{
	Address GetAddress() const;

	Key32 id;

	ConstReference <PropertyDef> definition;
};




//
//Detail (legacy)

REFLEX_NS(Netx::Detail)

inline void SetValue(PropertySet & p, Key32 key, Value32 value)
{
	Data::SetBinary(p, key, Data::Pack(value));
}

inline Value32 GetValue(const PropertySet & p, Key32 key, Value32 fallback = MakeValue32(0))
{
	auto binary = Data::GetBinary(p, key);

	return binary.size == 4 ? Data::Unpack<Value32>(binary) : fallback;
}

REFLEX_END

inline Array <Key32> Netx::QueryKeys(const Netx::PropertySet & properties, TypeID type_id)
{
	Array <Key32> rtn;

	auto all = properties.QueryProperties();

	for (auto i : all)
	{
		if (i.type_id == type_id) rtn.Push(i.id);
	}

	return rtn;
}
