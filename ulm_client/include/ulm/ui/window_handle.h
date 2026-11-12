#pragma once

#include "[require].h"




//
//Secondary API

namespace ULM
{

	class WindowHandle;


	REFLEX_DECLARE_KEY32(dialog);

	AlreadyRetained <WindowHandle> ShowDialogWindow(GLX::Object & owner, const WString::View & title, WillRetain <GLX::Object> content, Key32 id = kdialog);

	void CloseDialogWindow(GLX::Object & owner, Key32 id = kdialog);

	WindowHandle * QueryDialogWindow(GLX::Object & owner, Key32 id = kdialog);

}




//
//WindowHandle

class ULM::WindowHandle : public Object
{
public:

	REFLEX_OBJECT(ULM::WindowHandle, Object);

	
	
	//lifetime
	
	WindowHandle(const WString::View & title, bool top_most, WillRetain <GLX::Object> content, const Function <void()> & on_close_request = {});


	
	//config
	
	void EnableLayoutPersistence(File::PersistentPropertySet & prefs, ArrayView <Key32> id);


	
	//access
	
	AlreadyRetained <GLX::Object> GetContent() { return m_content; }


	void Show(GLX::Alignment position = GLX::kAlignmentCenter, bool allow_animations = false);

	void Hide();

	void Close() { GLX::EmitCloseRequest(m_content); }



protected:

	struct LayoutPersistence;

	static UInt8 MakeStyleFlags(GLX::Object & content, bool closeable) { return System::kWindowStyleFrame | System::kWindowStyleResizable | (closeable ? 0 : System::kWindowStyleCloseable); }


	Reference <System::Window> m_window;

	AlreadyRetained <GLX::WindowClient> m_window_client;

	AlreadyRetained <GLX::Object> m_content;

	
	Reference <Object> m_clock, m_persistence;

	bool m_initialized;

};
