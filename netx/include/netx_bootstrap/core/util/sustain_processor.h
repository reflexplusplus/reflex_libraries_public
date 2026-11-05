#pragma once

#include "keyboard_state.h"




//
//Primary API

namespace Netx
{

	template <class CLIENT, class CALLBACKS> class SustainProcessor;

}




//
//SustainProcessor

template <class CLIENT, class CALLBACKS>
class Netx::SustainProcessor : public KeyboardState <UInt16,Float>
{
public:

	//read

	bool SustainEnabled() const;



	//write
	
	void EnableSustain(CLIENT & client, UInt position, bool enable);

	
	void Trigger(CLIENT & client, const GateEvent & trigger);

	void Release(CLIENT & client, const GateEvent & release);

	void ReleaseAll(CLIENT & client);



private:

	bool m_sustain = false;

	Flags64 m_sustain_state[2];

};




//
//impl

template <class CLIENT, class CALLBACKS> REFLEX_INLINE void Netx::SustainProcessor<CLIENT,CALLBACKS>::EnableSustain(CLIENT & client, UInt position, bool enable)
{
	if (SetFiltered(m_sustain, enable))
	{
		if (enable)
		{
			auto keyboard = GetState();

			m_sustain_state[0] = keyboard[0];

			m_sustain_state[1] = keyboard[1];
		}
		else
		{
			GateEvent release(position, Netx::kEventTypeGateRelease, 0, 0, 0.0f);

			Iterator itr(m_sustain_state);

			while (auto idx = itr.GetNext())
			{
				UInt note = idx.value;

				auto [packetid,velocity] = GetKeyValue(note);

				release.packetid = packetid;

				release.note = note;

				if (!velocity)
				{
					CALLBACKS::OnNoteOff(client, release);

					KeyboardState::Release(note);
				}
			}

			m_sustain_state[0].Clear();

			m_sustain_state[1].Clear();
		}
	}
}

template <class CLIENT, class CALLBACKS>REFLEX_INLINE bool Netx::SustainProcessor<CLIENT,CALLBACKS>::SustainEnabled() const
{
	return m_sustain;
}

template <class CLIENT, class CALLBACKS> REFLEX_INLINE void Netx::SustainProcessor<CLIENT,CALLBACKS>::Trigger(CLIENT & client, const GateEvent & trigger)
{
	REFLEX_ASSERT(trigger.type == kEventTypeGateTrigger);

	UInt8 note = trigger.note & 127;

	auto [packetid,velocity] = KeyboardState::GetKeyValue(note);

	if (velocity || CheckState(m_sustain_state, note))
	{
		GateEvent release(trigger.position, kEventTypeGateChoke, packetid, note, 0.0f);

		CALLBACKS::OnNoteOff(client, release);
	}
		
	KeyboardState::Trigger(note, trigger.packetid, trigger.velocity);

	SetState(m_sustain_state, note, m_sustain);
	
	CALLBACKS::OnNoteOn(client, trigger);
}

template <class CLIENT, class CALLBACKS> REFLEX_INLINE void Netx::SustainProcessor<CLIENT,CALLBACKS>::Release(CLIENT & client, const GateEvent & release)
{
	REFLEX_ASSERT(release.type == kEventTypeGateRelease || release.type == kEventTypeGateChoke);

	UInt8 note = release.note & 127;

	auto [packetid,velocity] = KeyboardState::GetKeyValue(note);

	if (velocity && !m_sustain)
	{
		GateEvent clone(release.position, EventType(release.type), packetid, note, 0.0f);

		CALLBACKS::OnNoteOff(client, clone);
	}

	KeyboardState::Release<false,true>(note);	//clear velocity, not packetid
}

template <class CLIENT, class CALLBACKS> REFLEX_INLINE void Netx::SustainProcessor<CLIENT,CALLBACKS>::ReleaseAll(CLIENT & client)
{
	GateEvent clone(0, Netx::kEventTypeGateChoke, 0, 0, 0.0f);

	Iterator itr(*this);

	while (itr)
	{
		if (auto idx = itr.GetNext())
		{
			UInt8 note = idx.value;

			auto packetid = KeyboardState::GetKeyValue(note).a;

			clone.packetid = packetid;

			clone.note = note;
			
			CALLBACKS::OnNoteOff(client, clone);

			KeyboardState::Release(note);
		}
	}
}
