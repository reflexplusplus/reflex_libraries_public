#pragma once

#include "../[require].h"




//
//Secondary API

namespace Netx
{

	template <class BASE> class ModuleBase;

}




//
//ModuleBase

template <class BASE>
class Netx::ModuleBase : public BASE
{
public:

	//Controller

	class Controller : public Component
	{
	public:

		REFLEX_OBJECT(Controller, Component);

		~Controller() = default;



	protected:

		Controller(ModuleBase & owner, ClassID classid, UInt version, bool has_resources) : Component(classid, version, {}, has_resources ? Component::kFlagResources : 0), owner(owner) {}

		using Record = Component::Record;
		using RtConfig = Component::RtConfig;

		void OnStore(Data::Archive & stream) const override;
		void OnRestore(Data::Archive::View & stream, Key32 context) override;
		bool OnImport(UInt16 version, Data::Archive::View & stream, Key32 context) override;
		RtConfig OnUpdate(UInt8 flags, const Data::PropertySet & properties, const StreamsView & streams, const RtConfig * previous) override;
		void OnCollectResources(File::ResourcePool::Lock & lock, Resources & resources) const override;
		void OnRelocateResource(File::ResourcePool::Lock & lock, Address from, const WString & to) override;
		bool OnCommand(Key32 cmd, Data::PropertySet & args) override;

		const AlreadyRetained <ModuleBase> owner;

		friend ModuleBase;
	};



	//lifetime

	~ModuleBase();



	//links

	const AlreadyRetained <Controller> controller;



protected:

	using Record = Controller::Record;
	using StreamsView = Controller::StreamsView;
	using Resources = Controller::Resources;
	using RtConfig = Controller::RtConfig;
	using ProcessInfo = Controller::ProcessInfo;

	
	
	//lifetime
	
	ModuleBase(ClassID class_id);	//for null

	template <class... ARGS> ModuleBase(ClassID class_id, UInt16 version, bool has_resources = false, ARGS &&... args);

	void AttachController();	//typically call after attaching other components, so that OnRestoreController is called last



	//controller callbacks

	virtual void OnStoreController(Data::Archive & stream) const {}

	virtual void OnRestoreController(Data::Archive::View & stream, Key32 context) {}

	virtual bool OnImportController(UInt16 version, Data::Archive::View & stream, Key32 context) { return false; }

	virtual RtConfig OnUpdateController(UInt8 flags, const Data::PropertySet & properties, const StreamsView & streams, const RtConfig * previous) { return {}; }

	virtual void OnCollectResources(File::ResourcePool::Lock & lock, Resources & resources) const {}

	virtual void OnRelocateResource(File::ResourcePool::Lock & lock, Address from, const WString & to) {}

	virtual bool OnCommand(Key32 cmd, Data::PropertySet & args) { return false; }



	//state

	void Update() { controller->Update(); }

	void Notify() { controller->Notify(); }

};




//
//impl

extern template class Netx::ModuleBase<Netx::Module>;

extern template Netx::ModuleBase<Netx::Module>::ModuleBase(ClassID, UInt16, bool);

template <class BASE> inline Netx::ModuleBase<BASE>::ModuleBase(ClassID class_id)	//for null
	: BASE(class_id)
	, controller(Retain(Cast<Controller>(Component::null)))	//safe because cant be public-accessed
{
}

template <class BASE> template <class... ARGS> inline Netx::ModuleBase<BASE>::ModuleBase(ClassID class_id, UInt16 version, bool has_resources, ARGS &&... args)
	: BASE(class_id, std::forward<ARGS>(args)...)
	, controller(Retain(REFLEX_CREATE(Controller, *this, class_id, version, has_resources)))
{
}

template <class BASE> inline void Netx::ModuleBase<BASE>::AttachController()	//typically call after attaching other components, this is so OnRestoreController is called last (after any other components)
{
	this->controller->Attach(this->GetAccess(), *this);
}

template <class BASE> inline Netx::ModuleBase<BASE>::~ModuleBase()
{
	Release(controller);
}

template <class BASE> inline void Netx::ModuleBase<BASE>::Controller::OnStore(Data::Archive & stream) const
{
	owner->OnStoreController(stream);
}

template <class BASE> inline void Netx::ModuleBase<BASE>::Controller::OnRestore(Data::Archive::View & stream, Key32 context)
{
	owner->OnRestoreController(stream, context);
}

template <class BASE> inline bool Netx::ModuleBase<BASE>::Controller::OnImport(UInt16 version, Data::Archive::View & stream, Key32 context)
{
	return owner->OnImportController(version, stream, context);
}

template <class BASE> inline Netx::Component::RtConfig Netx::ModuleBase<BASE>::Controller::OnUpdate(UInt8 flags, const Data::PropertySet & properties, const StreamsView & streams, const RtConfig * previous)
{
	return owner->OnUpdateController(flags, properties, streams, previous);
}

template <class BASE> inline void Netx::ModuleBase<BASE>::Controller::OnCollectResources(File::ResourcePool::Lock & lock, Resources & resources) const
{
	owner->OnCollectResources(lock, resources);
}

template <class BASE> inline void Netx::ModuleBase<BASE>::Controller::OnRelocateResource(File::ResourcePool::Lock & lock, Address from, const WString & to)
{
	owner->OnRelocateResource(lock, from, to);
}

template <class BASE> inline bool Netx::ModuleBase<BASE>::Controller::OnCommand(Key32 cmd, Data::PropertySet & args)
{
	return owner->OnCommand(cmd, args);
}
