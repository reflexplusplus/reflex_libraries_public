#pragma once

#include "../../netx.h"




//
//decl

REFLEX_NS(Netx)

struct ComponentViewClass : public Reflex::Detail::StaticItem <ComponentViewClass>
{
	ComponentViewClass(Reflex::Detail::DynamicTypeRef object_t, FunctionPointer <Unretained<GLX::Object>(Component & component)> ctr)
		: object_t(object_t)
		, ctr(ctr)
	{
	}

	const Reflex::Detail::DynamicTypeRef object_t;

	const FunctionPointer <Unretained<GLX::Object>(Component & component)> ctr;
};

struct GenericTableView : public GLX::Object
{
	struct Descriptor
	{
		ClassID classid;
		WString::View label;
		Array <WString::View> columns;
		Key32 style_id = GLX::kcontent;
	};

	GenericTableView(const Descriptor & desc);

	virtual void AddRow(ArrayView <WString> row) = 0;


	const Descriptor desc;
};

[[nodiscard]] Unretained <GLX::Object> CreateGenericTableView(Component & component, ArrayView <GenericTableView::Descriptor> tables, const Function <void(Component & component, GenericTableView & view)> & callback);

REFLEX_END
