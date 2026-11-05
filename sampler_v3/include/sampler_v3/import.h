#pragma once

#include "instrument.h"




//
//decl

REFLEX_NS(SamplerV3)

bool Import(Netx::Module & module, File::ResourcePool::Lock & lock, const WString::View & filename, System::FileHandle & instream);

REFLEX_END




//
//detail

REFLEX_NS(SamplerV3::Detail)

bool UpdateInstrument(File::ResourcePool::Lock & lock, Instrument & instrument, const Data::PropertySet & node);

REFLEX_END
