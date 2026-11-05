#pragma once

#include "../propertydef.h"




REFLEX_NS(Netx::Detail)

struct PropertyNormaliser
{
	virtual Value32 Clip(const PropertyDef & property, Value32 value) const = 0;

	virtual Value32 NormalToValue(const PropertyDef & property, Float32 value) const = 0;

	virtual Float32 ValueToNormal(const PropertyDef & property, Value32 value) const = 0;
};

extern ConstRef <PropertyNormaliser> kPropertyNormalisers[PropertyDef::kNumType];

REFLEX_END
