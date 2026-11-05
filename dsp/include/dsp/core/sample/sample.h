#pragma once

#include "../module.h"
#include "defines.h"




//
//Primary API

REFLEX_NS(DSP)

class Sample;

REFLEX_END




//
//Sample

class DSP::Sample final : 
	public Object,
	public StateMt
{
public:

	REFLEX_OBJECT(DSP::Sample, Object);

	static Sample & null;

	template <class TYPE> using ReferenceType = void;

	using MetaDataItem = Pair <Key32, Value32>;



	//defines

	static constexpr UInt kPadding = 16;

	static const UInt32 kWavDataChunk = ID32("data");// 1635017060ul;

	static const File::ResourcePool::Ctr kResourcePoolCtr;


	typedef FunctionPointer <DSP::Sample*(Key32 id, File::Attributes::Status status, System::FileHandle&)> Loader;

	REFLEX_DECLARE_KEY32(BitDepth);
	REFLEX_DECLARE_KEY32(LoopStart);
	REFLEX_DECLARE_KEY32(LoopLength);
	REFLEX_DECLARE_KEY32(Beats);



	//setup

	static void RegisterFormat(const WString::View & ext, Loader loader, UInt32 magicbytes = 0);

	static Array <WString::View> GetFormats();

	static bool IsRegisteredFormat(Key32 ext);



	//lifetime

	static ConstAlreadyRetained <Sample> Retrieve(File::ResourcePool::Lock & lock, const WString::View & path);	//from ResourcePool, retain it to keep it alive beyong the snapshot
	
	static Sample * Create(Float sr, UInt nchn, UInt length, Key32 id, ArrayView <MetaDataItem> metadata, File::Attributes::Status status = File::Attributes::kStatusReady, Allocator & allocator = g_default_allocator);

	static Sample * Create(Float sr, UInt nchn, UInt length, Key32 id = kNullKey, File::Attributes::Status status = File::Attributes::kStatusReady, Allocator & allocator = g_default_allocator)
	{
		return Create(sr, nchn, length, id, {}, status, allocator);
	}
	
	~Sample();


	
	//state
	
	void Wipe();


	ArrayView <Float*> Write();

	Float * WriteChannel(UInt channel);


	ArrayView <const Float*> Read() const;

	const Float * ReadChannel(UInt channel) const;


	const Value32 * Query(Key32 id, const Value32 * fallback = nullptr) const;

	Value32 Get(Key32 id, Value32 fallback = {}) const;



	//info
	
	const Key32 id;

	const Float samplerate;

	const UInt length;

	const UInt8 nchn;

	const ArrayView <Pair <Key32, Value32>> metadata;

	const File::Attributes::Status status;



protected:

	friend class Reflex::Detail::Constructor <Sample>;
	
	Sample();

	Sample(Float sr, UInt nchn, UInt length, Key32 id, ArrayView <MetaDataItem> metadata, File::Attributes::Status status);

	
	bool m_debug_set;

	Float * m_channels[8];

	alignas(16) Float m_data[1];

};




//
//impl

REFLEX_INLINE ConstAlreadyRetained <DSP::Sample> DSP::Sample::Retrieve(File::ResourcePool::Lock & lock, const WString::View & path)
{
	REFLEX_ASSERT(module.IsInitialised());

	return lock.Retrieve<Sample>(path, Null<Data::PropertySet>(), kResourcePoolCtr);
}

REFLEX_INLINE ArrayView <Float*> DSP::Sample::Write()
{
	m_debug_set = true;

	Notify();

	return { m_channels, nchn };
}

REFLEX_INLINE Float * DSP::Sample::WriteChannel(UInt channel)
{
	m_debug_set = true;

	Notify();

	return m_channels[channel];
}

REFLEX_INLINE ArrayView <const Float*> DSP::Sample::Read() const
{
	return { m_channels, nchn };
}

REFLEX_INLINE const Float * DSP::Sample::ReadChannel(UInt channel) const
{
	return m_channels[channel];
}

REFLEX_INLINE Bootstrap::Value32 DSP::Sample::Get(Key32 id, Bootstrap::Value32 fallback) const
{
	return *Query(id, &fallback);
}
