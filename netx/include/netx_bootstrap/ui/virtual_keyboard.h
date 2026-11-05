#pragma once

#include "detail/componentview.h"




//
//Primary API

namespace Netx
{

	[[nodiscard]] Unretained <GLX::Object> CreateVirtualKeyboardView(VirtualKeyboard & keyboard);

}
