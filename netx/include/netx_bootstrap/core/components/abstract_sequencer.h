#pragma once

#include "../util/observed_value.h"




//
//Primary API

namespace Netx
{

	class AbstractSequencer;

}




//
//AbstractSequencer 

class Netx::AbstractSequencer : public Component
{
public:

	REFLEX_OBJECT(Netx::AbstractSequencer, Component);

	static AbstractSequencer & null;



	//types

	struct Impl;

	struct Event
	{
		void Deserialize(Data::Archive::View & stream);	//extended only valid for stream lifetime

		void Serialize(Data::Archive & stream) const;

		UInt32 position;	//ticks
		UInt16 uid;
		UInt16 row;
		UInt32 length;
		Data::Archive::View extended;
	};

	class Sequence;

	class Processor;



	//configuration

	struct Config
	{
		ClassID classid;

		bool serialize = true;

		bool clip_mode = false;
	};


	
	//setup

	//void EnableSongMode



	//content

	void Clear();

	Reference <Object> BeginEdit(UInt idx);					//returns scoped edit, for time-scoped transaction

	void AddSequence(WillRetain <Sequence> sequence);

	void SetSequence(Object & scoped_edit, WillRetain <Sequence> sequence);

	void RemoveSequence(UInt idx);


	UInt GetNumSequence() const;

	ConstAlreadyRetained <Sequence> GetSequence(UInt idx) const;

	bool Validate(const Event & event) const;

	void InitialiseEvent(const Event & event, Data::Archive & extended) const;



	//playback
	
	void SelectSequence(UInt sequence, bool scheduled = false);



	//info 

	const Config config;

	const Scope scope;

	const UInt16 pin_idx;


	Netx::ObservedValue < Pair <Idx,UInt> > current_position;



protected:

	AbstractSequencer(const Config & config, Scope scope, UInt16 pin_idx);
	
	~AbstractSequencer();


	
	//specialisation

	virtual bool OnValidate(const Event & event) const = 0;

	virtual void OnInitialiseEvent(const Event & event, Data::Archive & extended) const = 0;

	virtual Unretained <Processor> OnCompile(UInt32 ppq) = 0;

	virtual Unretained <Object> OnImportMidiEvent(UInt32 midimsg, Event & event, Data::Archive & extended) { return {}; }



private:

	friend struct Impl;

	RtConfig OnUpdate(UInt8 flags, const Data::PropertySet & properties, const StreamsView & streams, const RtConfig * previous) override;

	void OnRestore(Data::Archive::View & stream, Key32 context) override;

	void OnStore(Data::Archive & stream) const override;

	bool OnCommand(Key32 id, Data::PropertySet & params) override;


	Reflex::Detail::Initialiser <Impl,8,16*sizeof(UIntNative)> impl;

};




//
//AbstractSequencer::Sequence

class Netx::AbstractSequencer::Sequence : public Object
{
public:

	REFLEX_OBJECT(Netx::AbstractSequencer::Sequence, Object);

	static Sequence & null;

	Sequence() : Sequence(16) {}

	Sequence(UInt reserve);


	
	//access

	WString::View GetLabel() const { return m_label; }

	UInt32 GetLength() const { return m_length; }

	UInt GetNumEvent() const { return m_events.GetSize(); }

	const Event & GetEvent(UInt idx) const;

	void Enumerate(bool intersects, UInt position, UInt length, const Function <void(const Event&)> & callback) const;

	
	//write (Mutable until successfully submitted to a sequencer)
	
	void SetLabel(WString::View label);

	void SetLength(UInt length);

	void AddEvent(UInt32 position, UInt16 row, UInt32 length_ticks, Data::Archive::View extended);



private:

	friend class AbstractSequencer;

	friend struct Impl;


	WString m_label;

	UInt32 m_length;

	Data::Archive m_extended;

	Reflex::Sequence <UInt,Event> m_events;	//songpos -> Event

	bool m_locked;
};




//
//AbstractSequencer::Processor

class Netx::AbstractSequencer::Processor : public Object
{
public:

	static Processor & null;

	enum Flags : UInt8
	{
		kFlagGateOff = MakeBit(0),		//track event lengths and call GateOff
		kFlagClock = MakeBit(1),		//call Clock instead of emitting the default clip clock
	};

	~Processor() = default;

	virtual bool OnGateOn(UInt buffer_position, const Event & event, UInt offset, UInt64 & client_data, EventStream & output) = 0;

	virtual void OnGateOff(UInt buffer_position, const Event & event, UInt64 client_data, EventStream & output) {}

	virtual void OnClock(UInt buffer_position, UInt songpos, const Event & event, EventStream & output) { }


	const UInt8 flags;



protected:

	Processor(UInt8 flags) : flags(flags) {}

};




//
//impl

inline void Netx::AbstractSequencer::Sequence::SetLength(UInt length)
{
	if (m_locked) return;

	m_length = Max<UInt>(length, 1);
}
