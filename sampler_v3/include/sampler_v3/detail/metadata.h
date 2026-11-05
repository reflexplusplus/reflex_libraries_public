#pragma once

#include "../forward.h"




//
//Detail::MetaData 

REFLEX_NS(SamplerV3::Detail)

struct MetaData
{
public:
	const Netx::Value32 * Query(Key32 id, const Netx::Value32 * fallback = nullptr) const;

	Netx::Value32 Get(Key32 id, Netx::Value32 fallback = {}) const;

	void Unset(Key32 id);

	void Set(Key32 id, Netx::Value32 value);



	//links
	
	const AlreadyRetained <Instrument> instrument;




protected:

	MetaData(Instrument & instrument, Key32 actionid, const Data::Archive::View & uid);


	void Reset();

	void Deserialize(Data::Archive::View & stream);

	void Serialize(Data::Archive & stream) const;

	virtual void OnSetProperties() {};



private:

	const Key32 m_actionid;

	const Data::Archive::View m_uid;

	Array < Pair <Key32,Netx::Value32> > m_data;

};

REFLEX_END




//
//impl

inline void SamplerV3::Detail::MetaData::Reset()
{
	m_data.Clear();

	OnSetProperties();
}

inline void SamplerV3::Detail::MetaData::Deserialize(Data::Archive::View & stream)
{
	auto [v, n] = Data::Deserialize<UInt16, UInt16>(stream);

	m_data.SetSize(n);

	auto bytes = n * kSizeOf<UInt64>;

	MemCopy(Data::ReadBytes(stream, bytes).data, m_data.GetData(), bytes);

	OnSetProperties();
}

inline Netx::Value32 SamplerV3::Detail::MetaData::Get(Key32 id, Netx::Value32 fallback) const
{
	return *Query(id, &fallback);
}
