#pragma once

#include "../[require].h"




//
//decl

namespace Netx
{

	constexpr UInt32 kMidiFileHeader = 1684558925;				//BitReverse(CC4("MThd"));

	typedef Sequence < UInt32, Array <UInt32> > MidiTrack;		//position,msg

	typedef Array < MidiTrack > MidiTracks;


	Data::Archive EncodeMidiFile(const MidiTracks::View & track, UInt ppq);

	MidiTracks DecodeMidiFile(const Data::Archive::View & archive, UInt ppq);

}