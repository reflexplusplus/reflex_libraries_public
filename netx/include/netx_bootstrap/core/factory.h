#pragma once

#include "registry.h"




//
//Primary API

namespace Netx
{

	class Factory;

}




//
//Factory

class Netx::Factory : public Object
{
public:

	static Factory & null;

	[[nodiscard]] static Unretained <Factory> Create(UInt8 filter_mask = kMaxUInt8);	//creates from Registry


	[[nodiscard]] virtual Unretained <Module> CreateModule(ClassID classid, const Data::PropertySet & args) const = 0;

};
