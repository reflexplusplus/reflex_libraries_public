#pragma once

#include "../[require].h"




//
//decl

REFLEX_NS(Netx::Detail)

[[nodiscard]] Unretained <GLX::TextArea> CreateCommandLine(const Function <bool(Key32 cmd, Data::PropertySet & args)> & callback);


void BeginSetValue(GLX::Object & owner, const WString::View & label, const WString::View & value, const Function <void(const WString&)> & apply);


REFLEX_DECLARE_KEY32(RequestDisplay);

void RequestDisplay(GLX::Object & source, WString && param, WString && value);

void RequestDisplay(GLX::Object & source, const ParameterSet::Allocation & allocation, UInt idx);


REFLEX_DECLARE_KEY32(GetSelectedComponents);

Array < Reference <Component> > GetSelectedComponents(GLX::Object & source, Component & focus);

REFLEX_END
