#pragma once

#include "../defines.h"
#include "../core/stream.h"




//
//declarations

template <typename T, UInt N, typename... Ts>
struct TupleArrayImpl : TupleArrayImpl<T, N - 1, T, Ts...> {};

template <typename T, typename... Ts>
struct TupleArrayImpl<T, 0, Ts...>
{
	using type = Tuple<Ts...>;
};

template <typename T, UInt N> using TupleArray = typename TupleArrayImpl<T, N>::type;





//
//Secondary API

namespace Netx
{

	template <UInt NUMCHANNEL, UInt CLASSID, class SAMPLE = Float32> class AudioStream;

}




//
//AudioStream

template <UInt NUMCHANNEL, UInt CLASSID, class SAMPLE>
class Netx::AudioStream : public Core::Stream
{
public:

	typedef AudioStream Self;

	REFLEX_OBJECT(AudioStream, Core::Stream);
	NETX_CLASSID(CLASSID);



	//types

	typedef SAMPLE SampleType;

	using ChannelRegion = TupleArray <SampleType*, NUMCHANNEL>;

	using ChannelView = TupleArray <const SampleType*, NUMCHANNEL>;



	//lifetime
	
	[[nodiscard]] static Unretained <Core::Stream> Create(const Data::PropertySet & properties, bool writeable);



	//clear

	void Clear();



	//access

	SampleType * WriteChannel(UInt idx);

	ChannelRegion WriteChannels();

	const SampleType * ReadChannel(UInt idx) const;

	ChannelView ReadChannels() const;


	
	//pdc

	void SetProcessingDelay(Access, Module & module, UInt samples);

	UInt GetProcessingDelay() const;



	//copy

	AudioStream & operator=(const AudioStream & value);



	//properties

	const UInt length;




protected:

	AudioStream(bool writeable, UInt length, UInt channel_length_bytes, UInt total_length_bytes);



private:

	friend Reflex::Detail::Constructor <AudioStream>;

	void OnClear() override { Clear(); }

	void OnCopy(const Stream & src) override { operator=(Cast<AudioStream>(src)); }


	const UInt m_bytesize;

	UInt m_processingdelay;

	ChannelRegion m_channels;

	alignas(sizeof(SIMD::FloatV4)) SampleType m_data[1];
};

namespace Reflex { template <UInt NUMCHANNEL, Netx::ClassID CLASSID, class SAMPLE> struct IsAbstract < Netx::AudioStream <NUMCHANNEL, CLASSID, SAMPLE>> { static const bool value = true; }; }




//
//impl

REFLEX_NS(Netx)
UInt32 GetMaxBufferSize(const Data::PropertySet & variables);
REFLEX_END

template <UInt NUMCHANNEL, Netx::ClassID CLASSID, class SAMPLE> Unretained <Netx::Core::Stream> inline Netx::AudioStream<NUMCHANNEL,CLASSID,SAMPLE>::Create(const Data::PropertySet & properties, bool writeable)
{
	auto maxbuffersize = GetMaxBufferSize(properties);

	auto bytes = maxbuffersize * kSizeOf<SAMPLE>;

	auto bytes_aligned16 = (bytes + 15) & ~15;

	auto total_bytes = bytes_aligned16 * NUMCHANNEL;

	return Reflex::Detail::Constructor<AudioStream>::CreateVariableSize(g_default_allocator, total_bytes, writeable, maxbuffersize, bytes_aligned16, total_bytes);
}

template <UInt NUMCHANNEL, Netx::ClassID CLASSID, class SAMPLE> inline Netx::AudioStream<NUMCHANNEL,CLASSID,SAMPLE>::AudioStream(bool writeable, UInt length, UInt channel_length_bytes, UInt total_length_bytes)
	: Core::Stream(CLASSID, writeable)
	, length(length)
	, m_bytesize(total_length_bytes)
	, m_processingdelay(0)
{
	MemClear(m_data, total_length_bytes);

	auto data = Reinterpret<UInt8>(m_data);

	REFLEX_LOOP(idx, NUMCHANNEL)
	{
		(&m_channels.a)[idx] = Reinterpret<SampleType>(data);

		data += channel_length_bytes;
	}
}

template <UInt NUMCHANNEL, Netx::ClassID CLASSID, class SAMPLE> REFLEX_INLINE SAMPLE * Netx::AudioStream<NUMCHANNEL,CLASSID,SAMPLE>::WriteChannel(UInt idx)
{
	MarkAsWrittenRt(true);

	return (&m_channels.a)[idx];
}

template <UInt NUMCHANNEL, Netx::ClassID CLASSID, class SAMPLE> REFLEX_INLINE TupleArray <SAMPLE*,NUMCHANNEL> Netx::AudioStream<NUMCHANNEL, CLASSID, SAMPLE>::WriteChannels()
{
	MarkAsWrittenRt(true);

	return m_channels;
}

template <UInt NUMCHANNEL, Netx::ClassID CLASSID, class SAMPLE> REFLEX_INLINE const SAMPLE * Netx::AudioStream<NUMCHANNEL,CLASSID,SAMPLE>::ReadChannel(UInt idx) const
{
	return (&m_channels.a)[idx];
}

template <UInt NUMCHANNEL, Netx::ClassID CLASSID, class SAMPLE> REFLEX_INLINE TupleArray <const SAMPLE *, NUMCHANNEL> Netx::AudioStream<NUMCHANNEL, CLASSID, SAMPLE>::ReadChannels() const
{
	return Reinterpret<ChannelView>(m_channels);
}

template <UInt NUMCHANNEL, Netx::ClassID CLASSID, class SAMPLE> REFLEX_INLINE void Netx::AudioStream<NUMCHANNEL,CLASSID,SAMPLE>::SetProcessingDelay(Access access, Module & module, UInt samples)
{
	if (SetFiltered(m_processingdelay, samples))
	{
		Reconnect(access, module);
	}
}

template <UInt NUMCHANNEL, Netx::ClassID CLASSID, class SAMPLE> REFLEX_INLINE UInt Netx::AudioStream<NUMCHANNEL,CLASSID,SAMPLE>::GetProcessingDelay() const
{
	return m_processingdelay;
}

template <UInt NUMCHANNEL, Netx::ClassID CLASSID, class SAMPLE> inline void Netx::AudioStream<NUMCHANNEL,CLASSID,SAMPLE>::Clear()
{
	MarkAsWrittenRt(false);

	MemClear(m_data, m_bytesize);
}

template <UInt NUMCHANNEL, Netx::ClassID CLASSID, class SAMPLE> inline Netx::AudioStream <NUMCHANNEL,CLASSID,SAMPLE> & Netx::AudioStream<NUMCHANNEL,CLASSID,SAMPLE>::operator=(const AudioStream & value)
{
	MarkAsWrittenRt(value.IsWrittenRt());

	MemCopy(value.m_data, m_data, Min(value.m_bytesize, m_bytesize));

	return *this;
}
