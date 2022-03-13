//
//  Copyright © 2022 Blue Canvas Studios LLC. All rights reserved. Commercial use prohibited by license.
//

#include "EventSampler.hpp"
#include "Module.hpp"

std::shared_ptr<IEventSampler> IEventSampler::Create() {
   return ModuleFactory<RendererFactory>::Instance()->createEventSampler();
}


