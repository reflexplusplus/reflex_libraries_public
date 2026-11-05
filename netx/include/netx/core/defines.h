#pragma once

#include "[require].h"




//
//Secondary API

namespace Netx::Core
{

	struct Accessor;

	class Network;

	class Module;

	class Pin;

	class Stream;

	enum Scope : UInt8
	{
		kScopeInput,
		kScopeOutput
	};


	typedef UInt32 RefreshFlags;

	constexpr UInt32 kRefreshFlagNetworkProperties = 1;		//properties changed e.g. samplerate, buffersize
	constexpr UInt32 kRefreshFlagModuleConnections = 2;		//modules were added/removed from network, pins were connected (or Reconnect called) (used for pdc change in Freestyle)

	constexpr UInt32 kRefreshFlagModuleUpdateGroup0 = 4;
	constexpr UInt32 kRefreshFlagModuleUpdateGroup1 = 8;
	constexpr UInt32 kRefreshFlagModuleUpdateGroup2 = 16;
	constexpr UInt32 kRefreshFlagModuleUpdateGroup3 = 32;
	constexpr UInt32 kRefreshFlagModuleUpdateGroup4 = 64;

	constexpr UInt32 kRefreshFlagModuleUpdateInternal = 128;

}

namespace Netx
{

	using ClassID = UInt32 ;

	using StateMt = Reflex::ChangeCount <true, true>;	//TRANSITIONAL

	using Value32 = Reflex::Bootstrap::Value32;


	class Network;

	class Module;

	extern Output output;

}
