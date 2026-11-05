#pragma once

#include "../components/parameterset.h"




//
//Primary API

namespace Netx
{

	struct ParameterSetInitialiser;

}




//
//ParameterSetInitialiser

struct Netx::ParameterSetInitialiser
{
	ParameterSetInitialiser(UInt reserve) { paramdefs.Reserve(reserve); }

	void AddParam(Key32 id, ConstWillRetain <Netx::PropertyDef> properties, UInt8 changeflags = 1);

	Unretained <Netx::ParameterSet::Allocation> CreateAllocation() const;

	Array <ParameterSet::Allocation::Definition> paramdefs;
};




//
//Detail

REFLEX_NS(Netx::Detail)

typedef Array <ParameterSet::Allocation::Definition> ParamDefs;

void AddParam(ParamDefs & paramdefs, Key32 id, ConstWillRetain <Netx::PropertyDef> properties, UInt8 changeflags);

Unretained <Netx::ParameterSet::Allocation> CreateAllocation(const ParamDefs & paramdefs);

REFLEX_END

inline void Netx::ParameterSetInitialiser::AddParam(Key32 id, ConstWillRetain <Netx::PropertyDef> properties, UInt8 changeflags)
{
	Detail::AddParam(paramdefs, id, properties, changeflags);
}

inline Unretained <Netx::ParameterSet::Allocation> Netx::ParameterSetInitialiser::CreateAllocation() const
{
	return Detail::CreateAllocation(paramdefs);
}
