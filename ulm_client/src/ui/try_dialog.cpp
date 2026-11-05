#include "common.h"




//
//impl

AlreadyRetained <GLX::FlowDialog> ULM::AcquireTryDialog(GLX::Object & owner, WillRetain <Product> product, const WString::View & stylesheet, Key32 style_id_opt, const Function <void(Product & product)> & startdemo)
{
	REFLEX_DECLARE_KEY32(DemoSelectPage);
	REFLEX_DECLARE_KEY32(RequestLicencePage);

	auto wizard = AcquireProductWizard(owner, product, Join(L"Try ", ToWString(product->GetName().b)));

	AddLoginFlow(wizard, kRequestLicencePage);

	wizard->RegisterPage(kDemoSelectPage, kZeroKey, kNullKey, { .prev = kCancel }, [product, startdemo](GLX::FlowDialog & wizard) -> Unretained <GLX::Object>
	{
		auto body = New<GLX::Object>();

		auto page = New<GLX::Form>(L"Offline Activation", body);

		GLX::SetFlow(body, GLX::kFlowY | GLX::kFlowCenter);

		Detail::AttachStyleCascader(body);

		AddActionButton(wizard, body, L"30 Day Trial", L"Create a user-account and get a free trial licence", [](GLX::FlowDialog & wizard)
		{
			wizard.ShowPage(Detail::kLogin);
		});

		auto step2 = AddActionButton(wizard, body, L"Quick Demo", L"Start a 60 minute demo session", [product, startdemo](GLX::FlowDialog & wizard)
		{
			startdemo(product);
		});

		GLX::Activate(step2, True(startdemo));

		return page;
	});

	wizard->RegisterPage(kRequestLicencePage, kZeroKey, kNullKey, {}, [product](GLX::FlowDialog & wizard) -> Unretained <GLX::Object>
	{
		wizard.SetProperty(kNullKey, RequestTrial(product, product->GetDefaultFlavour(), [&wizard](Result result, Key32 code, const Message & msg)
		{
			GLX::Core::Context context;

			if (result == kResultSuccess)
			{
				wizard.ShowPage(kAutoActivatePage);
			}
			else
			{
				Data::SetWStringArray(wizard, Detail::kError, msg);

				Data::SetKey32(wizard, kback, Detail::kLogin);

				wizard.ShowPage(Detail::kError);
			}
		}));

		return New<GLX::Label>(L"Requesting trial licence...");
	});

	SetStyleSheet(wizard, stylesheet, style_id_opt);

	wizard->ShowPage(kDemoSelectPage);

	return wizard;
}
