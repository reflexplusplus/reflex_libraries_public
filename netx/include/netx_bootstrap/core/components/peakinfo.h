#pragma once

#include "../functions/pin_address.h"




//
//Primary API

namespace Netx
{

	class PeakInfo;

	struct Peaks : public Object
	{
		REFLEX_OBJECT(Netx::Peaks, Object);

		static Peaks & null;

		struct Peak
		{
			DSP::Stereo <Float> peaks;
			DSP::Stereo <bool> clip;
		};

		virtual UInt GetSize() const = 0;	//immutable number of stereo channels. ReadPeaks fills this many entries.

		virtual void SetMultiplier(ArrayView <DSP::Stereo<Float>> value) = 0;

		virtual void ReadPeaks(ArrayRegion <Peak> peaks) const = 0;		//peaks.size should equal GetSize
	};

}




//
//Peakinfo

class Netx::PeakInfo : public Component
{
public:

	NETX_COMPONENT(Netx, PeakInfo, K32("PeakInfo"), Scope, UInt);

	static PeakInfo & null;

	
	static Key32 MakeExternalID(Scope scope, UInt16 idx) { return EncodePinAddress(scope, idx); }

	static Unretained <Component> CreateExternal(Key32 id);


	static constexpr Int32 kProcessOrder = kMaxInt16;


	virtual AlreadyRetained <Peaks> GetPeaks() const = 0;

};
