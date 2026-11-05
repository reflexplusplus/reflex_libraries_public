#pragma once

#include "ui.h"
#include "reflex_ext/bootstrap/audioplugin.h"




//
//Public API

namespace Netx
{

	class Instance;
	
}




//
//Instance

class Netx::Instance : 
	public Bootstrap::App,
	public System::AudioPlugin::Callbacks
{
public:

	REFLEX_OBJECT(Netx::Instance, Bootstrap::App);

	[[nodiscard]] static Unretained <Instance> Create(System::AudioPlugin & audioplugin, const Registry::Class & cls, Data::PropertySet & args = Data::PropertySet::null);


	const AlreadyRetained <System::AudioPlugin> audioplugin;

	const AlreadyRetained <Network> network;

	const AlreadyRetained <Module> root;

	const AlreadyRetained <Detail::MidiAssignments> midi;

	const AlreadyRetained <Detail::AutomationAssignments> automation;



protected:

	Instance(System::AudioPlugin & system, const Registry::Class & cls);

};
