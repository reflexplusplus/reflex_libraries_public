#include "../../include/ulm/ui/window_handle.h"




//
//impl

struct Reflex::Bootstrap::WindowHandle::LayoutPersistence : public Object
{
	LayoutPersistence(WindowHandle & handle, File::PersistentPropertySet & prefs, ArrayView <Key32> path)
		: handle(handle)
		, m_prefs(prefs)
		, m_prefs_listener(prefs.CreateListener([this](File::PersistentPropertySet::Notification n, Key32 context)
		{
			if (n == File::PersistentPropertySet::kNotificationStore)
			{
				Store();
			}
		}))
		, m_path(Left(path, path.size - 1))
		, m_id(path.GetLast())
	{
	}
	
	~LayoutPersistence()
	{
		Store();
	}

	void Store()
	{
		if (!handle.m_initialized) return;

		auto node = Data::AcquirePropertySet(m_prefs, m_path);

		Data::SetBinary(node, m_id, Data::Pack(handle.m_window_client->GetRect()));
	}


	WindowHandle & handle;

	Reference <File::PersistentPropertySet> m_prefs;

	Reference <Object> m_prefs_listener;

	Array <Key32> m_path;

	Key32 m_id;
};

Reflex::Bootstrap::WindowHandle::WindowHandle(const WString::View & title, bool top_most, WillRetain <GLX::Object> content, const Function <void()> & on_close)
	: m_window(System::Window::Create(MakeStyleFlags(True(on_close)), top_most)),
	m_window_client(New<GLX::WindowClient>(false)),
	m_content(content),
	m_initialized(false)
{
	m_window->SetTitle(title);

	m_window_client->SetContent(content);

	if (on_close)
	{
		GLX::BindEventVoid(m_content, GLX::kRequestClose, on_close);
	}
}

void Reflex::Bootstrap::WindowHandle::EnableLayoutPersistence(File::PersistentPropertySet & prefs, ArrayView <Key32> path)
{
	REFLEX_ASSERT(path);	//clients responsiblity

	m_persistence = REFLEX_CREATE(LayoutPersistence, *this, prefs, path);
}

void Reflex::Bootstrap::WindowHandle::Show(GLX::Alignment position, bool allow_animations)
{
	m_clock = GLX::CreateAnimationClock([this, position, allow_animations](Float)
	{
		Reference retain(this);		//SetRect and SetDisplayMode trigger callbacks, so could delete prematurely

		GLX::AnimationScope scope(allow_animations);

		if (SetFiltered(m_initialized, true))
		{
			const auto bounding_rect = GLX::Indent(GLX::Detail::ToFloat(System::GetScreens().GetFirst().size), GLX::MakeMargin(32.0f));

			const auto content_size = GLX::Detail::ComputeContentSize(m_content);

			GLX::Rect rect = { bounding_rect.origin + GLX::Detail::Snap(GLX::Detail::Align(bounding_rect.size, content_size, position), { 1.0f, 1.0f }), content_size };

			if (m_persistence)
			{
				auto persistence = Cast<LayoutPersistence>(m_persistence);

				if (auto binary = Data::GetBinary(Data::GetPropertySet(persistence->m_prefs, persistence->m_path), persistence->m_id))
				{
					Data::Deserialize(binary, rect);

					rect = Detail::ConstrainRectToDisplay(rect, content_size);
				}
			}

			auto initial_mode = System::kWindowDisplayWindowed;
			auto initial_rect = GLX::Detail::ToInt(rect);
			m_window->SetClient(m_window_client, initial_mode, initial_rect);
			m_window->SetRect(GLX::Detail::ToInt(rect));
			m_window->SetDisplayMode(System::kWindowDisplayWindowed);
		}
		else
		{
			m_window->SetDisplayMode(System::kWindowDisplayWindowed);
		}

		m_clock.Clear();
	});
}

void Reflex::Bootstrap::WindowHandle::Hide()
{
	m_clock.Clear();

	m_window->SetDisplayMode(System::kWindowDisplayMinimised); 
}

Reflex::AlreadyRetained <Reflex::Bootstrap::WindowHandle> Reflex::Bootstrap::ShowDialogWindow(GLX::Object & owner, const WString::View & title, WillRetain <GLX::Object> content, Key32 id)
{
	auto window_handle = REFLEX_CREATE(WindowHandle, title, true, content, [&owner, id]()
	{
		CloseDialogWindow(owner, id);
	});

	owner.SetProperty(id, window_handle);

	window_handle->Show();

	return window_handle;
}

void Reflex::Bootstrap::CloseDialogWindow(GLX::Object & owner, Key32 id)
{
	owner.UnsetProperty<WindowHandle>(id);
}

Reflex::Bootstrap::WindowHandle * Reflex::Bootstrap::QueryDialogWindow(GLX::Object & owner, Key32 id)
{
	return owner.QueryProperty<WindowHandle>(id);
}
