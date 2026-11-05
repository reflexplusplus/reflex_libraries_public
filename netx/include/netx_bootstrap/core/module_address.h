#pragma once

#include "modules/group.h"




//
//Primary API

namespace Netx
{

	class ModuleAddress;

}




//
//ModuleAddress

class Netx::ModuleAddress : public Object
{
public:

	REFLEX_OBJECT(Netx::ModuleAddress, Object);

	static ModuleAddress & null;


	
	//construction

	[[nodiscard]] static Unretained <ModuleAddress> Create(const Group & root, const Module & module, UInt32 max_depth = 64);



	//persistence

	void Serialize(Data::Archive & stream) const;

	[[nodiscard]] static Unretained <ModuleAddress> Deserialize(Data::Archive::View & stream);



	//access

	ArrayView <UInt32> GetPath() const { return { m_module_ids, m_depth }; }

	bool operator==(const ModuleAddress & other) const;



protected:

	friend Reflex::Detail::Constructor <ModuleAddress>;

	ModuleAddress(UInt depth);

	[[nodiscard]] static Unretained <ModuleAddress> Create(ArrayView <UInt32> path);


	UInt32 m_depth;

	UInt32 m_module_ids[1];	//variable-size tail

};




//
//impl

REFLEX_SET_TRAIT(Netx::ModuleAddress, IsAbstract);

