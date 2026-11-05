#include "../../include/ulm/ui/login_dialog.h"
#include "common.h"
#include "resources.h"




//
//impl

void ULM::AddLoginFlow(GLX::FlowDialog & wizard, Key32 ondone_page, decltype (&Login) loginfn)
{
	//REFLEX_SHOWSTOPPER("message box lines");

	REFLEX_DECLARE_KEY32(CreateAccountStep1);
	REFLEX_DECLARE_KEY32(CreateAccountStep2);
	REFLEX_DECLARE_KEY32(CreateAccountStep3);
	REFLEX_DECLARE_KEY32(SubmitCreateAccountStep);

	REFLEX_DECLARE_KEY32(first_name);
	REFLEX_DECLARE_KEY32(last_name);

	static constexpr auto response_handler = [](GLX::FlowDialog & wizard, bool login, Result result, Key32 code, const Message & msg, Key32 ondone_page)
	{
		GLX::Core::Context context;

		if (result == kResultSuccess)
		{
			//StoreUserEmail(Data::GetCString(wizard, kemail));

			if (login)
			{
				if (!wizard.ShowPage(ondone_page))
				{
					wizard.Close();
				}
			}
			else
			{
				ShowLoginFlowMessage(wizard, kNullKey, Detail::kLogin, true, Mid<true>(msg, 1));
			}

			return;
		}
		else if (result == kResultFail && code == K32("ErrorAccountAlreadyExists"))
		{
			//StoreUserEmail(Data::GetCString(wizard, kemail));
		}

		ShowLoginFlowMessage(wizard, login ? Detail::kLogin : kCreateAccountStep3, kNullKey, false, msg);
	};

	wizard.RegisterPage(Detail::kLogin, kZeroKey, kSubmitLoginStep, { .prev = kCancel }, [](GLX::FlowDialog & wizard) -> Unretained <GLX::Object>
	{
		GLX::SetText(wizard.header, kLoginPageTitle);

		RestoreEmail(wizard);

		auto form = CreateInputForm(wizard,
		{
			.pageid = Detail::kLogin,
			.title = kLoginPageHeadline,
			.first =
			{
				.id = kemail,
				.title = L"Email",
				.validate = &ValidateEmail
			},
			.second =
			{
				.id = kpwd,
				.title = L"Password",
				.validate = &ValidatePassword
			},
			.option_txt = L"I don't have a user account",
			.option = kCreateAccountStep1,
			.next = kSubmitLoginStep
		});

		return form;
	});

	wizard.RegisterPage(kSubmitLoginStep, kNullKey, kNullKey, {}, [ondone_page, loginfn](GLX::FlowDialog & wizard) -> Unretained <GLX::Object>
	{
		wizard.SetProperty(kNullKey, loginfn(Data::GetCString(wizard, kemail), Data::GetCString(wizard, kpwd), [&wizard, ondone_page](Result result, Key32 code, const Message & msg)
		{
			response_handler(wizard, true, result, code, msg, ondone_page);
		}));

		return New<GLX::Label>(kPleaseWait);
	});

	wizard.RegisterPage(kCreateAccountStep1, kZeroKey, kCreateAccountStep2, { kCancel, L"Agree" }, [](GLX::FlowDialog & wizard) -> Unretained <GLX::Object>
	{
		GLX::SetText(wizard.header, L"Create Account");


		auto page = New<GLX::Object>();

		GLX::EnableAutoFit(page, false, false);

		GLX::SetFlow(page, GLX::kFlowY);

		Detail::AttachStyleCascader(page);


		auto scroller = New< GLX::ScrollerOfType <GLX::Object> >();

		scroller->SetContent(CreateMarkupViewer(File::Extract(kUserAccountTerms)));

		Detail::SetCascadingStyleId(GLX::AddInline(page, scroller), K32("scroller"));

		GLX::EnableInline<true>(scroller);


		auto footer = GLX::AddInline(page, New<GLX::Button>(L"I already have a user account"));

		GLX::BindClick(footer, [&wizard]()
		{
			wizard.ShowPage(Detail::kLogin);
		});

		Detail::SetCascadingStyleId(footer, K32("footer"));

		return page;
	});

	wizard.RegisterPage(kCreateAccountStep2, kCreateAccountStep1, kNullKey, {}, [](GLX::FlowDialog & wizard) -> Unretained <GLX::Object>
	{
		auto form = CreateInputForm(wizard,
		{
			.pageid = kCreateAccountStep2,
			.title = L"Please enter your email address",
			.first =
			{
				.id = kemail,
				.title = L"Email",
				.validate = &ValidateEmail
			},
			.next = kCreateAccountStep3
		});

		return form;
	}); 

	wizard.RegisterPage(kCreateAccountStep3, kCreateAccountStep2, kNullKey, {}, [](GLX::FlowDialog & wizard) -> Unretained <GLX::Object>
	{
		auto form = CreateInputForm(wizard,
		{
			.pageid = kCreateAccountStep3,
			.title = L"Please enter your name",
			.first =
			{
				.id = kfirst_name,
				.title = L"First Name",
				.validate = [](const CString::View & name) { return True(name.size); }
			},
			.second =
			{
				.id = klast_name,
				.title = L"Last Name"
			},
			.next = kSubmitCreateAccountStep
		});

		return form;
	});

	wizard.RegisterPage(kSubmitCreateAccountStep, kNullKey, kNullKey, {}, [ondone_page](GLX::FlowDialog & wizard) -> Unretained <GLX::Object>
	{
		wizard.SetProperty(kNullKey, CreateAccount(Data::GetCString(wizard, kemail), Data::GetCString(wizard, kfirst_name), Data::GetCString(wizard, klast_name), [&wizard, ondone_page](Result result, Key32 code, const Message & msg)
		{
			response_handler(wizard, false, result, code, msg, ondone_page);
		}));
			
		return New<GLX::Label>(kPleaseWait);
	});

	wizard.RegisterPage(kAutoActivatePage, kZeroKey, kNullKey, {}, [](GLX::FlowDialog & wizard) -> Unretained <GLX::Object>
	{
		if (auto product = GetProperty<Product>(wizard, kproduct))
		{
			wizard.SetProperty(kNullKey, AutoActivate(RemoveConst(*product), [&wizard](Result result, Key32 code, const Message & msg)
			{
				GLX::Core::Context context;

				if (result == kResultSuccess)
				{
					wizard.Close();	//need this for case of demoing soundpack
				}
				else
				{
					Data::SetWStringArray(wizard, Detail::kError, msg);

					Data::SetKey32(wizard, kback, Detail::kLogin);

					wizard.ShowPage(Detail::kError);
				}
			}));
		}
		else
		{
			REFLEX_ASSERT(false);
		}

		return New<GLX::Label>(L"Activating...");
	});
}

Unretained <GLX::Object> ULM::Detail::CreateMessageBox(bool success, const Message::View & msg)
{
	constexpr WString::View kTitles[2] = { L"Error", L"Success" };

	auto body = New<GLX::Object>();

	Detail::AttachStyleCascader(body);

	auto message_box = New<GLX::Form>(kTitles[success], body);

	GLX::SetFlow(body, GLX::kFlowY | GLX::kFlowCenter);

	for (auto & i : msg)
	{
		Detail::SetCascadingStyleId(GLX::AddInline(body, New<GLX::Label>(i)), K32("p"));
	}

	return message_box;
}

AlreadyRetained <GLX::FlowDialog> ULM::AcquireLoginDialog(GLX::Object & owner, const WString::View & stylesheet, Key32 style_id_opt, decltype (&Login) loginfn)
{
	auto wizard = New<GLX::FlowDialog>(kLoginPageTitle);

	AddLoginFlow(wizard, kNullKey, loginfn);

	wizard->SetProperty(kNullKey, CreateLoginListener([wizard]()
	{
		if (GetUserID())
		{
			GLX::Core::Context context;

			wizard->Close();
		}
	}));

	SetStyleSheet(wizard, stylesheet, style_id_opt);

	wizard->ShowPage(Detail::kLogin);

	Bootstrap::ShowDialogWindow(owner, kLoginPageTitle, wizard);

	return wizard;
}
