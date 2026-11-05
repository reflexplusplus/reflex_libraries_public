#pragma once

#include "detail/metadata.h"




//
//Zone

class SamplerV3::Zone : 
	public Item <Zone>,
	public Detail::MetaData
{
public:
	
	template <class TYPE> using PropertyRef = void;



	//types

	typedef Pair <UInt> Range;

	struct Transaction;



	//lifetime

	static Unretained <Zone> Create(Group & group);

	static Unretained <Zone> CreateFromStream(File::ResourcePool::Lock & lock, Group & group, Data::Archive::View & stream);

	virtual void Discard() = 0;

	

	//transaction
	
	virtual Unretained <Transaction> Begin2() = 0;



	//properties (transacted)

	virtual void SetSample(const DSP::Sample & sample) = 0;

	
	virtual void EnableLoop(bool enable, bool flip, bool during_release) = 0;

	virtual bool LoopEnabled() const = 0;

	virtual bool LoopFlip() const = 0;
	
	virtual bool LoopDuringRelease() const = 0;


	virtual void SetRegion(bool loop, Range region) = 0;

	virtual Range GetRegion(bool loop) const = 0;


	virtual void SetFade(bool loop, Float length) = 0;

	virtual Float GetFade(bool loop) const = 0;



	//serialize

	virtual void Deserialize(File::ResourcePool::Lock & lock, Data::Archive::View & stream) = 0;	//(transacted)

	virtual void Serialize(File::ResourcePool::Lock & lock, Data::Archive & stream) const = 0;



	//info
	
	virtual ConstAlreadyRetained <Object> GetEnginexData() const = 0;
	

	
	//links

	const AlreadyRetained <Group> group;
	
	const UInt32 uid;	//persistent

	const ConstAlreadyRetained <DSP::Sample> sample;



protected:

	Zone(Group & group, UInt16 uid);

};
