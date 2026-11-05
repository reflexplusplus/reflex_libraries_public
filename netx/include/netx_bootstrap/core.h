#pragma once

#include "core/[require].h"

#include "core/module.h"

#include "core/factory.h"
#include "core/propertydef.h"
#include "core/external_component_handle.h"
#include "core/external_component.h"
#include "core/writeable_input_token.h"

#include "core/detail/extin.h"
#include "core/detail/extout.h"
#include "core/detail/parameter_assignments.h"
#include "core/components/detail/group.h"
#include "core/components/refresh_clock.h"
#include "core/components/abstract_propertyset.h"
#include "core/components/dynamic_components.h"
#include "core/components/dynamic_pins.h"
#include "core/components/fileformat.h"
#include "core/components/parameterset.h"
#include "core/components/propertyset.h"
#include "core/components/abstract_sequencer.h"
#include "core/components/mixer.h"
#include "core/components/peakinfo.h"
#include "core/components/voicemanager.h"
#include "core/components/matrix.h"
#include "core/components/voicefx.h"
#include "core/components/virtual_keyboard.h"

#include "core/modules/group.h"
#include "core/modules/plugin.h"

#include "core/util/module_base.h"
#include "core/util/observed_value.h"
#include "core/util/network_clock.h"
#include "core/util/keyboard_state.h"
#include "core/util/sustain_processor.h"
#include "core/util/midi.h"
#include "core/util/pins.h"
#include "core/util/parameterset_initialiser.h"

#include "core/parameter.h"

#include "core/functions.h"

#include "core/detail/fileformat.h"
#include "core/detail/property_normaliser.h"
