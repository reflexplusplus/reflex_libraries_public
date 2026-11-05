#pragma once

#include "core/defines.h"




//
//declarations

#define NETX_CLASSID(ID) static constexpr Netx::ClassID kClassID = ID; 

#define NETX_CLASS(CLASS, BASE, CLASSID) protected: using BASE::BASE; public: REFLEX_OBJECT(CLASS,BASE); NETX_CLASSID(CLASSID)

#define NETX_COMPONENT(NS, CLASS_NAME, CLASS_ID, ...) NETX_CLASS(NS::CLASS_NAME, ComponentType, CLASS_ID); CLASS_NAME(__VA_ARGS__); [[nodiscard]] static Unretained <CLASS_NAME> Create(__VA_ARGS__)




namespace Netx
{

	REFLEX_USE_ENUM(Core, Scope)

	typedef UInt32 RefreshFlags;

	constexpr auto kRefreshFlagNetworkProperties = Core::kRefreshFlagNetworkProperties;
	constexpr auto kRefreshFlagModuleConnections = Core::kRefreshFlagModuleConnections;
	constexpr auto kRefreshFlagModuleUpdateGroup0 = Core::kRefreshFlagModuleUpdateGroup0;
	constexpr auto kRefreshFlagModuleUpdateGroup1 = Core::kRefreshFlagModuleUpdateGroup1;
	constexpr auto kRefreshFlagModuleUpdateGroup2 = Core::kRefreshFlagModuleUpdateGroup2;
	constexpr auto kRefreshFlagModuleUpdateGroup3 = Core::kRefreshFlagModuleUpdateGroup3;
	constexpr auto kRefreshFlagModuleUpdateGroup4 = Core::kRefreshFlagModuleUpdateGroup4;

	typedef Core::Stream Stream;

	typedef Core::Pin Pin;

	class Network;

	class Module;

	class Component;


	REFLEX_DECLARE_KEY32(SampleRate);
	REFLEX_DECLARE_KEY32(BufferSize);
	REFLEX_DECLARE_KEY32(PPQ);


	extern const WString::View kPercent;


	constexpr UInt32 kStreamContextSession = kHashSeed;

	constexpr UInt32 kStreamContextPreset = K32("preset");

	constexpr UInt32 kStreamContextUndoRedo = K32("history");


	constexpr WString::View kBooleanLabels[2] = { L"Off", L"On" };


	extern const bool & kIsPlugin;

	extern bool booted;	//temp solution, should be detected via Reflex::Module initialisation query
}
