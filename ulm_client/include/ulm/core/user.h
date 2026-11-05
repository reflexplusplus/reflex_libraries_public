#pragma once

#include "detail/webapi.h"




//
//

REFLEX_NS(ULM)

Unretained <Object> CreateAccount(const CString::View & email, const CString::View & firstname, const CString::View & lastname, const ResultCallback & ondone);

Unretained <Object> Login(const CString::View & email, const CString::View & password, const ResultCallback & callback);

void Logout();


UInt32 GetUserID();

Data::Archive::View GetUserLogin();


Unretained <Object> RequestTrial(const Product & product, Key32 flavour, const ResultCallback & callback);

REFLEX_END
