#pragma once

#include "../[require].h"




//
//Detail::GenericControl

REFLEX_NS(Netx::Detail)

struct ComponentView : 
	public GLX::Object,
	public Data::iSerializable
{
public:

	REFLEX_OBJECT(ComponentView, GLX::Object);



	//lifetime

	~ComponentView();



	//links

	const AlreadyRetained <Netx::Component> component;



protected:

	ComponentView(Netx::Component & component, UInt16 chunk_version);

	void OnClock(Float32) override;



private:

	Reference <Module> m_module;

	StateMt::Monitor m_monitor;
};

REFLEX_END
