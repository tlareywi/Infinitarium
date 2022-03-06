#include "RenderTarget.hpp"
#include "Module.hpp"

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_IMPLEMENT(RenderTargetProxy)
BOOST_CLASS_TRACKING(RenderTargetProxy, boost::serialization::track_never)
BOOST_CLASS_TRACKING(IRenderTarget, boost::serialization::track_never)
BOOST_CLASS_TRACKING(ITexture, boost::serialization::track_never)

////////////////////////////////////////////////////////////////////////////////////////
// IRenderTarget
////////////////////////////////////////////////////////////////////////////////////////

static std::map<unsigned long long, std::shared_ptr<IRenderTarget>> registeredObjsRT = std::map<unsigned long long, std::shared_ptr<IRenderTarget>>();

std::shared_ptr<IRenderTarget> IRenderTarget::Create(unsigned int x, unsigned int y, Format format, Type type, Resource resource) {
	return ModuleFactory<RendererFactory>::Instance()->createRenderTarget(glm::uvec2(x, y), format, type, resource);
}

std::shared_ptr<IRenderTarget> IRenderTarget::Clone(const IRenderTarget& obj) {
	std::map<unsigned long long, std::shared_ptr<IRenderTarget>>::iterator it{ registeredObjsRT.find(obj._objId) };

	if (it == registeredObjsRT.end())
		registeredObjsRT[obj._objId] = ModuleFactory<RendererFactory>::Instance()->createRenderTargetCopy(obj);

	return registeredObjsRT[obj._objId];
}

void IRenderTarget::clearRegisteredObjs() {
	registeredObjsRT.clear();
}

template<class Archive> void RenderTargetProxy::serialize(Archive& ar, const unsigned int version) {
	ar & BOOST_SERIALIZATION_NVP(dim);
	ar & BOOST_SERIALIZATION_NVP(format);
	ar & BOOST_SERIALIZATION_NVP(type);
	ar & BOOST_SERIALIZATION_NVP(resource);
	ar & BOOST_SERIALIZATION_NVP(clearColor);
	ar & BOOST_SERIALIZATION_NVP(blending);
	ar & BOOST_SERIALIZATION_NVP(name);
	ar & BOOST_SERIALIZATION_NVP(_objId);

	std::cout << "Serializing RenderTargetProxy " << _objId << " " << name << std::endl;
}