#pragma once

#include "../[require].h"




//
//Primary API

namespace Netx
{

	class VoiceManager;

}




//
//VoiceManager

class Netx::VoiceManager : public Component
{
public:

	class VoiceBlock;

	class AbstractProcessor;

	template <class VOICEBLOCK> class Processor;



	//lifetime

	NETX_COMPONENT(Netx, VoiceManager, K32("Netx::VoiceManager"), UInt16 max_polyphony, UInt16 default_polyphony);

	static VoiceManager & null;



	//access

	virtual void SetPolyphony(UInt n) = 0;


	const UInt16 max_polyphony;

	const UInt16 default_polyphony;

	const UInt16 polyphony;

};




//
//VoiceManager::VoiceBlock

class Netx::VoiceManager::VoiceBlock : public Reflex::Item <VoiceBlock,false>
{
public:

	static VoiceBlock & null;

	const UInt index = 0;

	const UInt flags = 0;

	const SIMD::IntV4 packetids = SIMD::kiZero;

	const SIMD::BoolV4 activity = SIMD::kBooleanFalse;



protected:

	virtual void OnWakeRt() {}

	virtual void OnReleaseVoiceRt(UInt vidx) {}

	virtual void OnChokeVoiceRt(UInt vidx) {}

	virtual void OnEndVoiceRt(UInt vidx) {}

	virtual void OnSleepRt() {}

	virtual SIMD::BoolV4 OnCheckActivityRt() const = 0;

};




//
//VoiceManager::AbstractProcessor

class Netx::VoiceManager::AbstractProcessor : public Object
{
public:

	static AbstractProcessor & null;

	struct VoiceRef
	{
		UInt16 processor_uid = 0;
		UInt8 voiceblock_idx = 0;
		UInt8 vidx = 0;	//simd channel
	};



	//mt

	UInt16 GetVoiceCountMt() const;
	
	VoiceRef GetLastVoiceMt() const;


	VoiceBlock * GetVoiceBlockMt(UInt idx);

	const VoiceBlock * GetVoiceBlockMt(UInt idx) const;



	//rt

	VoiceRef CreateVoiceRt(UInt16 packetid, UInt flags);

	void ReleaseVoiceRt(VoiceRef voiceref, bool choke);

	void ReleaseVoicesByPacketRt(UInt16 packetid, bool choke);

	void ReleaseVoicesByValueRt(UIntNative value_v4_offset, Int32 value, Int32 or_mask, bool choke);	//set or_mask to always match bits

	void ReleaseVoicesByVoiceBlockRt(UInt voiceblock_idx, UInt8 flags, bool choke);

	void ReleaseVoicesRt(bool choke);

	void PurgeVoicesRt();

	VoiceBlock * GetFirstActiveVoiceBlockRt() { return m_inactive_active_rt[1].GetFirst(); }



	//access

	const UInt16 uid;

	const UInt16 polyphony;

	const UInt8 nvblock;



protected:

	AbstractProcessor(UInt16 uid, UInt16 stride, UInt16 polyphony, UInt8 nvblock);

	static AbstractProcessor * CreateImpl(VoiceManager & component, UInt16 stride, FunctionPointer <void(VoiceBlock&)> dtr, void * args, FunctionPointer <void(VoiceBlock*, void*)> ctr);

	virtual VoiceRef OnCreateVoiceRt(UInt16 packetid, UInt flags) = 0;

	virtual void OnReleaseVoiceRt(VoiceRef voiceref, bool choke) = 0;

	virtual void OnReleaseVoicesByVoiceBlockRt(UInt voiceblock_idx, UInt8 flags, bool choke) = 0;

	virtual void OnReleaseVoicesByValueRt(UIntNative value_v4_offset, Int32 value, Int32 mask, bool choke) = 0;

	virtual void OnReleaseVoicesRt(bool choke) = 0;

	virtual void OnPurgeVoicesRt() = 0;


	const UInt8 m_vblock_wrap;

	const UInt16 m_stride;

	FunctionPointer <void(VoiceBlock&)> m_destroy_vb;

	VoiceBlock::List m_inactive_active_rt[2];

	UInt16 m_voicecount;

	mutable VoiceRef m_last;

	alignas(sizeof(SIMD::FloatV4)) UInt8 m_pvoiceblocks_mt[1];

};




//
//VoiceManager::Processor

template <class VOICEBLOCKIMPL>
class Netx::VoiceManager::Processor : public VoiceManager::AbstractProcessor
{
public:

	//lifetime

	template <class ... VARGS> [[nodiscard]] static Unretained <Processor> Create(Netx::Access access, VoiceManager & voicemanager, VARGS && ... params);



	//access

	VOICEBLOCKIMPL & operator[](UInt idx) { return *Reinterpret<VOICEBLOCKIMPL>(GetVoiceBlockMt(idx)); }

	const VOICEBLOCKIMPL & operator[](UInt idx) const { return *Reinterpret<VOICEBLOCKIMPL>(GetVoiceBlockMt(idx)); }


	VOICEBLOCKIMPL * begin() 
	{
		REFLEX_ASSERT(sizeof(VOICEBLOCKIMPL) == m_stride || !nvblock);

		return Reinterpret<VOICEBLOCKIMPL>(m_pvoiceblocks_mt); 
	}

	VOICEBLOCKIMPL * end() 
	{
		REFLEX_ASSERT(sizeof(VOICEBLOCKIMPL) == m_stride || !nvblock);

		return Reinterpret<VOICEBLOCKIMPL>(m_pvoiceblocks_mt) + nvblock;
	}

	const VOICEBLOCKIMPL * begin() const
	{
		REFLEX_ASSERT(sizeof(VOICEBLOCKIMPL) == m_stride || !nvblock);

		return Reinterpret<VOICEBLOCKIMPL>(m_pvoiceblocks_mt);
	}

	const VOICEBLOCKIMPL * end() const 
	{
		REFLEX_ASSERT(sizeof(VOICEBLOCKIMPL) == m_stride || !nvblock);

		return Reinterpret<VOICEBLOCKIMPL>(m_pvoiceblocks_mt) + nvblock;
	}



protected:

	typedef VoiceManager::AbstractProcessor Base;

};




//
//impl

REFLEX_INLINE UInt16 Netx::VoiceManager::AbstractProcessor::GetVoiceCountMt() const
{
	return m_voicecount;
}

REFLEX_INLINE Netx::VoiceManager::AbstractProcessor::VoiceRef Netx::VoiceManager::AbstractProcessor::GetLastVoiceMt() const
{
	return m_last;
}

REFLEX_INLINE Netx::VoiceManager::VoiceBlock * Netx::VoiceManager::AbstractProcessor::GetVoiceBlockMt(UInt idx)
{
	return Reinterpret<VoiceBlock>(m_pvoiceblocks_mt + (m_stride * (idx & m_vblock_wrap)));
}

REFLEX_INLINE const Netx::VoiceManager::VoiceBlock * Netx::VoiceManager::AbstractProcessor::GetVoiceBlockMt(UInt idx) const
{
	return Reinterpret<VoiceBlock>(m_pvoiceblocks_mt + (m_stride * (idx & m_vblock_wrap)));
}

REFLEX_INLINE Netx::VoiceManager::AbstractProcessor::VoiceRef Netx::VoiceManager::AbstractProcessor::CreateVoiceRt(UInt16 packetid, UInt flags) 
{
	return OnCreateVoiceRt(packetid, flags); 
}

REFLEX_INLINE void Netx::VoiceManager::AbstractProcessor::ReleaseVoiceRt(VoiceRef voiceref, bool choke) 
{
	//REFLEX_ASSERT(voiceref.processor_uid == uid);

	OnReleaseVoiceRt(voiceref, choke); 
}

REFLEX_INLINE void Netx::VoiceManager::AbstractProcessor::ReleaseVoicesByPacketRt(UInt16 packetid, bool choke)
{
	OnReleaseVoicesByValueRt(REFLEX_OFFSETOF(VoiceBlock, packetids), packetid, 0, choke);
}

REFLEX_INLINE void Netx::VoiceManager::AbstractProcessor::ReleaseVoicesByValueRt(UIntNative value_v4_offset, Int32 value, Int32 or_mask, bool choke)
{
	OnReleaseVoicesByValueRt(value_v4_offset, value, or_mask, choke);
}

REFLEX_INLINE void Netx::VoiceManager::AbstractProcessor::ReleaseVoicesByVoiceBlockRt(UInt voiceblock_idx, UInt8 flags, bool choke) 
{
	OnReleaseVoicesByVoiceBlockRt(voiceblock_idx, flags, choke);
}

REFLEX_INLINE void Netx::VoiceManager::AbstractProcessor::ReleaseVoicesRt(bool choke) 
{
	OnReleaseVoicesRt(choke); 
}

REFLEX_INLINE void Netx::VoiceManager::AbstractProcessor::PurgeVoicesRt() 
{
	OnPurgeVoicesRt();
}

template <class VOICEBLOCKIMPL> template <class ... VARGS> inline Unretained < Netx::VoiceManager::Processor <VOICEBLOCKIMPL> > Netx::VoiceManager::Processor<VOICEBLOCKIMPL>::Create(Netx::Access access, VoiceManager & component, VARGS && ... params)
{
	typedef Reflex::Detail::Constructor <VOICEBLOCKIMPL> Constructor;

	Tuple <VARGS...> param = { params... };

	FunctionPointer <void(const VOICEBLOCKIMPL&)> dtr = &Reflex::Detail::Constructor<VOICEBLOCKIMPL>::Destruct;

	auto base = CreateImpl(component, sizeof(VOICEBLOCKIMPL), Reflex::Detail::CastFunctionPointer<FunctionPointer<void(VoiceBlock&)>>(dtr), &param, [](VoiceBlock * vb, void * param)
	{
		auto & args = *Cast< Tuple <VARGS...> >(param);

		if constexpr (sizeof...(VARGS) == 0)
		{
			Constructor::Construct(vb);
		}
		else if constexpr (sizeof...(VARGS) == 1)
		{
			Constructor::Construct(vb, args.a);
		}
		else if constexpr (sizeof...(VARGS) == 2)
		{
			Constructor::Construct(vb, args.a, args.b);
		}
		else if constexpr (sizeof...(VARGS) == 3)
		{
			Constructor::Construct(vb, args.a, args.b, args.c);
		}
	});

	return Cast<Processor>(base);
}
