#include "../../include/ulm/ui/notifications.h"




REFLEX_BEGIN_INTERNAL(ULM)

struct Notifications : public GLX::Object
{
	Notifications()
	{
		GLX::EnableMouse(*this, false);

		GLX::EnableAutoFit(*this, false, false);

		GLX::EnableMouse(m_list, false);

		GLX::SetFlow(m_list, GLX::kFlowY);


		GLX::AddFloat(*this, m_list, GLX::kAlignmentBottomLeft);

		EnableOnClock();
	}

	void OnSetStyle(const GLX::Style & style) override
	{
		m_item_style = style[GLX::kitem];
	}

	void OnClock(Float delta) override
	{
		if (m_list.Empty()) Object::Detach();
	}

	void AddItem(const WString & msg)
	{
		auto item = GLX::AddInline(m_list, New<GLX::Object>(), GLX::kOrientationNear);

		GLX::SetFlow(item, GLX::kFlowY);

		GLX::EnableMouse(item, false);

		auto text = AddInline(item, Init(New<GLX::Label>(msg), m_item_style));

		GLX::EnableMouse(text, false);

		GLX::SetOpacity(item, kNullKey, 0.0f, GLX::Detail::ComputedStyle::kRenderFalse);

		auto h = GLX::Detail::ComputeContentSize(item).h;

		GLX::SetBounds(item, kNullKey, {}, MakeSize(GLX::kLarge.w, 0.0f));


		auto enter = New<GLX::Multi>();

		{
			GLX::AddScene(enter, GLX::CreateMaxBoundsAnimation(kNullKey, true, 0.0f, h));

			GLX::AddScene(enter, GLX::CreateOpacityAnimation(kNullKey, 0.0f, 1.0f))->SetTime(0.5f);
		}

		auto playlist = New<GLX::PlayList>();

		{
			GLX::AddScene(playlist, enter);

			GLX::AddScene(playlist, GLX::CreateWaitAnimation())->SetTime(2.75f);

			GLX::AddScene(playlist, GLX::CreateOpacityAnimation(kNullKey, 1.0f, 0.0f))->SetTime(0.25f);

			GLX::AddScene(playlist, GLX::CreateCallbackAnimation([](Object & object) { object.Detach(); }));
		}

		GLX::Run(item, kNullKey, playlist);
	}

	ConstReference <GLX::Style> m_item_style;

	GLX::Object m_list;
};

REFLEX_END_INTERNAL

void ULM::AttachNotificationHandler(GLX::Object & object)
{
	GLX::BindEvent(object, kNotification, [&object](GLX::Object & src, GLX::Event & e)
	{
		auto notifications = GLX::Acquire(object, kNotification, 0, [&object]() -> Unretained <GLX::Object>
		{
			auto notifications = New<Notifications>();

			notifications->SetStyle(GLX::FindStyle(object, K32("notifications")));

			GLX::EnableFloat(notifications, GLX::kOrientationFit, GLX::kOrientationFit);

			return notifications;
		});

		Cast<Notifications>(notifications)->AddItem(Data::GetWString(e, GLX::kvalue));

		return true;
	});
}

bool ULM::EmitNotification(GLX::Object & object, const WString::View & msg)
{
	REFLEX_ASSERT(GLX::module.IsInitialised());

	return GLX::Emit(object, kNotification, GLX::kvalue, msg);
}

void ULM::EmitGlobalNotification(const WString::View & msg)
{
	if (GLX::module.IsInitialised())
	{
		GLX::Core::Context ctx;

		GLX::Core::desktop->EnumerateWindows([msg](GLX::WindowClient & window)
		{
			EmitNotification(window.GetContent(), msg);
		});
	}
}
