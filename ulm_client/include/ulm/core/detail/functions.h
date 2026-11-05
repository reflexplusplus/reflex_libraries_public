#pragma once

#include "../product.h"




//
//declarations

REFLEX_NS(ULM::Detail)

inline WString MakeProductPath(const CString::View & vendor, const CString::View & product, const WString::View & filename)
{
	return Reflex::Bootstrap::Detail::MakeProductPath(System::kPathUserData, vendor, product, filename);
}

UInt32 FromUnixTimestamp(UInt64 unixtimestamp);

UInt64 ToUnixTimestamp(UInt32 ulmtimestamp);

Key32 GetLicenceInfoKey();

REFLEX_END
