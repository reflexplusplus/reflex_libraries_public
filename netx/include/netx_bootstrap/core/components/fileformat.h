#pragma once

#include "../[require].h"




//
//Primary API

namespace Netx
{

	class FileFormat;

}




//
//FileFormat

class Netx::FileFormat : public Component
{
public:

	NETX_COMPONENT(Netx, FileFormat, K32("Preset"), WString::View ext);

	static FileFormat & null;



	//access
	
	bool Save(const WString::View & filename, bool monolith = false);

	bool Open(const WString::View & filename);

	void SetFilename(Access, const WString::View & filename);

	

	//info

	const WString::View ext;


	
	//properties
	
	const WString filename;

	const bool is_monolith;
	


protected:

	virtual bool OnUnpack(System::FileHandle & file_handle);

	virtual bool OnPack(System::FileHandle & file_handle) const;


	void OnRestore(Data::Archive::View & stream, Key32 context) override;;

	void OnStore(Data::Archive & stream) const override;

	RtConfig OnUpdate(UInt8 flags, const Data::PropertySet & properties, const StreamsView & streams, const RtConfig * previous) override;

	bool OnCommand(Key32 cmd, Data::PropertySet & args) override;


	bool m_flushlocator;	//TODO remove when Update is per component
};





//
//Secondary API

namespace Netx
{

	struct ExternalFileImporter;

}




//
//Importer 

struct Netx::ExternalFileImporter : public Object
{
	using FunctionPointer = FunctionPointer <bool(Module & module, File::ResourcePool::Lock & lock, const WString::View & path, System::FileHandle & instream)>;

	FunctionPointer import = nullptr;

	bool autoreset = true;
};
