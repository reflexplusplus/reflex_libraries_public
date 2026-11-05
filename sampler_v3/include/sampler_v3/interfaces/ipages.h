#pragma once 

#include "../[require].h"




//
//declarations

REFLEX_NS(SamplerV3)
		
class iPages;

REFLEX_END




//
//iPages

class SamplerV3::iPages
{
public:

	static iPages & null;

	virtual ArrayView < Pair <Key32,WString::View> > GetPages(const Netx::ParameterSet & parameterset) const = 0;
};
