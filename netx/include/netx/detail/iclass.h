#pragma once

#include "../defines.h"




REFLEX_NS(Netx::Detail)

struct iClass;

REFLEX_END




//
//iclass

struct Netx::Detail::iClass
{
	iClass(ClassID classid) : classid(classid) {}

	const ClassID classid;
};
