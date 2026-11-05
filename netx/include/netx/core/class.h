#pragma once

#include "defines.h"




//
//Secondary API

namespace Netx::Core
{

	struct iClass;

}




//
//Core::iClass

struct Netx::Core::iClass
{
	using ClassID = ClassID;

	iClass(ClassID classid) : classid(classid) {}

	const ClassID classid;
};
