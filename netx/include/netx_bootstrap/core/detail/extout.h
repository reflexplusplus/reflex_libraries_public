#pragma once

#include "../[require].h"




//
//declarations

namespace Netx::Detail
{

	class ExtOut;

}




//
//ExtOut

class Netx::Detail::ExtOut : public Component
{
public:

	NETX_CLASS(Netx::Detail::ExtOut, ComponentType, K32("ExtOut"));

	static constexpr Int32 kProcessOrder = kMaxInt16;

	static ExtOut & null;

	[[nodiscard]] static Unretained <ExtOut> Create();



	//interface

	virtual void SetEventPinFilter(UInt idx, bool port_filter, UInt8 port, bool channel_filter, UInt8 channel) = 0;

	virtual void Bind(Access, Ref <System::AudioPlugin::EventBuffer> events, const ArrayView <Float*> & audio) = 0;



protected:

	ExtOut();

};
