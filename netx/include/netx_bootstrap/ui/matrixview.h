#pragma once

#include "detail/componentview.h"




//
//Primary API

namespace Netx
{

	using GetMatrixEndPointsFn = Function < Array< Tuple <Key32,WString> >(const Matrix::Map & map, bool dests)>;	//use SetFunctionProperty(parent_viewport, "GetMatrixEndPoints", ...) null id == seperator

	[[nodiscard]] Unretained <Detail::ComponentView> CreateMatrixView(Matrix & matrix, bool y);

}
