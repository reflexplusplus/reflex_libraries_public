#pragma once

#include "../modules/group.h"
#include "../components/parameterset.h"




//
//Primary API

namespace Netx
{

	using HashFunction = Function <void(UInt64&)>;

	
	void PublishHashFunction(Module & module, const HashFunction & compute_hash);

	const ObjectOf <HashFunction> * QueryHashFunction(const Module & module);


	void AttachChangeTracker(Module & module, Key32 property_id, const Function<void(UInt64)> & on_change);		//deep tracking of changes


	HashFunction MakeGroupHashFunction(Group & group);

	void IncrementHash(UInt64 & hash, const ParameterSet & parameters);

}
