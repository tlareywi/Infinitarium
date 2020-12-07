#include "ImGUI.hpp"

BOOST_CLASS_EXPORT_IMPLEMENT(ImGUI)

ImGUI::ImGUI() {
	platformGUI = IImGUI::Create();
}

ImGUI::~ImGUI() {
	platformGUI = nullptr;
}

void ImGUI::prepare(IRenderContext& context) {
	platformGUI->prepare(context);
}

void ImGUI::update(UpdateParams&) {

}

void ImGUI::render(IRenderPass& renderPass) {
	platformGUI->apply(renderPass);
}

template<class Archive> void ImGUI::serialize(Archive& ar, unsigned int version) {
	boost::serialization::void_cast_register<ImGUI, IRenderable>();
	ar& boost::serialization::make_nvp("IRenderable", boost::serialization::base_object<IRenderable>(*this));
}