#pragma once

#include "../util/keyboard_state.h"
#include "../util/pins.h"
#include "../util/parameterset_initialiser.h"
#include "../util/module_base.h"
#include "../components/dynamic_pins.h"




//
//Primary API

namespace Netx
{

	class Plugin;

}




//
//Plugin

class Netx::Plugin : public ModuleBase <Module>
{
public:

	REFLEX_OBJECT(Netx::Plugin, Module);

	static Plugin & null;


	
	//types
	
	enum Format : UInt8
	{
		kFormatNull,
		kFormatVST2,
		kFormatVST3,
		kFormatCLAP,

		kNumFormat
	};

	class DynamicLibrary;

	class Instance;

	using Relocator = Function <WString(Format format, Data::Archive::View uid)>;



	//setup

	static void RegisterRelocator(Network & network, ClassID plugin_classid, const Relocator & callback);



	//lifetime

	Plugin(ClassID class_id, CString::View host_vendor_name, CString::View host_product_name, CString::View host_url = {});

	~Plugin();



	//access

	AlreadyRetained <Instance> Open(const WString & path, Data::Archive::View uid);

	AlreadyRetained <Instance> GetCurrentInstance() { return m_current_instance; }

	ConstAlreadyRetained <Instance> GetCurrentInstance() const { return m_current_instance; }


	
	//pins

	bool SetPins(Pins pins);	//May configure a lightweight proxy before Open; validates loaded instance IO.



	//bypass

	void Enable(bool enable);

	bool Enabled() const { return m_enabled; }



protected:

	void OnRestoreController(Data::Archive::View & stream, Key32 ctx) final;

	void OnStoreController(Data::Archive & stream) const final;

	bool OnCommand(Key32 cmd, Data::PropertySet & args) final;


	CString m_vendor_name, m_product_name, m_host_url;

	Reference <Instance> m_current_instance;
	
	Reference <DynamicPins> m_input;
	
	Reference <DynamicPins> m_output;
	
	bool m_enabled = true;

};




//
//Plugin::DynamicLibrary

class Netx::Plugin::DynamicLibrary : public Object
{
public:

	REFLEX_OBJECT(Netx::Plugin::DynamicLibrary, Object);

	static DynamicLibrary & null;


	typedef FunctionPointer <Unretained<System::DynamicLibrary>(const WString&)> CreateFn;

	typedef FunctionPointer <void(System::DynamicLibrary&)> DeinitFn;


	static Reference <DynamicLibrary> Acquire(File::ResourcePool::Lock & lock, const WString & path);

	[[nodiscard]] static Unretained <Object> Open(const File::ResourcePool::StreamContext & ctx, System::FileHandle & stream);

	DynamicLibrary(const WString & path, Format format);

	~DynamicLibrary();


	const WString path;

	const Format format;

	const AlreadyRetained <System::DynamicLibrary> dll;
};




//
//Plugin::Instance

class Netx::Plugin::Instance : public Component
{
public:

	static Instance & null;



	//types

	using ShellUIDs = Array< Pair <Format, Data::Archive> >;

	using EnumurateFn = FunctionPointer <bool(const System::DynamicLibrary&, ShellUIDs&)>;

	class EditorHandle;

	struct HostInfo
	{
		CString vendor;
		CString product;
		CString url;
	};



	//lifetime

	static void Enumurate(const DynamicLibrary & dll, ShellUIDs & pluginid);

	[[nodiscard]] static Unretained <Instance> Create(const DynamicLibrary & dll, Data::Archive::View plugin_id, const HostInfo & host);

	virtual ~Instance() = default;



	//info

	virtual ConstAlreadyRetained <DynamicLibrary> GetDynamicLibrary() const = 0;

	virtual Data::Archive::View GetPluginID() const = 0;

	virtual bool IsInstrument() const = 0;



	//pins

	virtual Array <Pins> GetSupportedPins() const = 0;

	virtual bool SetPins(Pins pins) = 0;



	//bypass

	virtual void Enable(Access access, bool enable) = 0;

	virtual bool Enabled() const = 0;



	//chunk

	virtual Data::Archive GetChunk() const = 0;

	virtual void SetChunk(const Data::Archive::View & view) = 0;



	//info

	virtual Pair <CString> GetProduct() const = 0;

	virtual Array <CString> GetCategories() const = 0;



	//editor

	[[nodiscard]] virtual Unretained <EditorHandle> AcquireEditor(System::iPoint position, const Function <void()> & on_request_close) = 0;



protected:

	//lifetime

	Instance(Format format);

	virtual void AddParameter(Detail::ParamDefs & paramdefs, WString && name) = 0;	//only during OnInitParameters

	virtual void BeginParameterEdit(UInt index) = 0;

	virtual void EndParameterEdit() = 0;

	virtual void UpdateParameter(UInt idx, Float value) = 0;

	virtual void ScheduleLoadParameters() = 0;

	virtual void PublishLatency(UInt32 samples) = 0;

	using MainThreadFn = void(*)(Instance &);

	virtual void DispatchOnMain(MainThreadFn fn) = 0;



	//callbacks

	virtual void OnInitParameters(Detail::ParamDefs & defs) = 0;

	virtual void OnDestroy() = 0;


	virtual void OnSetChunk(const Data::Archive::View & chunk) = 0;

	virtual void OnGetChunk(Data::Archive & chunk) const = 0;


	virtual void OnGetSupportedPins(Array <Pins> & pins) const = 0;

	virtual bool OnSetPins(Pins pins) = 0;


	virtual void OnSetParameter(UInt idx, Float value) = 0;

	virtual void OnSetParameterRt(UInt idx, Float value) = 0;

	virtual Float OnGetParameter(UInt idx) const = 0;


	virtual WString OnParameterValueToString(UInt idx, Value32 in) = 0;

	virtual Value32 OnParameterStringToValue(UInt idx, WString::View string) { return MakeValue32(ToFloat32(string) / 100.0f); }


	virtual Unretained <Object> OnPrepareProcessing(Pins io, const Module::StreamsView & streams, Float samplerate, UInt blocksize, UInt ppq) = 0;


	virtual void OnGetProduct(CString & vendor, CString & product) const = 0;

	virtual void OnGetCategories(Array <CString> & categories) const = 0;

	virtual System::Window::Client * OnCreateEditor(Object & owner) = 0;


	virtual void CloseEditor() = 0;

};




//
//Plugin::Instance::EditorHandle

class Netx::Plugin::Instance::EditorHandle : public Reflex::SignalComponent<>::Listener
{
public:

	static EditorHandle & null;

	using Listener::Listener;

	virtual AlreadyRetained <System::Window> GetWindow() = 0;
};




//
//impl

inline Reference <Netx::Plugin::DynamicLibrary> Netx::Plugin::DynamicLibrary::Acquire(File::ResourcePool::Lock & lock, const WString & path)
{
	return lock.Retrieve<DynamicLibrary>(path, Data::PropertySet::null, &DynamicLibrary::Open);
}
