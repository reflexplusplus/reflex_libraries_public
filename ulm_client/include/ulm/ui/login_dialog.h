#pragma once

#include "[require].h"



	
//
//declarations

REFLEX_NS(ULM)

AlreadyRetained <GLX::FlowDialog> AcquireLoginDialog(GLX::Object & owner, const WString::View & stylesheet, Key32 style_id_opt, decltype (&Login) = &Login);

constexpr WString::View kPleaseWait = L"Please wait...";

REFLEX_END

REFLEX_NS(ULM::Detail)

void EnableNextButton(GLX::FlowDialog & dialog, Key32 page_id, Key32 next_page_id, bool enable);

Unretained <GLX::Object> CreateMessageBox(bool result, const Message::View & msg);

REFLEX_END




//
//impl

inline void ULM::Detail::EnableNextButton(GLX::FlowDialog & dialog, Key32 page_id, Key32 next_page_id, bool enable)
{
	dialog.UpdateButton(page_id, GLX::FlowDialog::kButtonNext, enable ? next_page_id : kNullKey);
}
