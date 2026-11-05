#pragma once

#include "../detail/extin.h"
#include "../components/fileformat.h"




//
//Primary API

namespace Netx
{

	Unretained <Object> CreateModuleListener(WillRetain <Module> module, const Function <void()> &ontransaction);	//module must be attached.  heuristic only: module state changed after Network::Transaction


	void RegisterClassImporter(Network & network, ClassID classid, ClassID imported, const Function <void(Module & module, ClassID classid)> on_import);

	void RegisterFileImporter(Network & network, ClassID classid, Key32 ext, ExternalFileImporter::FunctionPointer import, bool autoreset = true);

	void RegisterNullSample(Network & network);

	AlreadyRetained <Detail::ExtIn> GetExtIn(Network & network);

}
