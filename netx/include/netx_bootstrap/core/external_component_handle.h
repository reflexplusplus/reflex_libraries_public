#pragma once

#include "[require].h"




//
//Detail

namespace Netx
{

	class ExternalComponentHandle;

}




//
//ExternalComponentHandle

class Netx::ExternalComponentHandle : public Object
{
public:

	REFLEX_OBJECT(Netx::ExternalComponentHandle, Object);

	static ExternalComponentHandle & null;

	struct Class;	//can be acquired without a C++ component type

	struct Args
	{
		ClassID classid;
		AlreadyRetained <Component> null;
		FunctionPointer <Unretained <Component>(Key32)> create;
		Key32 id;
	};


	static Unretained <ExternalComponentHandle> Acquire(Module & module, const ExternalComponentHandle::Args & args);
	static Unretained <ExternalComponentHandle> Acquire(Module & module, ClassID classid, Key32 id);

	ExternalComponentHandle(Component & null_instance);

	~ExternalComponentHandle();


	const AlreadyRetained <Component> component;



protected:

	ExternalComponentHandle(Module & module, const Args & args);

	ExternalComponentHandle(const ExternalComponentHandle & copy) = delete;



private:

	Reference <Module> m_module;

};




//
//ExternalComponentHandle::Class

struct Netx::ExternalComponentHandle::Class : public Reflex::Detail::StaticItem <Class>
{
	using Create = FunctionPointer <Unretained <Component>(Key32)>;

	Class(ClassID classid, Component & null, Create create) : classid(classid), null(null), create(create) {}

	static const Class * Query(ClassID classid);

	const ClassID classid;
	Component & null;
	const Create create;
};
