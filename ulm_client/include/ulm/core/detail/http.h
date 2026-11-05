#pragma once

#include "../defines.h"

#define HttpRequest _ALibComponentImpl
#define CreateRequest uCreateItem




//
//declarations

REFLEX_NS(ULM::Detail)

class HttpRequest;

typedef Function <void()> Callback;

REFLEX_DECLARE_KEY32(task);
REFLEX_DECLARE_KEY32(auth);
REFLEX_DECLARE_KEY32(email);
REFLEX_DECLARE_KEY32(password);
REFLEX_DECLARE_KEY32(status);
REFLEX_DECLARE_KEY32(data);

Unretained <System::HttpConnection> CreateHttpRequest(bool https, const CString::View & server);

System::HttpConnection::Response Open(System::HttpConnection & http, const CString::View & resource, Array < Pair <CString> > && headers, const Data::Archive::View & post, const Function <bool(UInt, const Data::Archive::View&)> & onprogress, UInt start = 0, UInt length = kMaxUInt32);

REFLEX_END




//
//Detail::HttpRequest

REFLEX_EXTERN_NULL(ULM::Detail::HttpRequest);

class ULM::Detail::HttpRequest : public Reflex::Object
{
public:

	enum Status
	{
		kStatusSending,
		kStatusReceiving,

		kStatusComplete,
		kStatusAborted,
		kStatusFailed,
	};


	
	//lifetime

	static Unretained <HttpRequest> Create(const CString::View & server, const CString::View & resource, Data::Archive && body, const Function <void(bool,const Data::Archive&)> & response, const Function <void(UInt, const Data::Archive&)> & onprogress = {});



	//response

	virtual Status GetStatus() const = 0;

};
