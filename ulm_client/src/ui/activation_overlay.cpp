#include "../../include/ulm/ui/activation_overlay.h"
#include "../../include/ulm/ui/activation_dialog.h"




//
//impl

REFLEX_BEGIN_INTERNAL(ULM)

struct ActivationOverlayImpl
{
	ActivationOverlayImpl(GLX::Object & owner)
		: owner(owner)
		, m_showchildren(false)
		, m_showchildren_z(kMaxUInt8)
	{
	}

	static void None(void * client) {}

	static void Show(void * self);

	static void Delete(void * self);


	GLX::Object & owner;

	FunctionPointer <void(ActivationOverlay&)> m_onclick;

	WString m_stylesheet;

	Key32 m_substyle;

	Key32 m_flavour;

	Reference <Product> m_product;

	Reference <Reflex::Object> m_licence_listener;

	UInt8 m_showchildren, m_showchildren_z;

	Reference <Reflex::Object> m_test;

	Pair <Product*,Key32> m_current_z;


	static const Pair <UIntNative> kTests[2];
};

void ActivationOverlayImpl::Show(void * client)
{
	GLX::Core::Context ctx;

	auto & self = *Cast<ActivationOverlay>(client);

	auto & impl = Reinterpret<ActivationOverlayImpl>(self.m_impl);

	self.EnableOnClock();

	GLX::AddStretch(impl.owner, self);

	GLX::Enter(self, GLX::kEnterAnimationFade);
}

void ActivationOverlayImpl::Delete(void * client)
{
	GLX::Core::Context ctx;

	auto & self = *Cast<ActivationOverlay>(client);

	auto & impl = Reinterpret<ActivationOverlayImpl>(self.m_impl);

	auto & product = *impl.m_product;

	GLX::Exit(self, true);

	Pair <UIntNative> tests[] = { ULM_CALLBACKS(ActivationOverlayImpl::None, ActivationOverlayImpl::Show), ULM_CALLBACKS(ActivationOverlayImpl::Show, ActivationOverlayImpl::Show) };

	bool force = impl.m_flavour != impl.m_product->GetActivationStatus().b;

	impl.m_test = product.CreateTest(ULM::kActivationStatusDemo, &self, tests[force], 1.0f, ULM_DESC("ActivationOverlay"));
}

const Pair <UIntNative> ActivationOverlayImpl::kTests[] =
{
	ULM_CALLBACKS(ActivationOverlayImpl::Delete, ActivationOverlayImpl::Show),
	ULM_CALLBACKS(ActivationOverlayImpl::Show, ActivationOverlayImpl::Show)
};

REFLEX_END_INTERNAL

ULM::ActivationOverlay::ActivationOverlay(GLX::Object & owner, FunctionPointer <void(ActivationOverlay&)> onclick)
{
	REFLEX_STATIC_ASSERT(sizeof(m_impl) >= sizeof(ActivationOverlayImpl));

	auto impl = Reinterpret<ActivationOverlayImpl>(&m_impl);

	Reflex::Detail::Constructor<ActivationOverlayImpl>::Construct(impl, owner);

	impl->m_onclick = onclick;

	Product::Decoy decoy;

	GLX::Detail::EnableEnterExitRenderToTexture(*this, false);

	GLX::SkipEnter(*this, GLX::kEnterAnimationFade);

	GLX::AddStretch(owner, this);

	EnableOnClock();
}

ULM::ActivationOverlay::~ActivationOverlay()
{
	auto & impl = Reinterpret<ActivationOverlayImpl>(m_impl);

	Reflex::Detail::Constructor<ActivationOverlayImpl>::Destruct(impl);
}

void ULM::ActivationOverlay::SetWizardStyle(const WString::View & path, Key32 style_id)
{
	auto impl = Reinterpret<ActivationOverlayImpl>(&m_impl);

	impl->m_stylesheet = path;

	impl->m_substyle = style_id;
}

void ULM::ActivationOverlay::SetProduct(Product & product, Key32 flavour)
{
	auto & impl = Reinterpret<ActivationOverlayImpl>(m_impl);

	if (SetFiltered(impl.m_current_z, MakeTuple(&product, flavour)))
	{
		impl.m_product = product;

		impl.m_flavour = flavour;

		impl.m_licence_listener = CreateActivationStatusListener(product, [this, &impl](ActivationStatus status, Key32 flavour)
		{
			bool force = impl.m_flavour != flavour;

			impl.m_test = impl.m_product->CreateTest(kActivationStatusDemo, this, ActivationOverlayImpl::kTests[force], 0.25f, ULM_DESC("ActivationOverlay"));
		});

		bool force = impl.m_flavour != impl.m_product->GetActivationStatus().b;

		impl.m_test = product.CreateTest(kActivationStatusDemo, this, ActivationOverlayImpl::kTests[force], 0.25f, ULM_DESC("ActivationOverlay"));
	}
}

AlreadyRetained <ULM::Product> ULM::ActivationOverlay::GetProduct()
{
	auto & impl = Reinterpret<ActivationOverlayImpl>(m_impl);

	return impl.m_product;
}

void ULM::ActivationOverlay::ShowActivationDialog()
{
	auto & impl = Reinterpret<ActivationOverlayImpl>(m_impl);

	if (impl.m_product)
	{
		AcquireActivationDialog(*this, *impl.m_product, impl.m_stylesheet, impl.m_substyle);
	}
}

bool ULM::ActivationOverlay::OnEvent(GLX::Object & src, GLX::Event & e)
{
	auto & impl = Reinterpret<ActivationOverlayImpl>(m_impl);

	if (e.id == GLX::kDragDropTender)
	{
		return !GLX::Emit(impl.owner, kAllowDragDrop, GLX::kitem, GLX::GetDragDropData(e));
	}
	else if (e.id == GLX::kMouseDown)
	{
		impl.m_onclick(*this);
	}

	return true;
}

bool ULM::ActivationOverlay::OnDragDropReceiveExternal(const Reflex::Object & object)
{
	return true;
}

void ULM::ActivationOverlay::OnClock(Float delta)
{
	SendTop();
}
