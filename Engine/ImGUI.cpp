#include "ImGUI.hpp"
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
		ImGui::Text(obj.getName().c_str());
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
		setExit = [&scene]() {
			scene.terminatePending();
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

	ImGui::NewFrame();

	showMainMenuBar();

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

	// Really nice for debugging and design ideas. 
	//	bool show_demo_window{ true };
	//	ImGui::ShowDemoWindow(&show_demo_window);

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
			[&uniform](int v) {
			},
			[&uniform](unsigned int v) {
			},
			[&uniform](glm::ivec2 v) {
			},
			[&uniform](glm::ivec3 v) {
			},
			[&uniform](glm::ivec4 v) {
			},
			[&uniform](glm::mat4x4 v) {
			},
			[&uniform](glm::uvec2 v) {
			},
			[&uniform](glm::uvec3 v) {
			},
		    [&uniform](glm::uvec4 v) {
			},
			[&uniform](glm::vec2 v) {
			},
			[&uniform](glm::vec3 v) {
			},
		    [&uniform](glm::vec4 v) {
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

	ImGui::Text("FPS   : %u", stats.fps);

	ImGui::Text("Eye   : (%f, %f, %f)", stats.eye.x, stats.eye.y, stats.eye.z);
	ImGui::Text("Center: (%f, %f, %f)", stats.center.x, stats.center.y, stats.center.z);
	ImGui::Text("Up    : (%f, %f, %f)", stats.up.x, stats.up.y, stats.up.z);

	ImGui::End();
}

template<class Archive> void ImGUI::serialize(Archive& ar, unsigned int version) {
	boost::serialization::void_cast_register<ImGUI, IRenderable>();
	ar& boost::serialization::make_nvp("IRenderable", boost::serialization::base_object<IRenderable>(*this));
}