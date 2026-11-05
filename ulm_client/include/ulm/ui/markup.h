#pragma once

#include "[require].h"




//
//Primary API

REFLEX_NS(ULM)

Unretained <GLX::Object> CreateMarkupViewer(const Data::Archive::View & reflex_markup);

REFLEX_END




//
//Detail

REFLEX_NS(ULM::Detail)

void AttachStyleCascader(GLX::Object & object);

void SetCascadingStyleId(GLX::Object & object, Key32 style_id);


using MarkupItemCtr = FunctionPointer <Unretained<GLX::Object>(Object & client, Key32 type, bool container, CString::View value)>;

Unretained <GLX::Object> CreateMarkupViewer(const Data::Archive::View & reflex_markup, Object & client, MarkupItemCtr ctr);

REFLEX_END




//
//impl

inline void ULM::Detail::SetCascadingStyleId(GLX::Object & object, Key32 style_id)
{
	Data::SetKey32(object, K32("style_id"), style_id);
}

inline Unretained <Reflex::GLX::Object> ULM::CreateMarkupViewer(const Data::Archive::View & reflex_markup)
{
	return Detail::CreateMarkupViewer(reflex_markup, REFLEX_NULL(Object), [](Object & client, Key32 type, bool container, CString::View value)
	{
		auto obj = New<GLX::Object>();

		Detail::SetCascadingStyleId(obj, type);

		GLX::SetFlow(obj, container ? GLX::kFlowY : GLX::kFlowX);

		GLX::EnableAutoFit(obj, false, true);

		GLX::SetText(obj, Data::DecodeUTF8(Data::Pack(value)));

		return obj;
	});
}
