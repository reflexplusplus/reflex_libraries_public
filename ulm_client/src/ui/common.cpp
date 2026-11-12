#include "common.h"




//
//impl

REFLEX_NOINLINE void ULM::RestoreEmail(GLX::FlowDialog & wizard)
{
	CString  email = Data::GetCString(wizard, kemail);

	if (!email)
	{
		if (auto product = GetProperty<Product>(wizard, kproduct))
		{
			auto [vendor, name] = product->GetName();

			auto path = ULM::Detail::MakeProductPath(vendor, name, {});

			auto parent = File::ResolveIncludePath(path, L"../");

			auto prefs = Make<File::PersistentPropertySet>(Data::kPropertySetFormat);

			for (auto & i : File::List(parent).a)
			{
				auto name = File::RemoveTrailingStroke(i.key);

				if (prefs->Open(ULM::Detail::MakeProductPath(vendor, ToCString(name), Bootstrap::Detail::kPrefsFilename)))
				{
					auto licenceinfo = Data::GetPropertySet(prefs, Detail::GetLicenceInfoKey());

					if (auto email = Data::GetBinary(licenceinfo, kUser))
					{
						Data::SetCString(wizard, kemail, Data::Unpack<CString::View>(email));

						GLX::output.Warn(name);

						break;
					}
				}
			}
		}
	}
}

REFLEX_NOINLINE AlreadyRetained <GLX::FlowDialog> ULM::AcquireProductWizard(GLX::Object & owner, Product & product, const WString::View & title)
{
	auto product_id = product.GetID();

	if (auto window = QueryDialogWindow(owner, product_id))
	{
		auto wizard = Cast<GLX::FlowDialog>(window->GetContent());

		wizard->Clear();

		return wizard;
	}

	auto wizard = New<GLX::FlowDialog>(title);

	Data::SetBool(wizard, GLX::kresizable, true);

	wizard->footer->GetLast()->SetState(GLX::kSelectedState);

	wizard->SetProperty(kproduct, product);	//!keep alive

	wizard->SetProperty(K32("auto_close"), CreateActivationStatusListener(product, [wizard](ActivationStatus status, Key32 flavour)
	{
		if (status != kActivationStatusNone)
		{
			GLX::Core::Context context;

			wizard->Close();
		}
	}));

	ShowDialogWindow(owner, title, wizard, product_id);

	return wizard;
}

REFLEX_NOINLINE void ULM::SetStyleSheet(GLX::FlowDialog & dialog, const WString::View & stylesheet, Key32 style_id)
{
	auto style_path = ToView(style_id);

	if (style_id == kNullKey) style_path.size = 0;

	Bootstrap::Detail::SetStyle(dialog, stylesheet, style_path);
}

REFLEX_NOINLINE Unretained <GLX::FormEx> ULM::CreateInputForm(GLX::FlowDialog & wizard, const InputFormDesc & desc)
{
	static constexpr auto validate = [](GLX::FlowDialog & wizard, const InputFormDesc & desc)
	{
		REFLEX_LOOP_PTR(&desc.first, pinput, 2)
		{
			if (!pinput->validate(Data::GetCString(wizard, pinput->id))) return false;
		}

		return true;
	};

	auto body = New<GLX::Object>();

	Detail::AttachStyleCascader(body);

	auto form = New<GLX::FormEx>(desc.title, body, New<GLX::Button>(desc.option_txt));

	GLX::SetFlow(form->body, GLX::kFlowY);

	REFLEX_LOOP_PTR(&desc.first, pinput, 2)
	{
		auto id = pinput->id;

		if (id != kNullKey)
		{
			auto value = Data::GetCString(wizard, id);

			auto input = New<WizardInputSection>(pinput->title);

			auto textedit = input->textarea->GetContent();

			textedit->id = id;

			GLX::SetText(textedit, ToWString(value));

			textedit->Focus();

			Detail::SetCascadingStyleId(GLX::AddInline(body, input), K32("input"));

			switch (id.value)
			{
			case kemail:
				Data::SetKey32(textedit, K32("transform"), K32("lowercase"));
				break;

			case kpwd:
				Data::SetKey32(textedit, K32("transform"), K32("mask"));
				break;
			};
		}
	}

	GLX::EnableMouse(GLX::BindClick(form->footer, [&wizard, desc]()
	{
		wizard.ShowPage(desc.option);
	}));

	GLX::SetEventDelegate(body, kNullKey, [&wizard, desc](GLX::Object & src, GLX::Event & e)
	{
		switch (e.id.value)
		{
		case GLX::kTransaction:
		case GLX::kComplete:
			Data::SetCString(wizard, src.id, ToCString(GLX::GetText(src)));
			Detail::EnableNextButton(wizard, desc.pageid, desc.next, validate(wizard, desc));
			if (e.id.value == GLX::kComplete) wizard.ShowPage(desc.next);
			return true;

		default:
			return false;
		}
	});

	GLX::EnableTabNavigation(form);

	Detail::EnableNextButton(wizard, desc.pageid, desc.next, validate(wizard, desc));

	body->GetFirst()->GetLast()->Focus();

	return form;
}

REFLEX_NOINLINE AlreadyRetained <GLX::Object> ULM::AddActionButton(GLX::FlowDialog & wizard, GLX::Object & parent, const WString::View & label, const WString::View & desc, const Function <void(GLX::FlowDialog&)> & onclick)
{
	auto object = New<GLX::Button>(label);

	GLX::SetText(object, desc, K32("desc"));

	GLX::BindClick(object, [&wizard, onclick]()
	{
		onclick(wizard);
	});

	Detail::SetCascadingStyleId(GLX::AddInline(parent, object), K32("action"));

	return object;
}

REFLEX_NOINLINE void ULM::ShowLoginFlowMessage(GLX::FlowDialog & wizard, Key32 back, Key32 next, bool status, const Message & msg)
{
	Data::SetWStringArray(wizard, kmessage, msg);

	wizard.RegisterPage(kmessage, back, next, {}, [](GLX::FlowDialog & wizard) -> Unretained <GLX::Object>
	{
		return Detail::CreateMessageBox(false, Data::GetWStringArray(wizard, kmessage));
	});

	wizard.ShowPage(kmessage, GLX::FlowDialog::Direction(status));
}
