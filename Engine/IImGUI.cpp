//
//  Copyright © 2022 Blue Canvas Studios LLC. All rights reserved. Commercial use prohibited by license.
//

#include "IImGUI.hpp"
#include "Module.hpp"

std::shared_ptr<IImGUI> IImGUI::Create() {
	return ModuleFactory<RendererFactory>::Instance()->createImGUI();
}