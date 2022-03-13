//
//  Copyright © 2022 Blue Canvas Studios LLC. All rights reserved. Commercial use prohibited by license.
//

#include "RenderProgram.hpp"
#include "Module.hpp"

std::shared_ptr<IRenderProgram> IRenderProgram::Create() {
   return ModuleFactory<RendererFactory>::Instance()->createRenderProgram();
}


