#pragma once

#include "Texture.hpp"

/// <summary>
/// X-plat interface for a on or off screen render target 
/// </summary>
class IRenderTarget : public ITexture {
public:
    IRenderTarget(const IRenderTarget& obj) : ITexture(obj), type(obj.type), resource(obj.resource), clearColor(obj.clearColor), blending(obj.blending) {}

    enum Type {
        Color,
        Depth,
        Stencil
    };

    enum Resource {
        Swapchain,
        Offscreen
    };

    virtual ~IRenderTarget() {}

    static std::shared_ptr<IRenderTarget> Create(unsigned int, unsigned int, Format, Type, Resource);
    static std::shared_ptr<IRenderTarget> Clone(const IRenderTarget&);
    static void clearRegisteredObjs();

    Resource getResource() { return resource; }
    glm::vec4 getClearColor() { return clearColor; }
    void setClearColor(float r, float g, float b, float a) { clearColor = glm::vec4(r, g, b, a); }
    const BlendState& getBlendState() const {
        return blending;
    }
    void setBlendState(const BlendState& blendState) {
        blending = blendState;
    }
    virtual void getData(const glm::uvec4&, void*) = 0;

protected:
    IRenderTarget() : ITexture(), clearColor(glm::vec4(0.0, 0, 0.0, 1.0)) {};
    IRenderTarget(const glm::uvec2& d, Format f, Type t, Resource r) : ITexture(d, f), type(t), resource(r), clearColor(glm::vec4(0.1, 0, 0.25, 1.0)) {}
    Type type;
    Resource resource;
    glm::vec4 clearColor;
    BlendState blending;
};

/// <summary>
/// X-plat serializable type of platform dependent render target type.
/// </summary>
class RenderTargetProxy : public IRenderTarget {
public:
    RenderTargetProxy() {}
    RenderTargetProxy(const IRenderTarget& obj) : IRenderTarget(obj) {}

    void prepare(IRenderContext&) override {};
    void getData(const glm::uvec4&, void*) override {};

private:
#if defined ENGINE_BUILD
    friend class boost::serialization::access;
    template<class Archive> void serialize(Archive&, unsigned int);
  //  template<class Archive> void load(Archive&, const unsigned int);
  //  template<class Archive> void save(Archive&, const unsigned int) const;
  //  BOOST_SERIALIZATION_SPLIT_MEMBER()
#endif
};

#if defined ENGINE_BUILD
#include <boost/serialization/export.hpp>
BOOST_SERIALIZATION_ASSUME_ABSTRACT(IRenderTarget)
BOOST_SERIALIZATION_ASSUME_ABSTRACT(ITexture)
BOOST_CLASS_EXPORT_KEY(RenderTargetProxy)
#endif