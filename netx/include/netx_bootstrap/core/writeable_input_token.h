#pragma once

#include "[require].h"




//
//Primary API

namespace Netx
{

	class WriteableInputToken;

}




//
//WriteableInputToken

class Netx::WriteableInputToken : public Object
{
public:

	REFLEX_OBJECT(WriteableInputToken, Object);

	static WriteableInputToken & null;

	
	
	//lifetime

	WriteableInputToken(WillRetain <Pin> pin);

	~WriteableInputToken();




private:

	Retained <Pin> m_pin;

	Retained <Object> m_counter;
};



