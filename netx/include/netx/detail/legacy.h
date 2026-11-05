#pragma once

#include "../module.h"




//
//declarations

REFLEX_NS(Netx::Detail)

struct LegacyImporter : public Reflex::Detail::StaticItem <LegacyImporter>
{
	static LegacyImporter null;

	static ConstAlreadyRetained <LegacyImporter> Get(ClassID classid);


	struct Chunk
	{
		Key32 uid;
		ClassID classid;
		UInt16 version;
		Data::Archive data;
	};

	typedef Array <Chunk> Chunks;

	typedef FunctionPointer <bool(Netx::ClassID classid, UInt32 clientversion, Data::Archive::View & stream, Chunks & chunks)> RepackChunksFn;

	typedef Sequence <UInt64,FunctionPointer<Pair<Key32,Value32>(UInt64,Value32)>> ParameterSetImporter;


	static constexpr UInt32 kParameterKeyMapID = K32("Params");

	static constexpr UInt32 kParameterSetsKeyMapID = K32("ParameterSets");


	LegacyImporter(ClassID classid, RepackChunksFn repackfn) : classid(classid), RepackChunks(repackfn) {}

	const ClassID classid;
	
	RepackChunksFn RepackChunks;

};

UInt32 ConvertLegacyID64(UInt64 id);

ClassID FromLegacyClassKey(UInt64 legacykey);

UInt64 ToLegacyClassKey(ClassID classid);

void RestoreLegacyRegistry(Data::Archive::View & stream, Data::PropertySet & registry, bool _64bit);

Data::Archive RepackLegacyChunk(const Detail::LegacyImporter & importer, UInt netxversion, UInt32 streamcontext, Data::Archive::View & stream);

extern const Data::DecompressionAlgorithm * g_lzo;

REFLEX_END
