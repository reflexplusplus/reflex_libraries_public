#pragma once

#include "[require].h"




//
//Primary API

REFLEX_NS(ULM)

REFLEX_DECLARE_KEY32(Notification);

void AttachNotificationHandler(GLX::Object & object);

bool EmitNotification(GLX::Object & src, const WString::View & msg);

void EmitGlobalNotification(const WString::View & msg);

REFLEX_END
