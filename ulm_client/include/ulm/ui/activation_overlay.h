#pragma once

#include "[require].h"



	
//
//declarations

REFLEX_NS(ULM)

class ActivationOverlay;

REFLEX_END




//
//ActivationOverlay

class ULM::ActivationOverlay : public GLX::Object
{
public:

	//types

	REFLEX_DECLARE_KEY32(AllowDragDrop);



	//lifetime

	ActivationOverlay(GLX::Object & owner, FunctionPointer <void(ActivationOverlay&)> onclick = [](ActivationOverlay&) {});

	~ActivationOverlay();



	//setup

	void SetWizardStyle(const WString::View & path, Key32 style_id = kNullKey);



	//run

	void SetProduct(Product & product, Key32 flavour);

	AlreadyRetained <Product> GetProduct();

	void ShowActivationDialog();



private:

	bool OnEvent(GLX::Object & src, GLX::Event & e) override;

	bool OnDragDropReceiveExternal(const Reflex::Object & object) override;

	void OnClock(Float) override;



public:	
	
	struct { UIntNative res[96 / sizeof(UIntNative)]; } m_impl;

};
