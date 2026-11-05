#include "common.h"
#include "resources.h"




//
//dialog

REFLEX_BEGIN_INTERNAL(ULM)

WString MakeOfflineOutputPath(Product & product)
{
	return Join(System::GetPath(System::kPathDesktop), ToWString(product.GetName().a), L" Offline Activation Request", System::kPathDelimiter);
}

REFLEX_END_INTERNAL

Unretained <Object> ULM::AutoActivate(Product & product, const Product::ActivateCallback & callback)
{
	struct AutoActivator :
		public Object,
		public Signal <Result, Key32, const Message&>
	{
		AutoActivator(Product & product)
			: product(product)
		{
			m_action = product.AutoActivate([this](Result result, Key32 code, const Message & msg)
			{
				auto & product = AutoActivator::product;
							
				if (result == kResultSuccess)
				{
					GLX::Core::Context context;

					auto licenceinfo = GetLicenceInfo(product);

					//auto type = NSA::GetLicenceTypeOrFlavourString(Data::Unpack<Key32>(Data::GetBinary(licenceinfo, kType)), false);

					auto serial = Data::Unpack<CString::View>(Data::GetBinary(licenceinfo, kSerial));

					auto notification = Join("Activated ", product.GetName().b, ' ', '[', serial, ']');

					EmitGlobalNotification(ToWString(notification));
				}

				Notify(result, code, msg);

				product.UnsetProperty<AutoActivator>(kNullKey);
			});
		}
		
		Product & product;

		Reference <Object> m_action;

		struct ListenerImpl : 
			public Reference <AutoActivator>,
			public Signal <Result,Key32,const Message&>::Listener
		{
			ListenerImpl(AutoActivator & activator, const Product::ActivateCallback & callback)
				: Reference<AutoActivator>(activator)
				, Listener(activator, callback)
			{
			}
		};
	};

	if (GetUserLogin())
	{
		auto activator = Data::Detail::AcquireProperty<AutoActivator>(product, kNullKey, product);

		return REFLEX_CREATE(AutoActivator::ListenerImpl, activator, callback);
	}
	else
	{
		return Null<Object>();
	}
}

AlreadyRetained <GLX::FlowDialog> ULM::AcquireActivationDialog(GLX::Object & owner, WillRetain <Product> product, const WString::View & stylesheet, Key32 style_id)
{
	REFLEX_DECLARE_KEY32(ActivateBySerialPage);
	REFLEX_DECLARE_KEY32(SubmitSerialStep);
	REFLEX_DECLARE_KEY32(OfflineStep1);
	REFLEX_DECLARE_KEY32(OfflineStep2);
	REFLEX_DECLARE_KEY32(CreateOfflineRequestStep);
	REFLEX_DECLARE_KEY32(SubmitOfflineResponseStep);

	REFLEX_DECLARE_KEY32(offline_response_filename);

	static constexpr WString::View kProductActivation = L"Product Activation";

	static constexpr auto response_handler = [](GLX::FlowDialog & wizard, Result result, Key32 code, const Message & msg)
	{
		GLX::Core::Context context;

		switch (result)
		{
		case kResultSuccess:
			//StoreUserEmail(Data::GetCString(wizard, kemail));
			break;

		case kResultFail:
			ShowLoginFlowMessage(wizard, kActivateBySerialPage, kNullKey, false, Mid<true>(msg, 1));
			break;

		case kResultNoConnection:
			wizard.ShowPage(kOfflineStep1);
			break;

		default:
			break;
		}
	};

	auto wizard = AcquireProductWizard(owner, product, kProductActivation);

	AddLoginFlow(wizard, kAutoActivatePage);

	//override the login page

	wizard->RegisterPage(Detail::kLogin, kZeroKey, kSubmitLoginStep, { .prev = kCancel }, [](GLX::FlowDialog & wizard) -> Unretained <GLX::Object>
	{
		GLX::SetText(wizard.header, kProductActivation);

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
			.option_txt = L"Activate with my Licence Key instead",
			.option = kActivateBySerialPage,
			.next = kSubmitLoginStep
		});

		return form;
	});

	//wizard->RegisterPage(kSubmitLoginStep, kNullKey, kNullKey, {}, [product](Bootstrap::FlowDialog & wizard) -> Unretained <GLX::Object>
	//{
	//	SetProperty(wizard, kNullKey, product->Activate(Data::GetCString(wizard, kemail), Data::GetCString(wizard, kserial), false, [&wizard](Result result, Key32 code, const Message & msg)
	//	{
	//		response_handler(wizard, result, code, msg);
	//	}));

	//	return New<GLX::Label>(kPleaseWait);
	//});

	wizard->RegisterPage(kActivateBySerialPage, kZeroKey, kSubmitSerialStep, { .prev = kCancel }, [product](GLX::FlowDialog & wizard) -> Unretained <GLX::Object>
	{
		GLX::SetText(wizard.header, kProductActivation);

		auto title = Join(L"Please enter your ", ToWString(product->GetName().b), L" licence details");

		auto form = CreateInputForm(wizard,
		{
			.pageid = kActivateBySerialPage,
			.title = title,
			.first =
			{
				.id = kemail,
				.title = L"Email",
				.validate = &ValidateEmail
			},
			.second =
			{
				.id = kserial,
				.title = L"Licence Key",
				.validate = &ValidateLicenceCode
			},
			.option_txt = L"Activate via login instead",
			.option = Detail::kLogin,
			.next = kSubmitSerialStep
		});
	
		return form;
	});

	wizard->RegisterPage(kSubmitSerialStep, kNullKey, kNullKey, {}, [product](GLX::FlowDialog & wizard) -> Unretained <GLX::Object>
	{
		wizard.SetProperty(kNullKey, product->Activate(Data::GetCString(wizard, kemail), Data::GetCString(wizard, kserial), false, [&wizard](Result result, Key32 code, const Message & msg)
		{
			response_handler(wizard, result, code, msg);
		}));

		return New<GLX::Label>(kPleaseWait);
	});

	wizard->RegisterPage(kCreateOfflineRequestStep, kNullKey, kNullKey, {}, [product](GLX::FlowDialog & wizard) -> Unretained <GLX::Object>
	{
		auto email = Data::GetCString(wizard, kemail);

		auto serial = Data::GetCString(wizard, kserial);

		wizard.SetProperty(kNullKey, product->CreateOfflineRequest(email, serial, false, [&wizard, product](const Data::Archive::View & request)
		{
			auto folder = MakeOfflineOutputPath(product);

			System::MakeDirectory(folder);

			File::Save(Join(folder, ToWString(product->GetName().b), L"ActivationRequest.nsa_req"), request);

			File::Save(Join(folder, L"Instructions.txt"), File::Extract(kOfflineActivationInstructions));

			System::Open(folder);

			GLX::Core::Context ctx;

			wizard.ShowPage(kOfflineStep2);
		}));

		return New<GLX::Label>(kPleaseWait);
	});

	wizard->RegisterPage(kSubmitOfflineResponseStep, kNullKey, kOfflineStep2, {}, [product](GLX::FlowDialog & wizard) -> Unretained <GLX::Object>
	{
		auto filename = Data::GetWString(wizard, koffline_response_filename);

		wizard.SetProperty(kNullKey, product->SubmitOfflineResponse(File::Open(filename), [&wizard](Result result, Key32 code, const Message & msg)
		{
			response_handler(wizard, result, code, msg);
		}));

		return New<GLX::Label>(kPleaseWait);
	});

	constexpr Key32 offline_steps[] = { kActivateBySerialPage, kOfflineStep1, kOfflineStep2 };

	REFLEX_LOOP(idx, 2)
	{
		auto id = offline_steps[idx + 1];

		wizard->RegisterPage(id, offline_steps[idx], kNullKey, {}, [product, id](GLX::FlowDialog & wizard) -> Unretained <GLX::Object>
		{
			auto body = New<GLX::Object>();

			auto page = New<GLX::Form>(L"Offline Activation", body);

			GLX::SetFlow(body, GLX::kFlowY | GLX::kFlowCenter);

			Detail::AttachStyleCascader(body);

			auto step1 = AddActionButton(wizard, body, L"Step 1", L"Generate Request File", [](GLX::FlowDialog & wizard)
			{
				wizard.ShowPage(kCreateOfflineRequestStep);
			});

			auto step2 = AddActionButton(wizard, body, L"Step 2", L"Open Response File", [product](GLX::FlowDialog & wizard)
			{
				auto folder = MakeOfflineOutputPath(product);

				if (auto path = GLX::ShowFileDialog(product->prefs, K32("offline_response_file"), false, { L"nsa_resp" }, folder))
				{
					Data::SetWString(wizard, koffline_response_filename, path);

					wizard.ShowPage(kSubmitOfflineResponseStep);
				}
			});

			if (id == kOfflineStep1)
			{
				GLX::Activate(step2, false);
			}
			else
			{
				GLX::Activate(step1, false);

			}

			return page;
		});
	}

	SetStyleSheet(wizard, stylesheet, style_id);

	wizard->ShowPage(Detail::kLogin);

	return wizard;
}
