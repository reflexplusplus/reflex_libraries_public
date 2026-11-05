#pragma once

#include "zone.h"




//
//Group

class SamplerV3::Group : 
	public Item <Group>,
	public Zone::List,
	public State,
	public Detail::MetaData
{
public:

	//ids

	REFLEX_DECLARE_KEY32(VTime);
	REFLEX_DECLARE_KEY32(Granular);
	REFLEX_DECLARE_KEY32(Vintage);


	typedef Array < Tuple < Key32, ConstReference <Netx::PropertyDef> > > EnginePropertyDefs;

	enum MainParameters { kMainPitch, kMainCustom0 };
	enum EngineParameters { kEngineA, kEngineB, kEngineC, kNumEngineParam };
	enum MixParameters { kMixGain, kMixPan, kMixSend1, kMixSend2, kNumMixParam };
	enum EnvParam { kEnvAttack, kEnvHold, kEnvDecay, kEnvSustain, kEnvRelease, kEnvVelocity };
	enum XyParam { kXyX, kXyY, kXyZ, kNumXyParam };
	enum LfoParam { kLfoShape, kLfoPhase, kLfoSpeed, kLfoMult, kNumLfoParam };

	enum Pages { kPageMain, kPageEngine, kPageMix, kPageFx, kPageAmpEnv, kPageXyPad, kPageModEnv1, kPageModEnv2, kPageModEnv3, kPageLfo1, kPageLfo2, kPageLfo3, kNumPage };

	enum LfoSync { kLfoSyncNone, kLfoSyncTempo, kLfoSyncPhase };



	//lifetime
	
	static Unretained <Group> Create(Instrument & instrument);	//transacted

	~Group();

	virtual void Discard() = 0;



	//info

	virtual void SetLabel(const WString & label) = 0;

	virtual WString::View GetLabel() const = 0;



	//config
	
	virtual void SetEngine(Key32 mode) = 0;

	virtual Key32 GetEngine() const = 0;

	virtual EnginePropertyDefs::View GetEngineProperties() const = 0;


	virtual void SetOutput(UInt8 idx) = 0;

	virtual UInt8 GetOutput() const = 0;


	virtual void EnableAuxOutputs(bool enable) = 0;

	virtual bool AuxOutputsEnabled() const = 0;


	virtual void SetAuxOutput(UInt8 idx, bool enable, UInt8 output) = 0;

	virtual Pair <bool,UInt8> GetAuxOutput(UInt8 idx) const = 0;



	//envs

	virtual void EnableAmpEnv(bool enable) = 0;

	virtual bool AmpEnvEnabled() const = 0;


	virtual void EnableAmpEnvSustain(bool sustain) = 0;

	virtual bool AmpEnvSustainEnabled() const = 0;


	virtual void EnableModEnvSustain(UInt idx, bool sustain) = 0;

	virtual bool ModEnvSustainEnabled(UInt idx) const = 0;

	
	virtual void SetLfoSync(UInt idx, LfoSync mode) = 0;

	virtual LfoSync GetLfoSync(UInt idx) const = 0;


	virtual void SetTable(UInt idx, const DSP::Table & table, Float step, bool retrig) = 0;
	
	virtual Tuple <const DSP::Table&,Float,bool> GetTable(UInt idx) const = 0;



	//serialize properties (excludes sub-components)

	virtual void ResetProperties() = 0;

	virtual void DeserializeProperties(Data::Archive::View & stream) = 0;

	virtual void SerializeProperties(Data::Archive & stream) const = 0;



	//components

	const AlreadyRetained <Netx::ParameterSet> parameterset;

	const AlreadyRetained <Netx::VoiceFx> voicefx;

	const AlreadyRetained <Netx::Matrix> matrix;



	//info

	const UInt16 uid;

	const Pair <UInt> current_voice;	//vb idx, vidx



protected:

	//friend Accessor;

	friend Item<Group>::List::Itr;
	friend Item<Group>::List::ConstItr;
	friend Item<Group>::List::ReverseItr;
	friend Item<Group>::List::ConstReverseItr;
	template <class TYPE> friend Idx Reflex::LookupIndex(TYPE && item, Idx fallback);

	Group(Instrument & instrument, UInt16 uid, UInt16 voicefxpage);
	
	template <bool REVERSE, class TYPE> friend AlreadyRetained <TYPE> Reflex::Detail::Traverse(TYPE * &);
		

	void Notify();

};




//
//Trigger

struct SamplerV3::Trigger
{
	UInt16 packetid;

	bool reverse = false;


	Float pitch = 0.0f;

	Float amp = 1.0f;
	
	Float pan = 0.0f;

	
	Float velocity = 1.0f;

	Float polymod1 = 0.0f;	//eg note

	Float polymod2 = 0.0f;	//eg unison
};

