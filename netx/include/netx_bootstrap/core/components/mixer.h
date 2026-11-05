#pragma once

#include "../[require].h"




//
//Primary API

namespace Netx
{

	class Mixer;

}




//
//Mixer

class Netx::Mixer : public Component
{
public:

	NETX_COMPONENT(Netx, Mixer, K32("Mixer"));

	static Mixer & null;



	//access

	virtual AlreadyRetained <ParameterSet> GetParameters() = 0;
};
