#include <LevelEditor/LevelEditor.h>
#include <LevelEditor/LevelSerializer.h>
#include <LevelEditor/ECS/ecsLevelEditor.h>

#include <Filesystem.h>
#include <ECS/ecsSystems.h>
#include <Parser/WorldParser.h>

#include <imgui.h>
#include <imgui_stdlib.h>

namespace GameEngine
{
	namespace Editor
	{
		LevelEditor::LevelEditor(flecs::world& world):
                m_World(&world)
		{
			m_Level = LevelSerializer::Deserialize(Core::g_FileSystem->GetFilePath("Levels/Main.xml").generic_string());
            m_Level->GetLevelObjects().reserve(100);

			for (World::LevelObject& levelObject : m_Level->GetLevelObjects())
			{
				RegisterNewLevelObject(levelObject);
			}

			EntitySystem::LevelEditorECS::RegisterLevelEditorEcsSystems(world);
		}

        void LevelEditor::RegisterNewLevelObject(World::LevelObject& levelObject)
        {
            flecs::entity entity = m_World->entity(levelObject.GetName().c_str());

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

		void LevelEditor::Draw()
		{
			ImGui::Begin(GetName());

			if (m_Level.has_value()) [[likely]]
			{
				for (World::LevelObject& levelObject : m_Level->GetLevelObjects())
				{
					if (!ImGui::TreeNode(levelObject.GetName().c_str())) continue;

                    for (World::LevelObject::Component& component : levelObject.GetComponents())
                    {
                        if (component.first == "Position")
                        {
                            float pos[3] = {0, 0, 0};
                            sscanf(component.second.c_str(), "%f, %f, %f", &pos[0], &pos[1], &pos[2]);

                            if (!ImGui::InputFloat3(component.first.c_str(), pos)) continue;

                            component.second = std::to_string(pos[0]) + ", " + std::to_string(pos[1]) + ", " + std::to_string(pos[2]);

                            continue;
                        }

                        ImGui::InputText(component.first.c_str(), &component.second);
                    }

                    ImGui::TreePop();

				}
			}

			if (ImGui::Button("Save"))
			{
				m_SaveButtonMessageTimer.Reset();
				m_SaveButtonPressed = true;

				Save();
			}

            ImGui::SameLine();

            if (ImGui::Button("New object"))
            {
                static int objCounter = 1;
                std::string objName = "NewObject" + std::to_string(objCounter++);

                World::LevelObject newObject;
                newObject.SetName(objName.c_str());
                newObject.AddComponent("Position", "0.0, 0.0, 0.0");
                newObject.AddComponent("GeometryPtr", "Cube");
                
                m_Level->AddLevelObject(newObject);
                // наша система обновлений позиции в редакторе
                // привязана к адресам, а вектор очень любит эти адреса менять
                // поэтому для работы с такой системой нужно закрепить адрес компонента,
                // запретив релокацию обекта и компонента
                m_Level->GetLevelObjects().back().GetComponents().reserve(100);
                RegisterNewLevelObject(m_Level->GetLevelObjects().back());
            }

            ImGui::SameLine();
            if (ImGui::Button("Delete all"))
            {
                ImGui::OpenPopup("Confirmation");
            }

            if (ImGui::BeginPopupModal("Confirmation"))
            {
                ImGui::Text("Are you sure you want to delete ALL objects?");
                if (ImGui::Button("Yes"))
                {
                    DeleteAllObjects();
                    ImGui::CloseCurrentPopup();
                }
                ImGui::SameLine();
                if (ImGui::Button("No"))
                {
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }

			if (m_SaveButtonPressed)
			{
				ImGui::Text("Saved!");
			}

            if (m_DeleteButtonPressed)
            {
                ImGui::Text("Deleted %d objects!", m_DeletedObjectsCount);
            }

			ImGui::End();
		}

		void LevelEditor::Update(float dt)
		{
			m_SaveButtonMessageTimer.Tick();
            m_DeleteButtonMessageTimer.Tick();

            if (m_SaveButtonMessageTimer.GetTotalTime() > m_TimeToShowSaveButtonMessage) m_SaveButtonPressed = false;
            if (m_DeleteButtonMessageTimer.GetTotalTime() > m_TimeToShowDeleteMessage) m_DeleteButtonPressed = false;
		}

		void LevelEditor::Save()
		{
			assert(m_Level.has_value());
			LevelSerializer::Serialize(Core::g_FileSystem->GetFilePath("Levels/Main.xml").generic_string(), m_Level.value());
		}

        void LevelEditor::DeleteAllObjects()
		{
			if (!m_Level.has_value()) return;

            std::vector<std::string> objectNames;
            for (const auto& obj : m_Level->GetLevelObjects()) objectNames.push_back(obj.GetName());

            for (const auto& name : objectNames)
            {
                flecs::entity e = m_World->lookup(name.c_str());
                e.destruct();
            }

            m_DeletedObjectsCount = static_cast<int>(m_Level->GetLevelObjects().size());
            m_Level->GetLevelObjects().clear();

            m_DeleteButtonMessageTimer.Reset();
            m_DeleteButtonPressed = true;
		}
	}
}