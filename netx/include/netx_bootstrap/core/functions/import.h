#pragma once

#include "../[require].h"




namespace Netx
{

	bool ImportPropertySheet(File::ResourcePool::Lock & lock, Module & module, const WString::View & filename, System::FileHandle & instream, Key32 format_id, FunctionPointer<bool(File::ResourcePool::Lock & lock, const WString::View & filename, Module & module, const Data::PropertySet & args)> decoder);

}