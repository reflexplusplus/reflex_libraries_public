#pragma once

#include "../[require].h"




//
//Secondary API

namespace Netx::Detail
{

	class RefreshClock;

}




//
//ClockScope

class Netx::Detail::RefreshClock
{
public:

	class Item;

	RefreshClock() = default;

	RefreshClock(const RefreshClock &) = delete;

	RefreshClock(RefreshClock &&) = delete;

	~RefreshClock();



protected:

	void AttachNetwork(Network & network);

	void DetachNetwork();

	bool IsAttached() const { return m_network != nullptr; }



private:

	friend class Item;


	List <Item,false> m_clocks;

	Network * m_network = nullptr;

};




//
//ScopedClock

class Netx::Detail::RefreshClock::Item : public Reflex::Item <RefreshClock::Item, false>
{
public:

	static Item & null;

	using Callback = FunctionPointer <bool(void *)>;


	Item();

	Item(RefreshClock & scope, void * client, Callback callback);

	template <class CLIENT, class CALLBACK> Item(RefreshClock & scope, CLIENT * client, CALLBACK callback);

	Item(const Item &) = delete;

	Item(Item &&) = delete;

	~Item();


	void Stop();

	bool IsActive() const;



private:

	using ItemBase = Reflex::Item <RefreshClock::Item, false>;

	friend ItemBase;

	friend RefreshClock;


	template <class CLIENT, class CALLBACK> static Callback MakeCallback(CALLBACK callback);

	static bool Dispatch(void * client);

	void OnAttach(List & list);

	void OnDetach(List & list);


	using ItemBase::Attach;

	using ItemBase::Detach;


	RefreshClock * m_scope = nullptr;

	void * m_client;

	Callback m_callback;

};




//
//impl

inline Netx::Detail::RefreshClock::~RefreshClock()
{
	DetachNetwork();

	while (auto clock = m_clocks.GetLast())
	{
		clock->Stop();
	}
}

inline void Netx::Detail::RefreshClock::AttachNetwork(Network & network)
{
	if (m_network == &network) return;

	DetachNetwork();

	m_network = &network;

	for (auto & clock : m_clocks)
	{
		network.ScheduleCallback(&clock, &Item::Dispatch);
	}
}

inline void Netx::Detail::RefreshClock::DetachNetwork()
{
	if (auto network = m_network)
	{
		m_network = nullptr;

		for (auto & clock : m_clocks)
		{
			network->CancelCallback(&clock);
		}
	}
}

inline Netx::Detail::RefreshClock::Item::Item()
	: m_scope(nullptr)
	, m_client(nullptr)
	, m_callback(nullptr)
{
}

inline Netx::Detail::RefreshClock::Item::Item(RefreshClock & scope, void * client, Callback callback)
	: m_scope(&scope)
	, m_client(client)
	, m_callback(callback)
{
	ItemBase::Attach(scope.m_clocks);
}

template <class CLIENT, class CALLBACK> inline Netx::Detail::RefreshClock::Item::Item(RefreshClock & scope, CLIENT * client, CALLBACK callback)
	: m_scope(&scope)
	, m_client(client)
	, m_callback(MakeCallback<CLIENT>(callback))
{
	ItemBase::Attach(scope.m_clocks);
}

inline Netx::Detail::RefreshClock::Item::~Item()
{
	Stop();
}

inline void Netx::Detail::RefreshClock::Item::Stop()
{
	ItemBase::Detach();
}

inline bool Netx::Detail::RefreshClock::Item::IsActive() const
{
	return m_scope && m_scope->m_network;
}

template <class CLIENT, class CALLBACK> inline Netx::Detail::RefreshClock::Item::Callback Netx::Detail::RefreshClock::Item::MakeCallback(CALLBACK callback)
{
	FunctionPointer <bool(CLIENT &)> checked = callback;

	return Reflex::Detail::CastFunctionPointer<Callback>(checked);
}

inline bool Netx::Detail::RefreshClock::Item::Dispatch(void * client)
{
	auto & self = *Reinterpret<Item>(client);

	Retain(self);

	bool keep = self.m_callback(self.m_client);

	bool externally_owned = self.GetRetainCount() > 1;

	if ((!keep || !externally_owned) && self.GetList())
	{
		self.Stop();
	}

	bool active = keep && externally_owned && self.IsActive();

	Release(self);

	return active;
}

inline void Netx::Detail::RefreshClock::Item::OnAttach(List &)
{
	if (auto network = m_scope->m_network)
	{
		network->ScheduleCallback(this, &Dispatch);
	}
}

inline void Netx::Detail::RefreshClock::Item::OnDetach(List &)
{
	auto scope = m_scope;

	m_scope = nullptr;

	if (auto network = scope->m_network)
	{
		network->CancelCallback(this);
	}
}
