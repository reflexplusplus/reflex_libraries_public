#pragma once

#include "module.h"




//
//Core::Stream

class Netx::Core::Stream : 
	public Reflex::Object,
	public iClass,
	public Protected
{
public:

	REFLEX_OBJECT(Netx::Core::Stream,Object);

	static Stream & null;



	//access
	
	void Reconnect(Access, Module & module) const;	//will triggr OnConnect on dependent modules (used for PDC) TODO: LOGICALLY SHOULD BE ON PIN?



	//rt
	
	template <bool CHECK = true> void ClearRt();

	template <bool CHECK = true> void CopyRt(const Stream & src);

	void MarkAsWrittenRt() { MarkAsWrittenRt(true); }

	bool IsWrittenRt() const { return m_written; }



	//info

	UInt32 GetConnectionID() const { return m_connectionid; }

	const bool writeable;



protected:

	//lifetime
	
	Stream(ClassID classid, bool writeable);

	void MarkAsWrittenRt(bool written);


	
private:

	friend class Network;

	friend class Pin;


	virtual void OnClear() = 0;

	virtual void OnCopy(const Stream & src) = 0;


	bool m_written;

	UInt32 m_connectionid;
	
};




//
//impl

template <bool CHECK> inline void Netx::Core::Stream::ClearRt()
{
	MarkAsWrittenRt(false);

	if constexpr (CHECK)
	{
		if (writeable)
		{
			OnClear();
		}
	}
	else
	{
		REFLEX_ASSERT(writeable);

		OnClear();
	}
}

template <bool CHECK> inline void Netx::Core::Stream::CopyRt(const Stream & src)
{
	if constexpr (CHECK)
	{
		if (writeable && src.classid == classid)
		{
			m_written = true;

			OnCopy(src);
		}
	}
	else
	{
		MarkAsWrittenRt(true);

		REFLEX_ASSERT(src.classid == classid);

		OnCopy(src);
	}
}

inline void Netx::Core::Stream::MarkAsWrittenRt(bool written)
{
	REFLEX_ASSERT(writeable);

	m_written = written;
}
