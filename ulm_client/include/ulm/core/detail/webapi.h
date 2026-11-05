#pragma once

#include "http.h"
#include "../defines.h"




//
//Detail

REFLEX_NS(ULM::Detail)

REFLEX_DECLARE_KEY32(Login);

extern const Key32 kError;

Data::Archive DecryptBinary(const File::EmbeddedResource & item);

Unretained <Object> CreateHttpRequest(const File::EmbeddedResource & resource, Key32 task, const Data::PropertySet & request, const Function <void(const Data::PropertySet & response)> & response, const Function <void()> & onfail);

REFLEX_END
