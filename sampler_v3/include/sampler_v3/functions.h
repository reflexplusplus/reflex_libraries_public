#pragma once

#include "instrument.h"




//
//declarations

REFLEX_NS(SamplerV3)

UInt GetPageOffset(const Instrument & group, Group::Pages page);

ArrayView <Netx::Value32> GetPage(const Group & group, Group::Pages page);


void ClearGroups(Instrument & instrument);

void ClearZones(Group & group);


AlreadyRetained <Group> GetGroupByIndex(Instrument & instrument, UInt idx);


UInt16 ZoneToGroupID(UInt32 zoneid);


Group * QueryGroup(Instrument & instrument, UInt16 uid, Group * fallback = nullptr);

const Group * QueryGroup(const Instrument & instrument, UInt16 uid);


Zone * QueryZone(Instrument & instrument, UInt32 uid);

const Zone * QueryZone(const Instrument & instrument, UInt32 uid);


WString GetLabel(const Group & group);


WString::View GetOutputLabel(UInt idx);


Data::Archive CopySettings(const Group & group);

void PasteSettings(Group & group, const Data::Archive::View & view);


void Reset(Group & group, bool zones);

void Deserialize(Data::Archive::View & stream, Group & group, File::ResourcePool::Lock * lock_for_zones);

void Serialize(Data::Archive & stream, const Group & group, File::ResourcePool::Lock * lock_for_zones);


void StoreMissingTrimAndLoop(Zone & zone, const ArrayView < Pair <UInt32> > trim_loop);	//stores missing region info to properties, for use when resolving samples



//legacy (move to detail)

Key64 MakePropertySetKey(Key32 key);

void ImportPropertySet(Netx::Module & engine, Data::Archive::View & stream);

void ImportPropertySet(Group & group, Data::Archive::View & stream);

REFLEX_END




//
//

REFLEX_NS(SamplerV3)

extern const WString::View kOutputLabels[8];

REFLEX_END

inline UInt SamplerV3::GetPageOffset(const Instrument & instrument, Group::Pages pageid)
{
	return instrument.pages[pageid].a;
}

inline ArrayView <Netx::Value32> SamplerV3::GetPage(const Group & group, Group::Pages pageid)
{
	auto page = group.instrument->pages[pageid];

	return Mid(GetValues(group.parameterset->GetAllocation()), page.a, page.b);
}

inline UInt16 SamplerV3::ZoneToGroupID(UInt32 uid)
{
	return Reinterpret<Pair<UInt16>>(uid).b;
}

inline const SamplerV3::Group * SamplerV3::QueryGroup(const Instrument & instrument, UInt16 uid)
{
	return QueryGroup(RemoveConst(instrument), uid);
}

inline const SamplerV3::Zone * SamplerV3::QueryZone(const Instrument & instrument, UInt32 uid)
{
	return QueryZone(RemoveConst(instrument), uid);
}

inline WString::View SamplerV3::GetOutputLabel(UInt idx)
{
	return kOutputLabels[idx & 7];
}
