#pragma once

#include "../module_address.h"
#include "../functions/parameter.h"




//
//Secondary API (used by Instance)

namespace Netx::Detail
{

	class ParameterAssignments;

	class AutomationAssignments;

	class MidiAssignments;

}




//
//ParameterAssignments

class Netx::Detail::ParameterAssignments : public Component
{
public:

	using ComponentType = ParameterAssignments;

	struct Impl;

	struct Assignment
	{
		UInt32 source;
		ConstReference <ModuleAddress> module_address;
		ParameterID parameter_id;
	};


	void Clear();

	bool Assign(UInt slot, const ModuleAddress & module_address, ParameterID parameter_id);

	bool Unassign(UInt slot);

	Array <Assignment> GetAssignments() const;


	void Bind(Access, Module & main, ConstRef <System::AudioPlugin::EventBuffer> events);



protected:

	ParameterAssignments(ClassID classid, UInt16 version);

	~ParameterAssignments();


	virtual bool ValidateSlot(UInt32 source) const { return false; }

	virtual void OnAssignmentsChanged() {}

	virtual void Refresh();

	void OnAttachNetwork(Network & network) override;

	void OnDetachNetwork(Network & network) override;


	void OnRestore(Data::Archive::View & stream, Key32 context) override;

	void OnStore(Data::Archive & stream) const override;


	Reflex::Detail::Initialiser <Impl,8,96> impl;
};




//
//AutomationAssignments 

class Netx::Detail::AutomationAssignments : public ParameterAssignments
{
public:

	NETX_CLASS(Netx::Detail::AutomationAssignments, ComponentType, K32("AutomationAssignments"));

	static AutomationAssignments & null;


	[[nodiscard]] static Unretained <AutomationAssignments> Create(System::AudioPlugin & host);

	virtual void GetParameterInfo(UInt slot, CString & name) const = 0;

	virtual Float32 GetParameterValue(UInt slot) const = 0;

	virtual void SetParameterFromHost(UInt slot, Float32 value) = 0;



protected:

	AutomationAssignments(UInt16 version);

};




//
//MidiAssignments

class Netx::Detail::MidiAssignments : public ParameterAssignments
{
public:

	NETX_CLASS(Netx::Detail::MidiAssignments, ComponentType, K32("MidiAssignments"));

	static MidiAssignments & null;


	[[nodiscard]] static Unretained <MidiAssignments> Create();


	static UInt32 EncodeControllerSlot(UInt8 channel, UInt8 cc) { return UInt32(channel) << 8 | cc; }

	static Pair <UInt8> DecodeControllerSlot(UInt32 source) { return { UInt8(source >> 8), UInt8(source) }; }	//channel,slot



protected:

	MidiAssignments(UInt16 version);

};
