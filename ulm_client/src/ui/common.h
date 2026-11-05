#pragma once

#include "../../include/ulm/ui/notifications.h"
#include "../../include/ulm/ui/markup.h"
#include "../../include/ulm/ui/window_handle.h"
#include "../../include/ulm/ui/activation_dialog.h"
#include "../../include/ulm/ui/login_dialog.h"




//
//dialog

REFLEX_NS(ULM)

constexpr WString::View kLoginPageTitle = L"Account Login";
constexpr WString::View kLoginPageHeadline = L"Please enter your user account details";

constexpr WString::View kCancel = L"Cancel";

REFLEX_DECLARE_KEY32(ellipsis);

REFLEX_DECLARE_KEY32(email);
REFLEX_DECLARE_KEY32(pwd);
REFLEX_DECLARE_KEY32(serial);
REFLEX_DECLARE_KEY32(back);	//for error handler
REFLEX_DECLARE_KEY32(next);	//for error handler

REFLEX_DECLARE_KEY32(form);
REFLEX_DECLARE_KEY32(progress);
REFLEX_DECLARE_KEY32(message);
REFLEX_DECLARE_KEY32(product);	//for autoactivate

REFLEX_DECLARE_KEY32(AutoActivatePage);

struct WizardInputSection : public GLX::Form
{
	WizardInputSection(const WString::View & key_label)
		: GLX::Form(key_label, New<GLX::TextArea>())
		, textarea(Cast<GLX::TextArea>(body))
	{
		GLX::EnableAutoFit(*this, false, true);

		GLX::EnableAutoFit(textarea, false, true);

		GLX::EnableMouse(header, false);

		Data::SetBool(textarea, GLX::kfocusable, true);

		Data::SetKey32(header, K32("overflow"), kellipsis);

		Data::SetKey32(textarea, K32("overflow"), kellipsis);
	}

	void OnSetStyle(const GLX::Style & style) override
	{
		header->SetStyle(style["key"]);

		textarea->SetStyle(style["value"]);
	}

	AlreadyRetained <GLX::TextArea> textarea;
};

struct InputFormDesc
{
	struct Input
	{
		Key32 id;
		WString::View title;
		decltype (&ValidateEmail) validate = [](const CString::View&) { return true; };
	};

	Key32 pageid;

	const WString::View title;

	Input first, second;

	const WString::View option_txt;

	Key32 option, next;
};


AlreadyRetained <GLX::FlowDialog> AcquireProductWizard(GLX::Object & owner, Product & product, const WString::View & title);

void SetStyleSheet(GLX::FlowDialog & dialog, const WString::View & path, Key32 id);


enum LoginFlowFlags : UInt8
{
	kLoginFlowCreateAccount = MakeBit(0),
	kLoginFlowAutoClose = MakeBit(1),
};

REFLEX_DECLARE_KEY32(SubmitLoginStep);

void RestoreEmail(GLX::FlowDialog & wizard);

void AddLoginFlow(GLX::FlowDialog & wizard, Key32 on_done_page, decltype (&Login) loginfn = &Login);

Unretained <GLX::FormEx> CreateInputForm(GLX::FlowDialog & wizard, const InputFormDesc & desc);

AlreadyRetained <GLX::Object> AddActionButton(GLX::FlowDialog & wizard, GLX::Object & form, const WString::View & label, const WString::View & desc, const Function <void(GLX::FlowDialog&)> & onclick);

void ShowLoginFlowMessage(GLX::FlowDialog & wizard, Key32 back, Key32 next, bool status, const Message & msg);

REFLEX_END
