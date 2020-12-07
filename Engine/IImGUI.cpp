#include "IImGUI.hpp"
#include "Module.hpp"

std::shared_ptr<IImGUI> IImGUI::Create() {
	return ModuleFactory<RendererFactory>::Instance()->createImGUI();
}