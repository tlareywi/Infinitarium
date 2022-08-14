//
//  Copyright � 2022 Blue Canvas Studios LLC. All rights reserved. Commercial use prohibited by license.
//

#include "ImGUI.hpp"
#include "ImGuiFileDialog.h"
#include "Scene.hpp"
#include "ConsoleInterface.hpp"
#include "Delegate.hpp"
#include "Application.hpp"
#include "Stats.hpp"

#include <glm/glm.hpp>

BOOST_CLASS_EXPORT_IMPLEMENT(ImGUI)

static void HelpMarker(const char* desc)
{
	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

ImGUI::ImGUI() {
	platformGUI = IImGUI::Create();

	// Subscribe to ESC event for showing/hiding UI
	auto fun = [this](int key) {
		_showMainMenuBar = !_showMainMenuBar;
	};
	std::shared_ptr<IDelegate> delegate = std::make_shared<EventDelegate<decltype(fun), int>>(fun);
	IApplication::Create()->subscribe("ESC", delegate);

	// Accumulator functions for building tree representation of scene graph.
	accumulatorPush = [this](SceneObject& obj) {
		ImGui::PushID(reinterpret_cast<unsigned long long>(&obj));

		ImGui::AlignTextToFramePadding();
		bool open = ImGui::TreeNode((void*)&obj, "%s", type_name<decltype(obj)>().c_str());
		ImGui::NextColumn();
		ImGui::AlignTextToFramePadding();
        ImGui::Text("%s", obj.getName().c_str());
		ImGui::NextColumn();

		if (!open) {
			ImGui::PopID();
			return false;
		}
		else {
			IRenderable* renderNode{ dynamic_cast<IRenderable*>(&obj) };
			if (renderNode) {
				showRenderNodeProps(renderNode);
			}

			return true;
		}
	};

	accumulatorPop = [](SceneObject& obj) {
		ImGui::TreePop();
		ImGui::PopID();
	};

	findNavNodes = [this](SceneObject& obj) {
		INavigatable* navNode{ dynamic_cast<INavigatable*>(&obj) };
		if (navNode)
			tabDefs.push_back(std::make_pair(navNode->label(), std::move(Database::instance().execute(navNode->query()))));

		return true;
	};
}

ImGUI::~ImGUI() {
	platformGUI = nullptr;
}

void ImGUI::prepare(IRenderContext& context) {
	platformGUI->prepare(context);

	toggleFullScreen = [&context]() {
		context.toggleFullScreen();
	};
}

void ImGUI::update(UpdateParams& params) {
	if (_showMainMenuBar) {
		Scene& scene{ params.getScene() };
		doVisit = [&scene, this]() {
			scene.visit(Accumulator(accumulatorPush, accumulatorPop));
		};
		scrapeNavNodes = [&scene, this]() {
			scene.visit(Visitor(findNavNodes));
		};
		setExit = [&scene]() {
			scene.terminatePending();
		};
		loadScene = [&scene]( const std::string& path ) {
			scene.prepareLoadScene( path );
		};
        getScene = [&scene]() -> Scene& {
            return scene;
        };

		platformGUI->update();
	}
}

void ImGUI::render(IRenderPass& renderPass) {
	if (!_showMainMenuBar)
		return;
	 
	if (dirty) {
		// Currently, there's no case under which ImGUI needs to be re-initialized. A framebuffer/swapchain resize 'just works'. How nice. 
		dirty = false;
	}
				
	platformGUI->apply(renderPass);

	setStyles();
	ImGui::NewFrame();

	showMainMenuBar();

    if (_showCalendar)
        showCalendar();
	if (_showSceneGraph)
		showSceneGraph();
	if (_showSettings)
		showSettings();
	if (_showStats)
		showStats();
	if (_showHelp)
		showHelp();
	if (_showAbout)
		showAbout();
	if (_showLoad)
		showLoad();
	if (_showNavigation)
		showNavigation();

	// Really nice for debugging and design ideas. 
    // bool show_demo_window{ true };
	// ImGui::ShowDemoWindow(&show_demo_window);

	ImGui::Render();
	platformGUI->render(renderPass);
}

void ImGUI::showAbout() {
	ImGui::SetNextWindowSize(ImVec2(640, 240), ImGuiCond_FirstUseEver);
	if (!ImGui::Begin("About Infinitarium", &_showAbout)) {
		ImGui::End();
		return;
	}

	ImGui::Text("Infinitarium Pre-Alpha\n\n(c)2020 Blue Canvas Studios LLC\n\nSee LICENSE for 3rd party acknowledgements.");

	ImGui::End();
}

void ImGUI::showHelp() {
	ImGui::SetNextWindowSize(ImVec2(640, 480), ImGuiCond_FirstUseEver);
	if (!ImGui::Begin("Help Content", &_showHelp)) {
		ImGui::End();
		return;
	}

	ImGui::Text(""
		"ESC: Show/Hide all UI elements\n"
		"Right Mouse Button Drag: Rotate Camera View");

	ImGui::End();
}

void ImGUI::showMainMenuBar() {

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Load", "")) {
				_showLoad = true;
				ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Load Scene", ".ieb", IApplication::Create()->getInstallationRoot() + "/share/Infinitarium/");
			}
			if (ImGui::MenuItem("Quit", ""))
				setExit();

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("View"))
		{
			if (ImGui::MenuItem("Toggle Fullscreen", ""))
				toggleFullScreen();

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Window"))
		{
            ImGui::MenuItem("Calendar", nullptr, &_showCalendar);
			ImGui::MenuItem("Navigation", nullptr, &_showNavigation);
			ImGui::MenuItem("SceneGraph", nullptr, &_showSceneGraph);
		//	ImGui::MenuItem("Settings", nullptr, &_showSettings);
			ImGui::MenuItem("Statistics", nullptr, &_showStats);
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Help")) {
			if (ImGui::MenuItem("Content", nullptr))
				_showHelp = true;
			ImGui::Separator();
			if( ImGui::MenuItem("About", nullptr) )
				_showAbout = true;

			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
}

void ImGUI::showCalendar() {
    ImGui::SetNextWindowSize(ImVec2(200, 380), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Calendar", &_showCalendar)) {
        ImGui::End();
        return;
    }
    
    Stats& stats{ Stats::Instance() };
    
    int multiplier{ getScene().timeMultiplier() };

    ImGui::Text("Simulation Time: %s", stats.simulationDateTime.c_str());
    
    if( ImGui::ArrowButton("DecTime", ImGuiDir_Left) ) {
        if( multiplier <= -10000000 )
            ;
        else if( multiplier == 0 )
            getScene().timeMultiplier( -1 );
        else if( multiplier == 1 )
            getScene().timeMultiplier( 0 );
        else if( multiplier > 1 )
            getScene().timeMultiplier( multiplier / 10 );
        else
            getScene().timeMultiplier( multiplier * 10 );
    }
    ImGui::SameLine();
    if( ImGui::ArrowButton("IncTime", ImGuiDir_Right) ) {
        if( multiplier >= 10000000 )
            ;
        else if( multiplier == 0 )
            getScene().timeMultiplier( 1 );
        else if( multiplier == -1 )
            getScene().timeMultiplier( 0 );
        else if( multiplier < 1 )
            getScene().timeMultiplier( multiplier / 10 );
        else
            getScene().timeMultiplier( multiplier * 10 );
    }
    ImGui::SameLine();
    ImGui::Text("Time Multiplier: %i", multiplier);
    
    ImGui::End();
}

void ImGUI::showLoad() {
	// https://github.com/aiekick/ImGuiFileDialog/tree/Lib_Only

	// display
	if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey"))
	{
		// action if OK
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
			//std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
			loadScene( filePathName );
		}

		ImGuiFileDialog::Instance()->Close();
	}
}

void ImGUI::showNavigation() {
	static bool init{ false };
	if (!init) { // Expensive and generally doesn't change at runtime. Do once.
		scrapeNavNodes();
		init = true;
	}

	ImGui::SetNextWindowSize(ImVec2(480, 540), ImGuiCond_FirstUseEver);
	if (!ImGui::Begin("Navigation", &_showNavigation)) {
		ImGui::End();
		return;
	}
    
    static int item_current_idx = 0;
    static std::string objID;
    
    ImGui::SameLine();
    if (ImGui::Button("Center", ImVec2(80,20))) {
        std::tuple<const std::string&, float> args( objID, 3.0f );
        Event e( args );
        e.setName("lookAt");
        IApplication::Create()->invoke( e );
    }
    ImGui::SameLine();
    if (ImGui::Button("Select", ImVec2(80,20))) {
        std::tuple<const std::string&> args( objID );
        Event e( args );
        e.setName("select");
        IApplication::Create()->invoke( e );
    }
    ImGui::SameLine();
    if (ImGui::Button("Tether", ImVec2(80,20))) {
        std::tuple<const std::string&> args( objID );
        Event e( args );
        e.setName("tether");
        IApplication::Create()->invoke( e );
    }
    ImGui::SameLine();
    if (ImGui::Button("Track", ImVec2(80,20))) {
        std::tuple<const std::string&> args( objID );
        Event e( args );
        e.setName("track");
        IApplication::Create()->invoke( e );
    }
    ImGui::NewLine();
    
	static constexpr char* const names[1] = { "Common Stars" };
	bool opened[1] = { true };

	if (!ImGui::BeginTabBar("Navbar", ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_NoCloseWithMiddleMouseButton)) {
		ImGui::End();
		return;
	}

	for( auto& tab : tabDefs ) {
		if( ImGui::BeginTabItem(tab.first.c_str(), nullptr, ImGuiTabItemFlags_None) ) {
			ImVec2 size(-FLT_MIN, -FLT_MIN); // Fill window

			if( ImGui::BeginListBox(("##" + tab.first).c_str(), size) ) {
				unsigned int rowIndx{ 0 };
				for (auto& row : tab.second) {
					const bool is_selected = (item_current_idx == rowIndx);
					if( ImGui::Selectable(row[0].c_str(), is_selected, ImGuiSelectableFlags_AllowDoubleClick) ) {
						item_current_idx = rowIndx;
                        objID = row[0];
                        if( ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) ) {
                            std::tuple<const std::string&, float> args( objID, 3.0f );
                            Event e( args );
                            e.setName("lookAt"); // TODO: Make event IDs enums
                            IApplication::Create()->invoke( e );
                        }
					}

					// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
					if( is_selected )
						ImGui::SetItemDefaultFocus();

					++rowIndx;
				}

				ImGui::EndListBox();
			}

			ImGui::EndTabItem();
		}
	}

	ImGui::EndTabBar();

	ImGui::End();
}

void ImGUI::showRenderNodeProps(IRenderable* const renderable) {
	for (auto& uniform : renderable->getUniforms()) {
		ImGui::PushID(reinterpret_cast<unsigned long long>(&uniform));

		ImGui::AlignTextToFramePadding();
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet;
		ImGui::TreeNodeEx((void*)&uniform, flags, "%s", uniform.first.c_str());
		ImGui::NextColumn();
		ImGui::SetNextItemWidth(-1);

		std::visit(overload{
			[&uniform](float val) {
				float min{ std::get<float>(uniform.second.min) };
				float max{ std::get<float>(uniform.second.max) };
				ImGui::SliderScalar("", ImGuiDataType_Float, &val, &min, &max);
				uniform.second.val = val;
			},
			[&uniform](int val) {
				int min{ std::get<int>(uniform.second.min) };
				int max{ std::get<int>(uniform.second.max) };
				ImGui::SliderScalar("", ImGuiDataType_S32, &val, &min, &max);
				uniform.second.val = val;
			},
			[&uniform](unsigned int val) {
				unsigned int min{ std::get<unsigned int>(uniform.second.min) };
				unsigned int max{ std::get<unsigned int>(uniform.second.max) };
				ImGui::SliderScalar("", ImGuiDataType_U32, &val, &min, &max);
				uniform.second.val = val;
			},
			[&uniform](glm::ivec2 v) {
				glm::ivec2 vec{ std::get<glm::ivec2>(uniform.second.val) };
				ImGui::Text("x: %i, y: %i", vec.x, vec.y);
			},
			[&uniform](glm::ivec3 v) {
				glm::ivec3 vec{ std::get<glm::ivec3>(uniform.second.val) };
				ImGui::Text("x: %i, y: %i, z: %i", vec.x, vec.y, vec.z);
			},
			[&uniform](glm::ivec4 v) {
				glm::ivec4 vec{ std::get<glm::ivec4>(uniform.second.val) };
				ImGui::Text("x: %i, y: %i, z: %i, w: %i", vec.x, vec.y, vec.z, vec.w);
			},
			[&uniform](glm::mat4x4 v) {
				// Not Yet Implemented
			},
			[&uniform](glm::uvec2 v) {
				glm::uvec2 vec{ std::get<glm::uvec2>(uniform.second.val) };
				ImGui::Text("x: %u, y: %u", vec.x, vec.y);
			},
			[&uniform](glm::uvec3 v) {
				glm::uvec3 vec{ std::get<glm::uvec3>(uniform.second.val) };
				ImGui::Text("x: %u, y: %u, z: %u", vec.x, vec.y, vec.z);
			},
		    [&uniform](glm::uvec4 v) {
				glm::uvec4 vec{ std::get<glm::uvec4>(uniform.second.val) };
				ImGui::Text("x: %u, y: %u, z: %u, w: %u", vec.x, vec.y, vec.z, vec.w);
			},
			[&uniform](glm::vec2 v) {
				glm::vec2 vec{ std::get<glm::vec2>(uniform.second.val) };
				ImGui::Text("x: %f, y: %f", vec.x, vec.y);
			},
			[&uniform](glm::vec3 v) {
				glm::vec3 vec{ std::get<glm::vec3>(uniform.second.val) };
				ImGui::Text("x: %f, y: %f, z: %f", vec.x, vec.y, vec.z );
			},
		    [&uniform](glm::vec4 v) {
				glm::vec4 vec{ std::get<glm::vec4>(uniform.second.val) };
				ImGui::Text("x: %f, y: %f, z: %f, w: %f", vec.x, vec.y, vec.z, vec.w );
			}
		}, uniform.second.val);

		ImGui::NextColumn();

		ImGui::PopID();
	}
}

void ImGUI::showSceneGraph() {
	ImGui::SetNextWindowSize(ImVec2(480, 640), ImGuiCond_FirstUseEver);
	if (!ImGui::Begin("SceneGraph", &_showSceneGraph))
	{
		ImGui::End();
		return;
	}

	HelpMarker(
		"Representation of all scenegraph nodes.\n"
		"Click an 'IRenderable' node to manipulate properties of that renderable; i.e. its uniforms."
	);

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
	ImGui::Columns(2);
	ImGui::Separator();

	doVisit();

	ImGui::Columns(1);
	ImGui::Separator();
	ImGui::PopStyleVar();
	ImGui::End();
}

void ImGUI::showSettings() {
	ImGui::SetNextWindowSize(ImVec2(200, 380), ImGuiCond_FirstUseEver);
	if (!ImGui::Begin("Settings", &_showSettings)) {
		ImGui::End();
		return;
	}

//	ImVec2 pos((ImGui::GetWindowSize().x - 80) * 0.5, ImGui::GetCursorPosY());
//	ImGui::SetCursorPos(pos);
//	if (ImGui::Button("Exit", ImVec2(80,20))) {
//		setExit();
//	}

	ImGui::End();
}

void ImGUI::showStats() {
	Stats& stats{ Stats::Instance() };

	ImGui::SetNextWindowSize(ImVec2(200, 380), ImGuiCond_FirstUseEver);
	if (!ImGui::Begin("Statistics", &_showStats)) {
		ImGui::End();
		return;
	}

	HelpMarker(
		"FPS is a 60 frame rolling average.\n"
	);

    ImGui::Text("Simulation Time: %s", stats.simulationDateTime.c_str());
	ImGui::Text("FPS            : %u", stats.fps);

	ImGui::Text("Eye            : (%f, %f, %f)", stats.eye.x, stats.eye.y, stats.eye.z);
	ImGui::Text("Center         : (%f, %f, %f)", stats.center.x, stats.center.y, stats.center.z);
	ImGui::Text("Up             : (%f, %f, %f)", stats.up.x, stats.up.y, stats.up.z);
	ImGui::Text("Distance       : %f", stats.distance);
    
	ImGui::End();
}

static constexpr bool bStyleDark_{ 1 };
static constexpr float alpha_{ 0.25 };

void ImGUI::setStyles() {
	ImGuiStyle& style = ImGui::GetStyle();

	style.Alpha = 1.0f;
	style.FrameRounding = 3.0f;

	style.Colors[ImGuiCol_Text] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.94f, 0.94f, 0.94f, 0.94f);
	style.Colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.94f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.00f, 0.00f, 0.00f, 0.39f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(1.00f, 1.00f, 1.00f, 0.10f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.94f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.96f, 0.96f, 0.96f, 1.00f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.82f, 0.82f, 0.82f, 1.00f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 1.00f, 1.00f, 0.51f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.98f, 0.98f, 0.98f, 0.53f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.69f, 0.69f, 0.69f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.59f, 0.59f, 0.59f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.59f, 0.78f, 1.00f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.78f, 1.00f);
	style.Colors[ImGuiCol_Header] = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_Separator] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
	style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.78f);
	style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.50f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
	style.Colors[ImGuiCol_Tab] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
	style.Colors[ImGuiCol_TabHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_TabActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
	style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
	style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.00f, 0.00f, 0.00f, 0.39f);
	style.Colors[ImGuiCol_TableBorderLight] = ImVec4(1.00f, 1.00f, 1.00f, 0.10f);
	style.Colors[ImGuiCol_TableRowBg]  = ImVec4(0.96f, 0.96f, 0.96f, 0.15f);
	style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.30f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
//	ImGuiCol_NavHighlight,          // Gamepad/keyboard: current highlighted item
//		ImGuiCol_NavWindowingHighlight, // Highlight window when using CTRL+TAB
//		ImGuiCol_NavWindowingDimBg,     // Darken/colorize entire screen behind the CTRL+TAB window list, when active
	style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);

	if (bStyleDark_)
	{
		for (int i = 0; i <= ImGuiCol_COUNT; i++)
		{
			ImVec4& col = style.Colors[i];
			float H, S, V;
			ImGui::ColorConvertRGBtoHSV(col.x, col.y, col.z, H, S, V);

			if (S < 0.1f)
			{
				V = 1.0f - V;
			}
			ImGui::ColorConvertHSVtoRGB(H, S, V, col.x, col.y, col.z);
			if (col.w < 1.00f)
			{
				col.w *= alpha_;
			}
		}
	}
	else
	{
		for (int i = 0; i <= ImGuiCol_COUNT; i++)
		{
			ImVec4& col = style.Colors[i];
			if (col.w < 1.00f)
			{
				col.x *= alpha_;
				col.y *= alpha_;
				col.z *= alpha_;
				col.w *= alpha_;
			}
		}
	}
}

template<class Archive> void ImGUI::serialize(Archive& ar, unsigned int version) {
	boost::serialization::void_cast_register<ImGUI, IRenderable>();
	ar& boost::serialization::make_nvp("IRenderable", boost::serialization::base_object<IRenderable>(*this));
}

BOOST_CLASS_EXPORT_IMPLEMENT(INavigatable)

template<class Archive> void INavigatable::serialize(Archive& ar, unsigned int version) {
	ar & queryStr;
	ar & tabLabel;
}
