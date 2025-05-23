#include <LevelEditor/LevelEditor.h>
#include <LevelEditor/LevelSerializer.h>
#include <LevelEditor/ECS/ecsLevelEditor.h>

#include <Filesystem.h>
#include <ECS/ecsSystems.h>
#include <Parser/WorldParser.h>

#include <imgui.h>
#include <imgui_stdlib.h>

namespace GameEngine {
namespace Editor {

LevelEditor::LevelEditor(flecs::world& world):
		m_World(world)
{
	m_Level = LevelSerializer::Deserialize(Core::g_FileSystem->GetFilePath("Levels/Main.xml").generic_string());

	for (auto& levelObjectPtr : m_Level->GetLevelObjects())
	{
		World::LevelObject& levelObject = *levelObjectPtr;
		flecs::entity entity = world.entity(levelObject.GetName().c_str());
		World::LevelObject::ComponentList& componentList = levelObject.GetComponents();

		World::LevelObject::ComponentList::iterator positionAttribute = std::ranges::find_if(componentList,
			[](World::LevelObject::Component& component)
			{
				return !std::strcmp(component.first.c_str(), "Position");
			}
		);

		World::LevelObject::ComponentList::iterator geometryAttribute = std::ranges::find_if(componentList,
			[](World::LevelObject::Component& component)
			{
				return !std::strcmp(component.first.c_str(), "GeometryPtr");
			}
		);

		if (positionAttribute != componentList.end() &&
			geometryAttribute != componentList.end())
		{
			assert(World::WorldParser::GetCustomComponents().contains(geometryAttribute->second));

			entity.set(EntitySystem::LevelEditorECS::PositionDesc{ &positionAttribute->second });

			// Can be set to 0 since it doesn't matter now, will be updated by the system
			entity.set(EntitySystem::EditorECS::Position{ 0.0f, 0.0f, 0.0f });
			entity.set(GeometryPtr{
				reinterpret_cast<RenderCore::Geometry*>(
					World::WorldParser::GetCustomComponents()[geometryAttribute->second]
					)
				});
		}
	}

	EntitySystem::LevelEditorECS::RegisterLevelEditorEcsSystems(world);
}

void LevelEditor::Draw()
{
	ImGui::Begin(GetName());

	static char searchText[256] = "";
	ImGui::InputTextWithHint("##Search", "Search...", searchText, sizeof(searchText));

	if (m_Level.has_value()) DrawObjects(searchText);

	if (ImGui::Button("Save"))
	{
		m_SaveButtonMessageTimer.Reset();
		m_SaveButtonPressed = true;

		Save();
	}

	if (m_SaveButtonPressed)
	{
		ImGui::SameLine();
		ImGui::Text("Saved!");
	}

	ImGui::End();
}

void LevelEditor::DrawObjects(const char* searchFilter)
{
	static std::optional<size_t> editingIndex;
	static char editBuffer[256];
	static std::vector<size_t> selectedIndices;
	static std::optional<size_t> lastSelectedIndex;

	const bool isCtrlPressed  = ImGui::GetIO().KeyCtrl;
	const bool isShiftPressed = ImGui::GetIO().KeyShift;

	for (size_t i = 0; i < m_Level->GetLevelObjects().size(); ++i)
	{
		World::LevelObject& levelObject = *m_Level->GetLevelObjects()[i];
		if (searchFilter[0] != '\0' && levelObject.GetName().find(searchFilter) == std::string::npos) continue;

		bool isEditing = editingIndex == i;
		bool isSelected = std::find(selectedIndices.begin(), selectedIndices.end(), i) != selectedIndices.end();
		
		if (isSelected)
		{
			ImGui::PushStyleColor(ImGuiCol_Header, ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]);
			ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered]);
		}

		bool isComponentsShown = ImGui::TreeNodeEx((void*)(intptr_t)i, ImGuiTreeNodeFlags_FramePadding | (isSelected ? ImGuiTreeNodeFlags_Selected : 0), "");

		if (isSelected) ImGui::PopStyleColor(2);
		ImGui::SameLine(ImGui::GetTreeNodeToLabelSpacing() + 8);

		if (isEditing)
		{
			strncpy(editBuffer, levelObject.GetName().c_str(), sizeof(editBuffer));
			ImGui::SetNextItemWidth(-1);
			if (ImGui::InputText("##edit", editBuffer, sizeof(editBuffer), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
			{
				levelObject.SetName(editBuffer);
				editingIndex.reset();
			}
			if (ImGui::IsMouseClicked(0) && !ImGui::IsItemHovered()) editingIndex.reset();
		}
		else
		{
			if (searchFilter[0] != '\0') ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 0, 1));
			
			if (ImGui::Selectable(levelObject.GetName().c_str(), isSelected, ImGuiSelectableFlags_AllowDoubleClick | ImGuiSelectableFlags_SpanAllColumns))
			{
				if (isShiftPressed && lastSelectedIndex)
				{
					selectedIndices.clear();
					size_t start = std::min(*lastSelectedIndex, i);
					size_t end   = std::max(*lastSelectedIndex, i);
					for (size_t rangeIdx = start; rangeIdx <= end; ++rangeIdx) selectedIndices.push_back(rangeIdx);
				}
				else if (isCtrlPressed)
				{
					auto it = std::find(selectedIndices.begin(), selectedIndices.end(), i);
					if (it != selectedIndices.end()) selectedIndices.erase(it);
					else
					{
						selectedIndices.push_back(i);
						lastSelectedIndex = i;
					}
				}
				else
				{
					selectedIndices.clear();
					selectedIndices.push_back(i);
					lastSelectedIndex = i;
				}

				if (ImGui::IsMouseDoubleClicked(0))
				{
					editingIndex = i;
					lastSelectedIndex = i;
					if (std::find(selectedIndices.begin(), selectedIndices.end(), i) == selectedIndices.end())
					{
						selectedIndices.clear();
						selectedIndices.push_back(i);
					}
				}
			}
			
			if (searchFilter[0] != '\0') ImGui::PopStyleColor();
		}

		if (isComponentsShown)
		{
			for (World::LevelObject::Component& component : levelObject.GetComponents()) ImGui::InputText(component.first.c_str(), &component.second);
			ImGui::TreePop();
		}
	}

	if (ImGui::IsMouseClicked(0) && !ImGui::IsAnyItemHovered() && ImGui::IsWindowHovered() && !isCtrlPressed)
	{
		selectedIndices.clear();
		lastSelectedIndex.reset();
	}

	if (!selectedIndices.empty() && ImGui::IsKeyPressed(ImGuiKey_Delete))
	{
		// Sort to keep indexes valid after deletion
		std::sort(selectedIndices.begin(), selectedIndices.end(), std::greater<size_t>());
		
		for (size_t index : selectedIndices)
		{
			auto iter = m_Level->GetLevelObjects().begin() + index;
			flecs::entity e = m_World.lookup((*iter)->GetName().c_str());
			e.destruct();
			m_Level->GetLevelObjects().erase(iter);
		}
		
		selectedIndices.clear();
		lastSelectedIndex.reset();
		editingIndex.reset();
	}
}

void LevelEditor::Update(float dt)
{
	m_SaveButtonMessageTimer.Tick();

	if (m_SaveButtonMessageTimer.GetTotalTime() > m_TimeToShowSaveButtonMessage)
	{
		m_SaveButtonPressed = false;
	}
}

void LevelEditor::Save()
{
	assert(m_Level.has_value());
	LevelSerializer::Serialize(Core::g_FileSystem->GetFilePath("Levels/Main.xml").generic_string(), m_Level.value());
}
}}