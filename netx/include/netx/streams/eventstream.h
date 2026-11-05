#pragma once

#include "../defines.h"
#include "../core/stream.h"




//
//Primary API

namespace Netx
{

	class EventStream;


	enum EventType : UInt8
	{
		kEventTypeClock,

		kEventTypeGateTrigger,
		kEventTypeGateAfterTouch,
		kEventTypeGateRelease,
		kEventTypeGateChoke,

		kEventTypeController,

		kEventTypeProgramChange,

		kEventTypeAutomation,

		kNumEventType,
	};

	enum Controller : UInt8
	{
		kControllerPitchBend = 0,
		kControllerModWheel = 1,
		kControllerSustainPedal = 64,

		//other controllers here

		kControllerAfterTouch = 128,

		kNumController,
	};


	struct Event;

	struct ClockEvent;

	struct MidiDerivedEvent;	//base type to carry original midi msg

	struct GateEvent;

	struct ControllerEvent;

	struct ProgramChangeEvent;

	struct AutomationEvent;

}




//
//EventStream

class Netx::EventStream : public Core::Stream
{
public:

	REFLEX_OBJECT(Netx::EventStream, Core::Stream);
	NETX_CLASSID(K32("Event"));					//was 3743064467 - kHashSeed



	//declarations	

	class Iterator;



	//lifetime

	EventStream(bool writeable);



	//events

	void Clear();

	template <class TYPE> void Insert(const TYPE & e);

	void Insert(const Event & e);



	//iterate

	Iterator begin() const;

	Iterator end() const;



	//info

	UInt GetSize() const { return m_index.GetSize(); }

	explicit operator bool() const { return True(m_index); }

	UInt GetFlags() const { return m_flags; }



private:

	friend class Iterator;


	void OnClear() override;

	void OnCopy(const Stream & src) override;


	UInt m_lastpos;

	UInt m_flags;

	Array < Pair <UInt> > m_index;	//time position / data position

	Data::Archive m_data;

};




//
//Event

struct Netx::Event
{
	Event(UInt8 size, UInt8 type, UInt16 position);

	UInt8 size;

	UInt8 type;

	UInt16 position;
};




//
//ClockEvent

struct Netx::ClockEvent : public Event
{
	ClockEvent(UInt position, Float bps, UInt songpos, Float64 samplepos);

	Key32 source;

	Float32 bps;

	UInt32 songpos;

	Float64 samplepos;
};




//
//MidiDerivedEvent

struct Netx::MidiDerivedEvent : public Event
{
	using Event::Event;

	UInt32 midi = 0;	//WIP this preserves original midi event from System in and to out
};




//
//GateEvent

struct Netx::GateEvent : public MidiDerivedEvent
{
	GateEvent(UInt position = 0, EventType type = kNumEventType, UInt16 packetid = 0, UInt8 note = 0, Float velocity = 0.0f);

	UInt16 packetid;

	UInt8 note;

	UInt8 reserved;

	Float32 velocity;
};




//
//ControllerEvent

struct Netx::ControllerEvent : public MidiDerivedEvent
{
	ControllerEvent(UInt position, Controller id, Float value);

	Controller controller;

	Float32 value;
};




//
//ProgramChangeEvent

struct Netx::ProgramChangeEvent : public MidiDerivedEvent
{
	ProgramChangeEvent(UInt position, UInt16 program);

	UInt16 program;
};




//
//AutomationEvent

struct Netx::AutomationEvent : public Event
{
	AutomationEvent(UInt position, Key32 paramsetid, UInt16 index, Value32 value);

	Key32 paramsetid;

	UInt16 index;

	Value32 value;
};




//
//EventStream::Iterator

class Netx::EventStream::Iterator
{
public:

	Iterator();

	Iterator(const EventStream & eventstream, UInt idx);


	const Event & operator*() const;

	bool operator==(const Iterator & value) const;

	bool operator!=(const Iterator & value) const;

	void operator++();



private:

	const Pair <UInt> * m_position;

	const UInt8 * m_pdata;

};




//
//impl

REFLEX_NS(Netx)
constexpr UInt kEventStreamAlignment = 8;
REFLEX_END

REFLEX_INLINE void Netx::EventStream::Clear()
{
	MarkAsWrittenRt(false);

	m_lastpos = 0;
	m_flags = 0;

	m_index.Clear();
	m_data.Clear();
}

template <class TYPE> REFLEX_INLINE void Netx::EventStream::Insert(const TYPE & e)
{
	REFLEX_STATIC_ASSERT(alignof(TYPE) <= 8);
	Insert(Cast<Event>(e));
}

REFLEX_INLINE Netx::Event::Event(UInt8 size, UInt8 type, UInt16 position)
	: size(size)
	, type(type)
	, position(position)
{
}

REFLEX_INLINE Netx::ClockEvent::ClockEvent(UInt position, Float bps, UInt songpos, Float64 samplepos)
	: Event(sizeof(ClockEvent), kEventTypeClock, UInt16(position))
	, bps(bps)
	, songpos(songpos)
	, samplepos(samplepos)
{
}

REFLEX_INLINE Netx::GateEvent::GateEvent(UInt position, EventType type, UInt16 packetid, UInt8 note, Float velocity)
	: MidiDerivedEvent(sizeof(GateEvent), type, UInt16(position))
	, packetid(packetid)
	, note(note)
	, reserved(0)
	, velocity(velocity)
{
}

REFLEX_INLINE Netx::ControllerEvent::ControllerEvent(UInt position, Controller controller, Float value)
	: MidiDerivedEvent(sizeof(ControllerEvent), kEventTypeController, UInt16(position))
	, controller(controller)
	, value(value)
{
}

REFLEX_INLINE Netx::ProgramChangeEvent::ProgramChangeEvent(UInt position, UInt16 program)
	: MidiDerivedEvent(sizeof(ProgramChangeEvent), kEventTypeProgramChange, UInt16(position))
	, program(program)
{
}

REFLEX_INLINE Netx::AutomationEvent::AutomationEvent(UInt position, Key32 paramsetid, UInt16 index, Value32 value)
	: Event(sizeof(AutomationEvent), kEventTypeAutomation, UInt16(position))
	, paramsetid(paramsetid)
	, index(index)
	, value(value)
{
}

REFLEX_INLINE Netx::EventStream::Iterator::Iterator()
	: m_position(0)
	, m_pdata(0)
{
}

REFLEX_INLINE Netx::EventStream::Iterator::Iterator(const EventStream & eventstream, UInt idx)
	: m_position(eventstream.m_index.GetData() + idx)
	, m_pdata(eventstream.m_data.GetData())
{
}

REFLEX_INLINE const Netx::Event & Netx::EventStream::Iterator::operator*() const
{
	return *Reinterpret<Event>(m_pdata + m_position->b);
}

REFLEX_INLINE bool Netx::EventStream::Iterator::operator==(const Iterator & value) const
{
	return m_position == value.m_position;
}

REFLEX_INLINE bool Netx::EventStream::Iterator::operator!=(const Iterator & value) const
{
	return m_position != value.m_position;
}

REFLEX_INLINE void Netx::EventStream::Iterator::operator++()
{
	++m_position;
}

Netx::EventStream::Iterator inline Netx::EventStream::begin() const { return Iterator(*this, 0); }

Netx::EventStream::Iterator inline Netx::EventStream::end() const { return Iterator(*this, m_index.GetSize()); }
