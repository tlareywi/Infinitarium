//
//  Copyright � 2022 Blue Canvas Studios LLC. All rights reserved. Commercial use prohibited by license.
//

#pragma once

#include "Renderable.hpp"
#include "IImGUI.hpp"
#include "Database.hpp"

#include "imgui.h"

/// <summary>
/// GUI implemention walks the scene and will add a Navigation tab for all objects implementing INavigatable,
/// populating the tab with the result set from the query string.
/// </summary>
class INavigatable {
public:    
    const std::string& query() {
        return queryStr;
    }
    const std::string label() {
        return tabLabel;
    }
    void setQuery(const std::string& q) {
        queryStr = q;
    }
    void setLabel(const std::string& l) {
        tabLabel = l;
    }

private:
    std::string queryStr;
    std::string tabLabel;

    friend class boost::serialization::access;
    template<class Archive> void serialize(Archive&, const unsigned int);
};

BOOST_CLASS_EXPORT_KEY(INavigatable);


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
    
    void setStyles();

    bool _showSceneGraph{ false };
    void showSceneGraph();
    void showRenderNodeProps( IRenderable* const );
    std::function<void()> doVisit;
    std::function<void(const std::string&)> loadScene;
    std::function<bool(SceneObject&)> accumulatorPush;
    std::function<void(SceneObject&)> accumulatorPop;

    std::function<void()> toggleFullScreen;

    bool _showSettings{ false };
    void showSettings();
    std::function<void()> setExit;

    bool _showMainMenuBar{ true };
    void showMainMenuBar();
    
    bool _showCalendar{ false };
    void showCalendar();

    bool _showLoad{ false };
    void showLoad();

    bool _showStats{ false };
    void showStats();

    bool _showHelp{ false };
    void showHelp();

    bool _showAbout{ false };
    void showAbout();

    std::vector<std::pair<std::string, Database::ResultSet>> tabDefs;
    std::function<void()> scrapeNavNodes;
    std::function<Scene&()> getScene;
    std::function<bool(SceneObject&)> findNavNodes;
    bool _showNavigation{ false };
    void showNavigation();

    friend class boost::serialization::access;
    template<class Archive> void serialize(Archive&, const unsigned int);
};

BOOST_CLASS_EXPORT_KEY(ImGUI);
