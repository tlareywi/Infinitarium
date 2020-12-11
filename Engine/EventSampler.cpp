//
//  EventSampler.cpp
//  InfinitariumEngine
//
//  Created by Trystan Larey-Williams on 11/7/18.
//

#include "EventSampler.hpp"
#include "Module.hpp"

std::shared_ptr<IEventSampler> IEventSampler::Create() {
   return ModuleFactory<RendererFactory>::Instance()->createEventSampler();
}


