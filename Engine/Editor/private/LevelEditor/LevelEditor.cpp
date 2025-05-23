#include <LevelEditor/LevelEditor.h>
#include <LevelEditor/LevelSerializer.h>
#include <LevelEditor/ECS/ecsLevelEditor.h>

#include <Filesystem.h>
#include <ECS/ecsSystems.h>
#include <Parser/WorldParser.h>

#include <imgui.h>
#include <imgui_stdlib.h>

#include <map>

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
		if (!levelObject.GetFolder().empty()) m_Foldernames.insert(levelObject.GetFolder());

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

		World::LevelObject::ComponentList::iterator folderAttribute = std::ranges::find_if(componentList,
			[](World::LevelObject::Component& component)
			{
				return !std::strcmp(component.first.c_str(), "Folder");
			}
		);
	}

	EntitySystem::LevelEditorECS::RegisterLevelEditorEcsSystems(world);
}

void LevelEditor::Draw()
{
	ImGui::Begin(GetName());

	static char searchText[256] = "";
	ImGui::InputTextWithHint("##Search", "Search...", searchText, sizeof(searchText));

	if (ImGui::Button("Create Folder")) ImGui::OpenPopup("Create New Folder");

	if (!m_SelectedIndexes.empty() && !m_Foldernames.empty())
	{
		ImGui::SameLine();
		if (ImGui::Button("Add to Folder")) ImGui::OpenPopup("Select Folder To Add");
	
		ImGui::SameLine();
		if (ImGui::Button("Remove from Folder"))
		{
			for (size_t index : m_SelectedIndexes) m_Level->GetLevelObjects()[index]->SetFolder("");
		}
	}

	if (ImGui::BeginPopupModal("Create New Folder", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		static char folderName[128] = "";
		ImGui::InputText("Folder Name", folderName, sizeof(folderName));
		
		if (ImGui::Button("Create"))
		{
			if (strlen(folderName) > 0)
			{
				m_Foldernames.insert(folderName);
				folderName[0] = '\0';
				ImGui::CloseCurrentPopup();
			}
		}
		
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			folderName[0] = '\0';
			ImGui::CloseCurrentPopup();
		}
		
		ImGui::EndPopup();
	}

	if (ImGui::BeginPopup("Select Folder To Add"))
	{
		for (const auto& folder : m_Foldernames)
		{
			if (!ImGui::MenuItem(folder.c_str())) continue;
			for (size_t index : m_SelectedIndexes) m_Level->GetLevelObjects()[index]->SetFolder(folder);
		}
		ImGui::EndPopup();
	}

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
	for (const auto& folder : m_Foldernames)
	{
		if (!ImGui::TreeNodeEx(folder.c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_FramePadding)) continue;
		for (size_t i = 0; i < m_Level->GetLevelObjects().size(); ++i)
		{
			World::LevelObject& levelObject = *m_Level->GetLevelObjects()[i];
			if (levelObject.GetFolder() != folder) continue;
			if (searchFilter[0] != '\0' && levelObject.GetName().find(searchFilter) == std::string::npos) continue;

			DrawObject(i, levelObject);
		}
		ImGui::TreePop();
	}

	for (size_t i = 0; i < m_Level->GetLevelObjects().size(); ++i)
	{
		World::LevelObject& levelObject = *m_Level->GetLevelObjects()[i];
		if (!levelObject.GetFolder().empty()) continue;
		if (searchFilter[0] != '\0' && levelObject.GetName().find(searchFilter) == std::string::npos) continue;

		DrawObject(i, levelObject);
	}

	if (ImGui::IsMouseClicked(0) && !ImGui::IsAnyItemHovered() && ImGui::IsWindowHovered() && !m_IsCtrlPressed)
	{
		m_SelectedIndexes.clear();
		m_LastSelectedIndex.reset();
	}

	if (!m_SelectedIndexes.empty() && ImGui::IsKeyPressed(ImGuiKey_Delete))
	{
		std::sort(m_SelectedIndexes.begin(), m_SelectedIndexes.end(), std::greater<size_t>());
		
		for (size_t index : m_SelectedIndexes)
		{
			auto iter = m_Level->GetLevelObjects().begin() + index;
			flecs::entity e = m_World.lookup((*iter)->GetName().c_str());
			e.destruct();
			m_Level->GetLevelObjects().erase(iter);
		}
		
		m_SelectedIndexes.clear();
		m_LastSelectedIndex.reset();
		m_nameEditingIndex.reset();
	}
}

void LevelEditor::DrawObject(size_t i, World::LevelObject& levelObject)
{
	bool isNameEditing = m_nameEditingIndex == i;
	bool isSelected = std::find(m_SelectedIndexes.begin(), m_SelectedIndexes.end(), i) != m_SelectedIndexes.end();
	
	if (isSelected)
	{
		ImGui::PushStyleColor(ImGuiCol_Header, ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]);
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered]);
	}

	bool isComponentsShown = ImGui::TreeNodeEx((void*)(intptr_t)i, ImGuiTreeNodeFlags_FramePadding | (isSelected ? ImGuiTreeNodeFlags_Selected : 0), "");

	if (isSelected) ImGui::PopStyleColor(2);
	ImGui::SameLine(ImGui::GetTreeNodeToLabelSpacing() + (levelObject.GetFolder().empty() ? 8 : 30));

	static char nameEditBuffer[256];
	if (isNameEditing)
	{
		strncpy(nameEditBuffer, levelObject.GetName().c_str(), sizeof(nameEditBuffer));
		ImGui::SetNextItemWidth(-1);
		if (ImGui::InputText("##edit", nameEditBuffer, sizeof(nameEditBuffer), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
		{
			levelObject.SetName(nameEditBuffer);
			m_nameEditingIndex.reset();
		}
		if (ImGui::IsMouseClicked(0) && !ImGui::IsItemHovered()) m_nameEditingIndex.reset();
	}
	else
	{
		if (ImGui::Selectable(levelObject.GetName().c_str(), isSelected, ImGuiSelectableFlags_AllowDoubleClick | ImGuiSelectableFlags_SpanAllColumns))
		{
			//if (isShiftPressed && m_LastSelectedIndex)
			//{
			//	m_SelectedIndexes.clear();
			//	size_t start = std::min(*m_LastSelectedIndex, i);
			//	size_t end   = std::max(*m_LastSelectedIndex, i);
			//	for (size_t rangeIdx = start; rangeIdx <= end; ++rangeIdx) m_SelectedIndexes.push_back(rangeIdx);
			//} else
			if (m_IsCtrlPressed)
			{
				auto it = std::find(m_SelectedIndexes.begin(), m_SelectedIndexes.end(), i);
				if (it != m_SelectedIndexes.end()) m_SelectedIndexes.erase(it);
				else
				{
					m_SelectedIndexes.push_back(i);
					m_LastSelectedIndex = i;
				}
			}
			else
			{
				m_SelectedIndexes.clear();
				m_SelectedIndexes.push_back(i);
				m_LastSelectedIndex = i;
			}

			if (ImGui::IsMouseDoubleClicked(0))
			{
				m_nameEditingIndex = i;
				m_LastSelectedIndex = i;
				if (std::find(m_SelectedIndexes.begin(), m_SelectedIndexes.end(), i) == m_SelectedIndexes.end())
				{
					m_SelectedIndexes.clear();
					m_SelectedIndexes.push_back(i);
				}
			}
		}
	}

	if (isComponentsShown)
	{
		for (World::LevelObject::Component& component : levelObject.GetComponents())
		{
			ImGui::SetNextItemWidth(150);
			ImGui::InputText(component.first.c_str(), &component.second);
		}
		ImGui::TreePop();
	}
}

void LevelEditor::Update(float dt)
{
	m_IsCtrlPressed = ImGui::GetIO().KeyCtrl;
	
	m_SaveButtonMessageTimer.Tick();
	if (m_SaveButtonMessageTimer.GetTotalTime() > m_TimeToShowSaveButtonMessage) m_SaveButtonPressed = false;
}

void LevelEditor::Save()
{
	assert(m_Level.has_value());
	LevelSerializer::Serialize(Core::g_FileSystem->GetFilePath("Levels/Main.xml").generic_string(), m_Level.value());
}
}}