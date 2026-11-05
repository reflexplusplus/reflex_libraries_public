#pragma once

#include "../../netx.h"




//
//Secondary API

namespace Netx
{

	void SetConsoleFocus(Module & module);

	AlreadyRetained <Module> GetConsoleFocus();

	Unretained <Object> CreateConsoleFocusListener(const Function <void(Module&)> & onfocus);

}




//
//impl

REFLEX_NS(Netx::Detail)

void ConnectConsole(Module & module);

void DisconnectConsole(Module & network);

REFLEX_END
