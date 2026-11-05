#pragma once

#include "../[require].h"
#include "../functions/debug.h"




//
//Primary API

namespace Netx
{

	class VoiceFx;

}




//
//VoiceFx

class Netx::VoiceFx : public Component
{
public:

	//types

	typedef SIMD::FloatV4 FloatV4;

	class Map;

	struct Type;

	struct Slot;


	struct Callbacks;

	class Processor;


	
	//lifetime

	NETX_COMPONENT(Netx, VoiceFx, 3932205433ul, const Map & map, Netx::ParameterSet & params, UInt offset, Key32 id = kNullKey);

	~VoiceFx();



	//actions (transacted)

	void Enable(bool enable);

	bool Enabled() const;


	void Clear();


	void SetNumSlot(UInt n);

	void SetSlot(UInt idx, const Slot & slot);

	
	ArrayView <Slot> GetSlots() const;



	//page

	ArrayView <Value32> GetPage() const;



	//links

	const ConstAlreadyRetained <Map> map;

	const AlreadyRetained <ParameterSet> params;

	const UInt64 hash;



private:

	friend class Processor;


	void OnRestore(Data::Archive::View & stream, Key32 context) override;

	void OnStore(Data::Archive & stream) const override;


	void UpdateHash();

	void UpdateParameters();


	const UInt m_paramidx;

	UInt8 m_enabled;

	Array <Slot> m_slots;

};




//
//VoiceFx::Type

struct Netx::VoiceFx::Type
{
	UInt32 id32 = 0;

	CString::View name;

	bool stereo_only = false;


	Array < ConstReference <PropertyDef> > parameters;

	Array <WString::View> flag_names;


	UInt m_sizeof;

	FunctionPointer <void(void*, Float, UInt, Callbacks&)> m_constructor;

	FunctionPointer <void(void*)> m_destructor;

};




//
//VoiceFx::Map

class Netx::VoiceFx::Map : public Object
{
public:

	//lifetime

	Map(UInt nparam, UInt maxslots);



	//setup

	template <class TYPE> static void RegisterType(Map & map, UInt32 id32, const CString::View & name);



	//access

	ArrayView <Type> GetTypes() const;



	//info

	const UInt num_slot;

	const UInt num_param;

	const Type null;



private:

	friend class VoiceFx;

	friend class Processor;


	template <class TYPE> struct SizeOf { static const UInt value = sizeof(TYPE) + (16 - (sizeof(TYPE) % 16)); };


	template <class TYPE> static void Construct(void * data, Float sr, UInt flags, Callbacks & callbacks);

	template <class TYPE> static void Destruct(void * data);



	Array <Type> m_types;

};




//
//VoiceFx::Slot

struct Netx::VoiceFx::Slot
{
	UInt32 id32 = 0;

	bool enabled = false;

	UInt16 flags = 0;
};




//
//VoiceFx::Callbacks

struct Netx::VoiceFx::Callbacks
{
	typedef FunctionPointer <void(void*, UInt)> InitVoiceFn;

	typedef FunctionPointer <void(void*, const FloatV4&, const Float*, const FloatV4*)> UpdateFn;

	typedef FunctionPointer <void(void*, FloatV4*, UInt)> ProcessMonoFn;

	typedef FunctionPointer <void(void*, DSP::Stereo<FloatV4>*, UInt)> ProcessStereoFn;


	InitVoiceFn initvoice;

	UpdateFn update;

	ProcessMonoFn processmono;

	ProcessStereoFn processstereo;
};




//
//VoiceFx::Processor

class Netx::VoiceFx::Processor : public Object
{
public:

	//types

	class VoiceBlock;



	//lifetime

	Processor(Netx::Access access, const VoiceFx & voicefx, UInt nvblock, Float sr);

	~Processor();



	//data

	UInt8 * GetVoiceBlockData(UInt voiceblock_index);



	//process

	void Start(UInt8 * vbslots, UInt vidx, const FloatV4 & pitch, const Float * parameters, const FloatV4 * modulation);

	void Update(UInt8 * vbslots, const FloatV4 & pitch, const Float * parameters, const FloatV4 * modulation);

	void Process(UInt8 * vbslots, FloatV4 * buffer, UInt buffersize);

	void Process(UInt8 * vbslots, DSP::Stereo <FloatV4> * buffer, UInt buffersize);



	//info

	bool IsStereoOnly() const { return m_stereo_only; }

	bool Active() const;

	const UInt64 hash;



private:

	ConstReference <Map> m_map;

	UInt m_nvb, m_nfx;

	bool m_active, m_stereo_only;

	UInt m_vb_size;

	Array <UInt8> m_data;

	Array <UInt8*> m_index;

};




//
//VoiceFx::Processor::VoiceBlock

class Netx::VoiceFx::Processor::VoiceBlock : public Callbacks
{
private:

	friend class Processor;

	static void InitVoice(void * clientdata, UInt vidx) {}

	static void Update(void * clientdata, const FloatV4 & pitch, const Float * parameters, const FloatV4 * modulation) {}

	static void ProcessMono(void * clientdata, FloatV4 * mono, UInt samples) {}

	static void ProcessStereo(void * clientdata, DSP::Stereo <FloatV4> * stereo, UInt samples) {}


	FunctionPointer <void(void*)> destructor;

	UInt16 size;

	UInt16 param_offset;

	FloatV4 clientdata;
};




//
//impl

template <class TYPE> inline void Netx::VoiceFx::Map::RegisterType(Map & map, UInt32 id32, const CString::View & name)
{
	auto & type = map.m_types.Push();

	type.id32 = id32;

	type.name = name;

	type.parameters.SetSize(map.num_param);

	TYPE::Register(type.parameters, type.flag_names);


	type.m_sizeof = SizeOf<TYPE>::value;

	type.m_constructor = &Map::Construct<TYPE>;

	type.m_destructor = &Map::Destruct<TYPE>;

	type.stereo_only = True(TYPE::kStereoOnly);
}

template <class TYPE> inline void Netx::VoiceFx::Map::Construct(void * data, Float sr, UInt flags, Callbacks & callbacks)
{
	Reflex::Detail::Constructor<TYPE>::Construct(data, sr, flags, callbacks);
}

template <class TYPE> inline void Netx::VoiceFx::Map::Destruct(void * data)
{
	Reflex::Detail::Constructor<TYPE>::Destruct(*Cast<TYPE>(data));
}

REFLEX_INLINE bool Netx::VoiceFx::Processor::Active() const
{
	return m_active;
}

REFLEX_INLINE UInt8 * Netx::VoiceFx::Processor::GetVoiceBlockData(UInt vb)
{
	return m_index[vb];
}

REFLEX_INLINE void Netx::VoiceFx::Processor::Start(UInt8 * data, UInt vidx, const FloatV4 & pitch, const Float * parameters, const FloatV4 * modulation)
{
	REFLEX_LOOP(idx, m_nfx)
	{
		auto & item = *Reinterpret<VoiceBlock>(data);

		UInt offset = item.param_offset;

		(*item.update)(&item.clientdata, pitch, parameters + offset, modulation + offset);

		(*item.initvoice)(&item.clientdata, vidx);

		data += item.size;
	}
}

REFLEX_INLINE void Netx::VoiceFx::Processor::Update(UInt8 * data, const FloatV4 & pitch, const Float * parameters, const FloatV4 * modulation)
{
	REFLEX_LOOP(idx, m_nfx)
	{
		auto & item = *Reinterpret<VoiceBlock>(data);

		UInt offset = item.param_offset;

		(*item.update)(&item.clientdata, pitch, parameters + offset, modulation + offset);

		data += item.size;
	}
}

REFLEX_INLINE void Netx::VoiceFx::Processor::Process(UInt8 * data, FloatV4 * buffer, UInt samples)
{
	REFLEX_LOOP(idx, m_nfx)
	{
		auto & item = *Reinterpret<VoiceBlock>(data);

		(*item.processmono)(&item.clientdata, buffer, samples);

#ifdef NETX_HAS_DSP
		CheckBuffer(buffer, samples);
#endif

		data += item.size;
	}
}

REFLEX_INLINE void Netx::VoiceFx::Processor::Process(UInt8 * data, DSP::Stereo <FloatV4> * buffer, UInt samples)
{
	REFLEX_LOOP(idx, m_nfx)
	{
		auto & item = *Reinterpret<VoiceBlock>(data);

#ifdef NETX_HAS_DSP
		CheckBuffer(buffer, samples);
#endif
		(*item.processstereo)(&item.clientdata, buffer, samples);

#ifdef NETX_HAS_DSP
		CheckBuffer(buffer, samples);
#endif

		data += item.size;
	}
}

namespace Netx
{

	const VoiceFx::Type * QueryVoiceFxType(const VoiceFx::Map & map, UInt32 id32, const VoiceFx::Type * fallback = nullptr);

	VoiceFx::Slot GetVoiceFxSlot(const VoiceFx & voicefx, UInt idx);

	void SetVoiceFxSlot(VoiceFx & voicefx, UInt idx, const VoiceFx::Slot & slot);

}