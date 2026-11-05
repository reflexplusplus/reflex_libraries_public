#pragma once

#include "../[require].h"




//
//namespace

namespace Netx::Detail
{

	class ExtIn;

}




//
//ExtIn

class Netx::Detail::ExtIn : public Component
{
public:

	NETX_CLASS(Netx::Detail::ExtIn, ComponentType, K32("ExtIn"));

	static ExtIn & null;

	[[nodiscard]] static Unretained <ExtIn> Create(bool plugin);



	//pins

	virtual void SetEventPinFilter(UInt idx, bool port_filter, UInt8 port, bool channel_filter, UInt8 channel) = 0;


	
	//transport (ext sync == off)
	
	virtual void EnableExternalSync(bool enable = true) = 0;

	virtual bool ExternalSyncEnabled() const = 0;


	virtual void SetTransport(Optional <bool> play, Optional <UInt> songpos = {}, Optional <Float> bps = {}) = 0;	//synchronised in audio thread

	virtual bool IsPlaying() const = 0;

	virtual const bool & IsPlayingMt() const = 0;

	virtual const Float32 & GetBpsMt() const = 0;

	virtual const UInt32 & GetSongPositionMt() const = 0;



	//monitor
	
	[[nodiscard]] virtual Unretained <Object> CreateTransportListener(const Function <void(bool,Float32)> & callback) = 0;

	[[nodiscard]] virtual Unretained <Object> CreateMidiListener(const Function <void(UInt32)> & callback) = 0;


	
	//prepare processing
	
	virtual void Bind(Access, ConstRef <System::AudioPlugin::EventBuffer> events, const ArrayView <const Float*> & audio) = 0;



protected:

	ExtIn();
	
};
