#pragma once

#include "../[require].h"




//
//declarations

REFLEX_NS(Netx::Detail)
	
enum MidiMsg : UInt8
{
	kMidiMsgNoteOff = 8, 
	kMidiMsgNoteOn = 9,
	kMidiMsgPolyAT = 10,
	kMidiMsgControlChange = 11,
	kMidiMsgProgramChange = 12,
	kMidiMsgChannelAT = 13,
	kMidiMsgPitchBend = 14,
};


UInt32 MakeNoteOnMessage(UInt channel, UInt note, Float velocity);

UInt32 MakeNoteOffMessage(UInt channel, UInt note);


void SetMidiMsgChannelAndType(UInt32 & midimsg, UInt8 channel, MidiMsg type);

Pair <UInt8,MidiMsg> GetMidiMsgChannelAndType(UInt32 midimsg);

template <UInt IDX> void SetMidiMsgValue(UInt32 & midimsg, UInt8 value);

template <UInt IDX> UInt8 GetMidiMsgValue(UInt32 midimsg);

REFLEX_END




//
//impl

REFLEX_NS(Netx::Detail)

typedef Tuple <UInt8,UInt8,UInt8,UInt8> MidiMsgData;

REFLEX_END

REFLEX_STATIC_ASSERT(sizeof(Netx::Detail::MidiMsgData) == 4);

REFLEX_INLINE UInt32 Netx::Detail::MakeNoteOnMessage(UInt channel, UInt note, Float velocity)
{
	MidiMsgData msg = { UInt8(channel | (kMidiMsgNoteOn << 4)), UInt8(note), UInt8(Truncate(velocity * 127.0f)), 0 };

	return Reinterpret<UInt32>(msg);
}

REFLEX_INLINE UInt32 Netx::Detail::MakeNoteOffMessage(UInt channel, UInt note)
{
	MidiMsgData msg = { UInt8(channel | (kMidiMsgNoteOff << 4)), UInt8(note), 0, 0 };

	return Reinterpret<UInt32>(msg);
}

REFLEX_INLINE void Netx::Detail::SetMidiMsgChannelAndType(UInt32 & midimsg, UInt8 channel, MidiMsg type)
{
	Reinterpret<UInt8>(&midimsg)[0] = UInt8((channel & 15) | (type << 4));
}

REFLEX_INLINE Pair <UInt8, Netx::Detail::MidiMsg> Netx::Detail::GetMidiMsgChannelAndType(UInt32 midimsg)
{
	auto byte = Reinterpret<UInt8>(&midimsg)[0];

	return { UInt8(byte & 15), MidiMsg((byte & 240) >> 4) };
}

template <UInt IDX> REFLEX_INLINE void Netx::Detail::SetMidiMsgValue(UInt32 & midimsg, UInt8 value)
{
	Reinterpret<UInt8>(&midimsg)[IDX] = value & 127;
}

template <UInt IDX> REFLEX_INLINE UInt8 Netx::Detail::GetMidiMsgValue(UInt32 midimsg)
{
	return Reinterpret<UInt8>(&midimsg)[IDX] & 127;
}
