#pragma once

#include "[require].h"




//
//token

REFLEX_NS(Netx::Core)

struct Identity
{
	Identity();

	const UInt32 auto_tid;
};

template <class TYPE>
struct Token : public Reflex::Item < Token <TYPE>, false >
{
	Token(AlreadyRetained <TYPE> module) : module(module) {}

	typedef Reflex::Item < Token <TYPE>, false > Item;

	using Item::Attach;

	using Item::Detach;

	using Item::SendTop;

	TYPE & module;
};

REFLEX_END