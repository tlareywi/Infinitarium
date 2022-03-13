//
//  Copyright © 2022 Blue Canvas Studios LLC. All rights reserved. Commercial use prohibited by license.
//

#include "RenderCommand.hpp"
#include "Module.hpp"

std::shared_ptr<IRenderCommand> IRenderCommand::Create() {
   return ModuleFactory<RendererFactory>::Instance()->createRenderCommand();
}

