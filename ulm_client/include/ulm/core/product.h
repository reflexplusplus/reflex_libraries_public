#pragma once

#include "defines.h"


	

//
//Public API

#define ULM_CALLBACKS(PASS,FAIL) Reflex::Pair<UIntNative>({ULM::Detail::FnEncoder<FAIL,0>::value,ULM::Detail::FnEncoder<PASS,1>::value})




//
//Product

class ULM::Product : public Data::PropertySet
{
public:

	REFLEX_OBJECT(ULM::Product, Data::PropertySet);

	static Product & null;



	//types

	struct TestObject : public Object
	{
		virtual Float32 GetRemainingTime() const = 0;
	};

	class Decoy_;

	typedef volatile Decoy_ Decoy;


	typedef Function <void(TestObject&)> DemoStartCallback;

	typedef Function <void()> DemoEndCallback;

	typedef ResultCallback ActivateCallback;

	typedef Function <void(const Data::Archive::View&)> CreateOfflineRequestCallback;


	static const Float32 kDefaultTestTime;


	
	//lifetime
	
	static Unretained <Product> Register(UInt32 productid, const CString::View & vendor, const CString::View & name, Key32 default_flavour = kStandard);

	static void Enumerate(const Function <void(Product&)> & callback);



	//product info

	virtual UInt32 GetID() const = 0;

	virtual Pair <CString::View> GetName() const = 0;

	virtual Key32 GetDefaultFlavour() const = 0;



	//activation

	virtual void ActivateTemp(Key32 flavour) = 0;	//temporary activation for internal use eg. soundpack encoder

	virtual Unretained <Object> AutoActivate(const ActivateCallback & callback = {}) = 0;	//works if logged in

	virtual Unretained <Object> Activate(const CString::View & email, const CString::View & key, bool pwd, const ActivateCallback & callback = {}) = 0;

	virtual Unretained <Object> CreateOfflineRequest(const CString::View & email, const CString::View & key, bool pwd, const CreateOfflineRequestCallback & callback) = 0;

	virtual Unretained <Object> SubmitOfflineResponse(const Data::Archive::View & response, const ActivateCallback & callback) = 0;

	virtual Unretained <Object> Deactivate(const ActivateCallback & callback) = 0;

	virtual Unretained <Object> Validate(const ActivateCallback & callback) = 0;



	//demo (returns a object which you must retain for onstart to be called.  onstart returns a TestObject)
	
	virtual Unretained <Object> StartDemo(Key32 flavour, const DemoStartCallback & onstart, const DemoEndCallback & timeout) = 0;


	
	//asynchronous validation (secure, use for copy-protection)

	virtual Unretained <Object> CreateTest(ActivationStatus status, void * client, Pair <UIntNative> passfail, Float32 mintime = kDefaultTestTime, const Desc & debug = {}) = 0;



	//synchronous validation (do *not* use for primary copy-protection) 

	virtual void Test(ActivationStatus status, void * client, Pair <UIntNative> passfail) = 0;



	//info (do not use for copy-protection!)

	virtual Pair <ActivationStatus,Key32> GetActivationStatus() const = 0;


	
	//preferences

	const AlreadyRetained <File::PersistentPropertySet> prefs;



protected:

	Product(File::PersistentPropertySet & prefs);

};

REFLEX_SET_TRAIT(ULM::Product, IsSingleThreadExclusive);

REFLEX_EXTERN_NULL(ULM::Product::TestObject);
REFLEX_SET_TRAIT(ULM::Product::TestObject, IsSingleThreadExclusive);




//
//Decoy

class ULM::Product::Decoy_
{
public:

	REFLEX_INLINE Decoy_()
	{
		if (st_test[st_count & 7])
		{
			SelfDestruct(0);
		}
	}

	REFLEX_INLINE ~Decoy_()
	{
		st_count = st_count + 1;
	}

	static void SelfDestruct(void * self);



private:

	static volatile UInt32 st_count;

	static volatile bool st_test[8];

};
