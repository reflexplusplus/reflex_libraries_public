#pragma once

#include "../[require].h"




//
//Detail

REFLEX_NS(Netx::Detail)

struct PresetHeader
{
	static constexpr UInt32 kMagicBytes = ID32("NetX");

	UInt32 magic = 0;
	UInt8 netxversion = 0;
	UInt8 reserved1 = 0;
	UInt16 reserved2 = 0;
	UInt32 classid = 0;
	UInt32 chunksize = 0;
	//chunk...
};

struct PresetState
{
	explicit operator bool() const { return header.magic == PresetHeader::kMagicBytes && header.netxversion == 3 && header.chunksize == chunk.GetSize(); }

	Reference <System::FileHandle> file_handle;

	PresetHeader header;

	Data::Archive chunk;
};


bool CheckPreset(File::VirtualFileSystem::Lock & lock, ClassID classid, const WString::View & path);


PresetState OpenPreset(File::VirtualFileSystem::Lock & lock, const WString::View & path, bool writeable = false);	//todo null filesystem that supports file locator

bool SavePreset(PresetState & preset);



//advanced

bool ReadPresetHeader(System::FileHandle & file_handle, PresetHeader & header);

bool ReadPresetBody(System::FileHandle & file_handle, PresetState & preset);	//ReadPresetHeader should already been called and magic bytes VERIFIED


bool WritePreset(System::FileHandle & file_handle, ClassID classid, Data::Archive && chunk);


inline Data::Archive PackComponentChunk(UInt16 version, const Data::Archive::View & chunk)
{
	Data::Archive stream;

	Data::Serialize(stream, chunk.size + 2, version);	
	
	stream.Append(chunk);

	return stream;
}

inline Tuple <UInt16, Data::Archive::View> UnpackComponentChunk(Data::Archive::View stream)
{
	Tuple <UInt16,Data::Archive::View> parts;
	
	Data::Deserialize(stream, parts.b.size, parts.a);
	
	parts.b.size -= 2;

	parts.b.data = stream.data;

	return parts;
}



//ChunkFormat, allows reading of component chunks

struct ChunkFormat : public Data::SerializableFormat
{
	bool SupportsType(TypeID type_id) const override { return true; };

	void OnReset(Data::PropertySet & data) const override {}

	DeserializeError OnDeserialize(Data::Archive::View & stream, Data::PropertySet & data, const Data::PropertySet & options) const override;

	void OnSerialize(Data::Archive & stream, const Data::PropertySet & data, const Data::PropertySet & options) const override;
};

extern const ChunkFormat kChunkFormat;

REFLEX_END

REFLEX_STATIC_ASSERT(sizeof(Netx::Detail::PresetHeader) == 16);
