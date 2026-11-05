#pragma once

#include "../[require].h"




//
//Primary API

namespace Netx
{

	class DynamicPins;

}




//
//DynamicPins

class Netx::DynamicPins : public Component
{
public:

	NETX_COMPONENT(Netx, DynamicPins, K32("DynamicPins"), ArrayView <ClassID> classes, Scope scope, Pin::Mode mode = Pin::kModeAuto);

	static DynamicPins & null;

	static DynamicPins * Query(Module & module, Scope scope) { return QueryComponent<DynamicPins>(module, Key32(scope)); }



	//info

	virtual bool CanAddPin(ClassID classid) const = 0;

	bool CanRemovePin(const Pin & pin) const;


	
	//access

	virtual AlreadyRetained <Pin> AddPin(ClassID class_id, const Data::PropertySet & metadata = Data::PropertySet::null) = 0;

	virtual void RemovePin(UInt idx) = 0;

	virtual void SetPinMetadata(UInt idx, const Data::PropertySet & metadata) = 0;

	virtual void ReorderPin(UInt from, UInt to) = 0;


	virtual void EnumeratePins(const Function<void(ClassID, const Data::PropertySet&)> & enumerate) const = 0;	// Metadata is valid only until the callback returns.



	const Scope scope;

	const Pin::Mode mode;



protected:

	DynamicPins(Scope scope, Pin::Mode mode);

};




//
//impl

inline bool Netx::DynamicPins::CanRemovePin(const Pin & pin) const
{
	return GetScope(pin) == DynamicPins::scope /*&& pin.classid == DynamicPins::pin_classid*/;
}
