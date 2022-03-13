//
//  Copyright © 2022 Blue Canvas Studios LLC. All rights reserved. Commercial use prohibited by license.
//

#include "ApplicationWindow.hpp"
#include "Module.hpp"

std::shared_ptr<IApplicationWindow> IApplicationWindow::Create() {
   return ModuleFactory<RendererFactory>::Instance()->createApplicationWindow();
}



