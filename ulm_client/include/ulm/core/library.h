#pragma once

#include "defines.h"




//
//

REFLEX_NS(ULM)

Unretained <Object> Start(const Configuration & configuration);

Unretained <Product> StartWithBootstrap(Bootstrap::Global & global, UInt32 key, const Data::Archive::View & domain, UInt32 productid, Key32 default_flavour = kStandard);

const Configuration & GetConfiguration();

CString GetDomain();

extern const bool & kIsAwake;

REFLEX_END
