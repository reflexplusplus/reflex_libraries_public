#pragma once

#include "[require].h"



	
//
//declarations

REFLEX_NS(ULM)

Unretained <Object> AutoActivate(Product & product, const Product::ActivateCallback & callback);


AlreadyRetained <GLX::FlowDialog> AcquireActivationDialog(GLX::Object & owner, WillRetain <Product> product, const WString::View & stylesheet, Key32 style_id_opt);

AlreadyRetained <GLX::FlowDialog> AcquireTryDialog(GLX::Object & owner, WillRetain <Product> product, const WString::View & stylesheet, Key32 style_id_opt, const Function <void(Product & product)> & startdemo);

REFLEX_END
