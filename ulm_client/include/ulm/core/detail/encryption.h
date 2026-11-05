#pragma once

#include "../defines.h"




//
//declarations

REFLEX_NS(ULM::Detail)

//not secure, xor based 'masking'

void XorEncryptDecrypt(const Data::Archive::View & key, const Data::Archive::Region & region);

void XorEncryptDecrypt32(UInt32 key, Data::Archive & region);

void XorEncryptDecrypt64(UInt64 key, Data::Archive & region);

REFLEX_END
