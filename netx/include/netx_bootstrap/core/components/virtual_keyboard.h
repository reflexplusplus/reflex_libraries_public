#pragma once

#include "../functions/pin_address.h"




//
//Primary API

namespace Netx
{

	class VirtualKeyboard;

}




//
//VirtualKeyboard

class Netx::VirtualKeyboard : public Component
{
public:

	NETX_COMPONENT(Netx, VirtualKeyboard, K32("VirtualKeyboard"), Scope scope, UInt16 pin_idx);

	static VirtualKeyboard & null;

	
	static Key32 MakeExternalID(Scope scope, UInt16 idx) { return EncodePinAddress(scope, idx); }

	static Unretained <Component> CreateExternal(Key32 id);



	//play

	virtual void Trigger(UInt8 note, Float velocity = 1.0f) = 0;

	virtual void Release(UInt8 note) = 0;

	virtual void ReleaseAll() = 0;

};
