#pragma once

#include "../[require].h"




//
//declarations

REFLEX_NS(DSP::Detail)

class Overlay;

REFLEX_END




//
//Overlay

class DSP::Detail::Overlay : public GLX::Object
{
public:

	//lifetime

	Overlay();



	//access

	void Show(bool visible, UInt bit = 0);



private:
	
	GLX::Core::Trap OnPointerTender(GLX::Core::PointerAction mouseaction, const GLX::Core::Pointer & pointer, UInt8 flags) override;

	void OnClock(Float) override;


	UInt8 m_show_flags = 0;

};


