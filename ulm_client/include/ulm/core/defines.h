#pragma once

#include "[require].h"

#ifdef ULM_3P
#define ULM_DEBUG 0
#else
#define ULM_DEBUG REFLEX_DEBUG
#endif

#if ULM_DEBUG
#define ULM_DESC(x) x
#else
#define ULM amlib										//for symbol obfuscation
#define StartDemo uAddAuxEx
#define CreateOfflineRequest uCompactEx
#define SubmitOfflineResponse uExpandEx
#define ProductImpl _ALibMixerImpl
#define TrialSubscriptionImpl _ALibSum
//#define Check_ _ALibAuxSend
#define Decoy_ uGainDesc

#define XorEncryptDecrypt UpdateCacheState
#define XorEncryptDecrypt32 UpdateCacheState32
#define XorEncryptDecrypt64 UpdateCacheState64
#define DecryptBinary NormalizeInputRange
#define CreateActivationListener EvaluatePathMetric
#define RequestTrial UpdateRuntimeFlags

#define ULM_DESC(x) K32(x)
#endif




//
//types

REFLEX_NS(ULM)

constexpr UInt32 kVersion = 1;	//for static lib integrity


REFLEX_DECLARE_KEY32(ProductInfo);

REFLEX_DECLARE_KEY32(vendor);

REFLEX_DECLARE_KEY32(name);


REFLEX_DECLARE_KEY32(UserID);

REFLEX_DECLARE_KEY32(User);

REFLEX_DECLARE_KEY32(LicenceID);

REFLEX_DECLARE_KEY32(Serial);

REFLEX_DECLARE_KEY32(MachineDescription);

REFLEX_DECLARE_KEY32(ProductID);

REFLEX_DECLARE_KEY32(FlavourID);

REFLEX_DECLARE_KEY32(Type);

REFLEX_DECLARE_KEY32(Expires);

REFLEX_DECLARE_KEY32(ActivationID);


REFLEX_DECLARE_KEY32(Success);
REFLEX_DECLARE_KEY32(ErrorNoConnection);


REFLEX_DECLARE_KEY32(Standard);


typedef UInt32 ActivationStatus;
extern const ActivationStatus & kActivationStatusNone;
extern const ActivationStatus & kActivationStatusDemo;
extern const ActivationStatus & kActivationStatusFull;

enum Result
{
	kResultNoConnection,
	kResultFail,
	kResultSuccess,
};

class Product;

struct Configuration
{
	static Unretained <File::PersistentPropertySet> OpenPrefs(UInt32, const CString::View&, const CString::View&);
	
	static void StorePrefs(Product & product);

	UInt32 domain_encryptionkey;

	Data::Archive::View domain;

	Float32 demo_duration = 3600.0f;				//seconds

	Function <CString()> machine_description;		//this is called during activation, fill with something like { L"Intel i7",  L"Windows 10" }

	FunctionPointer <Unretained<File::PersistentPropertySet>(UInt32, const CString::View&, const CString::View&)> restore_prefs = &OpenPrefs;
	
	FunctionPointer <void(Product&)> store_prefs = &StorePrefs;
};


typedef Array <WString> Message;

typedef Function <void()> Callback;

typedef Function <void(Result, Key32, const Message&)> ResultCallback;

typedef typename Selector<bool(ULM_DEBUG), CString::View, UInt32>::type Desc;


extern const WString::View kNoConnection;

extern const Reflex::File::EmbeddedResource kUserURL;

extern const Reflex::File::EmbeddedResource kProductsURL;

REFLEX_END

namespace ULM::Detail { template <void(*FN)(void *), unsigned X> struct FnEncoder; template <unsigned X> struct FnEncoderKey; }
template <unsigned X> struct ULM::Detail::FnEncoderKey { static inline const size_t value = X ? 923884023 : 4910123; };
template <void(*FN)(void*), unsigned X> struct ULM::Detail::FnEncoder { static inline const size_t value = reinterpret_cast<size_t>(FN) ^ FnEncoderKey<X>::value; };
