#pragma once

#include "[require].h"





//
//Primary API

namespace Netx
{

	[[nodiscard]] Unretained <GLX::Object> CreateCpuMeter(Network & network);

	[[nodiscard]] Unretained <GLX::Object> CreateTransportControl(Network & network);

	
	void SetLevelMeterCanvas(GLX::Object & object, WillRetain <Peaks> peaks);	//use GraphicCanvas(id: meter) redraws on each frame without Redraw

	void SetCursorCanvas(GLX::Object & object, WillRetain <Object> owner, FunctionPointer <Float64(Object &)> position);	//use GraphicCanvas(id: cursor), position is in viewport units, redraws on each frame without Redraw

}
