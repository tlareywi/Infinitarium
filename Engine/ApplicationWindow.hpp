//
//  SimulationWindow.hpp
//  InfinitariumEngine
//
//  Created by Trystan Larey-Williams on 11/8/18.
//

#pragma once

#include <memory>

#include "RenderContext.hpp"

class IApplicationWindow {
public:
   static std::shared_ptr<IApplicationWindow> Create( IRenderContext& );
};
