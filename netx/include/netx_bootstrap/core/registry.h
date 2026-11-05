#pragma once

#include "[require].h"




//
//Primary API

namespace Netx
{

	class Registry;


	[[nodiscard]] Unretained <Module> CreateModule(ClassID classid, const Data::PropertySet & args, UInt8 filter_flags = kMaxUInt8);

}




//
//Registry

class Netx::Registry : public Object
{
public:

	struct Info
	{
		const CString::View name;

		const UInt8 filter_flags = kMaxUInt8;
	};

	struct Class;

	static const Class * Query(ClassID classid, UInt8 filter_flags = kMaxUInt8);
};




//
//Registry::Class

struct Netx::Registry::Class :
	public Reflex::Detail::StaticItem <Class>,
	public Core::iClass,
	public Info
{
	using Ctr = FunctionPointer <Unretained<Module>(Access, ClassID class_id, const Data::PropertySet & args)>;


	//lifetime

	Class(ClassID classid, const Info & info, Ctr ctr) : Core::iClass(classid), Info(info), ctr(ctr) {}


	//properties

	const Ctr ctr;
};
