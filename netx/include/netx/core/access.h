#pragma once

#include "[require].h"




//
//Secondary API

namespace Netx
{

	struct Access;

	struct Protected;

}




//
//Access

struct Netx::Access
{
	Access() = delete;

	Access(const Access &) = default;

	Access & operator=(const Access &) = default;
};




//
//Access

struct Netx::Protected
{
protected:

	using Access = Access;

	static Access GetAccess() { Reflex::NullType t; return Reinterpret<Access>(t); }
};




//
//impl

namespace Netx::Detail
{

	inline Access MakeAccess()
	{
		Reflex::NullType t;

		return Reinterpret<Access>(t);
	}

}