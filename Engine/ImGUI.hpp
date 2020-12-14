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
    
    bool _showSceneGraph{ false };
    void showSceneGraph();
    void showRenderNodeProps( IRenderable* const );
    std::function<void()> doVisit;
    std::function<bool(SceneObject&)> accumulatorPush;
    std::function<void(SceneObject&)> accumulatorPop;

    bool _showSettings{ false };
    void showSettings();
    std::function<void()> setExit;

    bool _showMainMenuBar{ true };
    void showMainMenuBar();

    bool _showStats{ false };
    void showStats();

    bool _showHelp{ false };
    void showHelp();

    bool _showAbout{ false };
    void showAbout();

    friend class boost::serialization::access;
    template<class Archive> void serialize(Archive&, const unsigned int);
};

BOOST_CLASS_EXPORT_KEY(ImGUI);