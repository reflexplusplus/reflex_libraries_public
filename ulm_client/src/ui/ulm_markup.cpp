#include "../../include/ulm/ui/markup.h"




void ULM::Detail::AttachStyleCascader(GLX::Object & object)
{
	REFLEX_LOCAL(void, ApplyStyles)(GLX::Object & object, const GLX::Style & style)
	{
		for (auto & i : object)
		{
			if (auto pstyle_id = i.QueryProperty<Data::Key32Property>(K32("style_id")))
			{
				auto sub_style = style[pstyle_id->value];

				i.SetStyle(sub_style);

				Call(i, sub_style);
			}
			else
			{
				Call(i, style);
			}
		}
	}
	REFLEX_END

	GLX::SetOnStyle(object, K32("Bootstrap::AttachStyleCascader"), Bind(&ApplyStyles::Call, ByRef(object), _P1));
}

Unretained <GLX::Object> ULM::Detail::CreateMarkupViewer(const Data::Archive::View & markup, Object & client, MarkupItemCtr ctr)
{
	REFLEX_LOCAL(void, Recurse)(Object & client, MarkupItemCtr ctr, GLX::Object & parent, ArrayView < ConstReference <Data::PropertySet> > children)
	{
		CString::View args;

		for (auto & i : children)
		{
			args.size = 0;

			auto value = Data::GetCString(i, K32("value"));

			auto grand_children = Data::GetXmlNodes(i);

			auto child = ctr(client, Data::GetKey32(i, "type"), True(grand_children), value);

			if (grand_children)
			{
				Call(client, ctr, child, grand_children);
			}

			GLX::AddInline(parent, child);
		}
	}
	REFLEX_END

	auto obj = New<GLX::Object>();

	GLX::EnableMouse(obj, false);

	GLX::SetFlow(obj, GLX::kFlowY);

	auto decoded = Data::DecodePropertySet(Data::kReflexMarkupFormat, markup);

	Recurse::Call(client, ctr, obj, Data::GetXmlNodes(decoded));

	GLX::EnableAutoFit(obj, true, true);

	AttachStyleCascader(obj);

	return obj;
}
