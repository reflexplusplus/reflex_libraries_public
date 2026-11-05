#pragma once

#include "[require].h"




//
//Primary API

namespace Netx
{

	class ParameterControl;
	
	class PropertyControl;


	void Connect(ParameterControl & control, WillRetain <ParameterSet> parameterset, UInt index);

	void Connect(PropertyControl & control, WillRetain <PropertySet> propertyset, Key32 id);

}




//
//PropertyControl

class Netx::PropertyControl : public Reflex::Bootstrap::ParameterControl
{
public:
	
	REFLEX_OBJECT(Netx::PropertyControl, GLX::Object);



	//lifetime

	PropertyControl() = default;

	~PropertyControl();

	
	
	//connect
	
	void Connect(WillRetain <Netx::PropertySet> propertyset, Key32 id);

	void Disconnect();
};




//
//ParameterControl

class Netx::ParameterControl : public Reflex::Bootstrap::ParameterControl
{
public:
	
	REFLEX_OBJECT(Netx::ParameterControl, GLX::Object);

	NETX_CLASSID(K32("Control"));	//for context menu



	//events

	struct AbstractDragData;
	struct ParameterDragData;
	struct ModulatorDragData;

	REFLEX_GLX_EVENT_ID(StartDragRequest);	//Pair<Key32>ab;
	REFLEX_GLX_EVENT_ID(ControlDropAssign);	//Pair<Key32>ab;
	REFLEX_DECLARE_KEY32(src);
	REFLEX_DECLARE_KEY32(dest);


	
	//lifetime

	ParameterControl();

	~ParameterControl();



	//setup

	void SetSensitivity(Float factor);

	void SetDragDropID(Key32 id);

	Key32 GetDragDropID() const;


	
	//connect
	
	void Connect(WillRetain <Parameter> parameter, bool enabled = true);

	void Disconnect();



	//access

	void SetValueAtomic(Netx::Value32 value);



	//propeties

	const AlreadyRetained <Parameter> parameter;



protected:

	void OnSetProperty(Address address, Reflex::Object & object) override;



	bool OnEvent(GLX::Object & src, GLX::Event & e) override;


private:

	Key32 m_dragdropid;

};




//
//ParameterControl::AbstractDragData

struct Netx::ParameterControl::AbstractDragData : public Object
{
public:

	REFLEX_OBJECT(Netx::ParameterControl::AbstractDragData, Object);


protected:

	AbstractDragData(WillRetain <AbstractProperty> payload, Key32 id)
		: m_payload(payload)
		, m_id(id)
	{
	}

	const Reference <AbstractProperty> m_payload;
	const Key32 m_id;
};




//
//ParameterControl::ParameterDragData

struct Netx::ParameterControl::ParameterDragData : public AbstractDragData
{
	REFLEX_OBJECT(Netx::ParameterControl::ParameterDragData, AbstractDragData);

	ParameterDragData(WillRetain <Parameter> parameter, Key32 destination_id)
		: AbstractDragData(parameter, destination_id)
	{
	}

	AlreadyRetained <Parameter> GetParameter() const { return Cast<Parameter>(m_payload); }
	
	Key32 GetDestination() const { return m_id; }
};




//
//ParameterControl::ModulatorDragData

struct Netx::ParameterControl::ModulatorDragData : public AbstractDragData
{
	REFLEX_OBJECT(Netx::ParameterControl::ModulatorDragData, AbstractDragData);

	ModulatorDragData(WillRetain <ParameterSet> parameterset, Key32 source_id)
		: AbstractDragData(parameterset, source_id)
	{
	}

	AlreadyRetained <ParameterSet> GetParameterSet() const { return Cast<ParameterSet>(m_payload); }
	
	Key32 GetSource() const { return m_id; }
};




//
//impl

inline void Netx::ParameterControl::SetDragDropID(Key32 id)
{
	m_dragdropid = id;
}

inline Key32 Netx::ParameterControl::GetDragDropID() const
{
	return m_dragdropid;
}

inline void Netx::Connect(ParameterControl & control, WillRetain <ParameterSet> parameterset, UInt index)
{
	auto t = AutoRelease(parameterset);

	auto allocation = parameterset->GetAllocation();

	const Netx::Value32 * current = &control.parameter->value;

	const Netx::Value32 * target = allocation->values + index;

	auto current_propertyset = control.parameter->properties.Adr();

	auto target_propertyset = allocation->properties[index].Adr();

	if (current != target || current_propertyset != target_propertyset)
	{
		control.Connect(Parameter::Create(parameterset, index));
	}
}

inline void Netx::Connect(PropertyControl & control, WillRetain <PropertySet> propertyset, Key32 id)
{
	control.Connect(propertyset, id);
}
