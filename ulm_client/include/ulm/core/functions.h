#pragma once

#include "detail/webapi.h"




//
//

REFLEX_NS(ULM)

Unretained <Object> CreateLoginListener(const Callback & callback);

Unretained <Object> CreateActivationStatusListener(Product & product, const Function<void(ActivationStatus status, Key32 flavour)> & listener);


ConstAlreadyRetained <Data::PropertySet> GetLicenceInfo(const Product & product);


Unretained <Object> Custom(Key32 msg, Data::PropertySet && request, const Function<void(const Data::PropertySet&)> & ondone, const Callback & onfail = {});


bool ValidateEmail(const CString::View & email);

bool ValidateLicenceCode(const CString::View & key);

bool ValidatePassword(const CString::View & key);

REFLEX_END
