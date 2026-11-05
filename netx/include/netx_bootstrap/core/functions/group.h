#pragma once

#include "../module_address.h"




//
//Primary API

namespace Netx
{

	Reference <Module> Clone(Network & network, const Module & module);


	Group * QueryParent(Module & module);

	const Group * QueryParent(const Module & module);


	Array < AlreadyRetained <Module> > GetChildren(const Group & group);


	Module * QueryModuleByID(const Group & group, UInt32 id, Module * fallback = nullptr);

	Module * QueryModuleByClass(const Group & group, ClassID class_id, Module * fallback = nullptr);


	AlreadyRetained <Module> GetModuleByID(const Group & group, UInt32 id);

	AlreadyRetained <Module> GetModuleByClass(const Group & group, ClassID class_id);


	Module * QueryModule(Group & group, const Function <bool(const Module & module)> & callback);


	Module * LookupModule(Group & root, const ModuleAddress & address);

}




//
//impl

inline const Netx::Group * Netx::QueryParent(const Module & module)
{
	return QueryParent(RemoveConst(module));
}

inline Netx::Module * Netx::QueryModuleByID(const Group & group, UInt32 uid, Module * fallback)
{
	group.Enumerate([uid, &fallback](Module & module)
	{
		if (module.GetID() == uid)
		{
			fallback = &module;
		}
	});

	return fallback;
}

inline Netx::Module * Netx::QueryModuleByClass(const Group & group, ClassID class_id, Module * fallback)
{
	group.Enumerate([class_id, &fallback](Module & module)
	{
		if (module.classid == class_id)
		{
			fallback = &module;
		}
	});

	return fallback;
}

inline AlreadyRetained <Netx::Module> Netx::GetModuleByID(const Group & group, UInt32 uid)
{
	return QueryModuleByID(group, uid, &Module::null);
}

inline AlreadyRetained <Netx::Module> Netx::GetModuleByClass(const Group & group, ClassID classid)
{
	return QueryModuleByClass(group, classid, &Module::null);
}
