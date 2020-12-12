#pragma once

#include "Renderable.hpp"
#include "IImGUI.hpp"

#include "imgui.h"

class ImGUI : public IRenderable {
public:
    ImGUI();
    ImGUI(const ImGUI& obj) : IRenderable(obj) {
    }

    virtual ~ImGUI();

    void prepare(IRenderContext&) override;
    void update(UpdateParams&) override;
    void render(IRenderPass&) override;

private:
    std::shared_ptr<IImGUI> platformGUI{ nullptr };
    
    void ShowSceneGraph();
    void ShowRenderNodeProps( IRenderable* const );
    bool showSceneGraph{ true };
    std::function<void()> doVisit;
    std::function<bool(SceneObject&)> accumulatorPush;
    std::function<void(SceneObject&)> accumulatorPop;

    friend class boost::serialization::access;
    template<class Archive> void serialize(Archive&, const unsigned int);
};

BOOST_CLASS_EXPORT_KEY(ImGUI);