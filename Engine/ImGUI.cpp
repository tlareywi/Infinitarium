#include "ImGUI.hpp"
#include "Scene.hpp"
#include "ConsoleInterface.hpp"
#include "Delegate.hpp"

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
				ShowRenderNodeProps(renderNode);
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
}

void ImGUI::update(UpdateParams& params) {
	if (showSceneGraph) {
		Scene& scene{ params.getScene() };
		doVisit = [&scene, this]() {
			scene.visit(Accumulator(accumulatorPush, accumulatorPop));
		};
	}

	platformGUI->update();
}

void ImGUI::render(IRenderPass& renderPass) {
	platformGUI->apply(renderPass);

	ImGui::NewFrame();

	if( showSceneGraph )
		ShowSceneGraph();

	//ImGui::ShowDemoWindow(&show_demo_window);

	ImGui::Render();

	platformGUI->render(renderPass);
}

void ImGUI::ShowRenderNodeProps(IRenderable* const renderable) {
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

void ImGUI::ShowSceneGraph() {
	ImGui::SetNextWindowSize(ImVec2(480, 640), ImGuiCond_FirstUseEver);
	if (!ImGui::Begin("SceneGraph", &showSceneGraph))
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

template<class Archive> void ImGUI::serialize(Archive& ar, unsigned int version) {
	boost::serialization::void_cast_register<ImGUI, IRenderable>();
	ar& boost::serialization::make_nvp("IRenderable", boost::serialization::base_object<IRenderable>(*this));
}