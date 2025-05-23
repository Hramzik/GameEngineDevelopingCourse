#pragma once

#include <Level.h>
#include <UIWindow.h>
#include <Timer.h>
#include <flecs.h>

#include <set>

namespace GameEngine
{
	namespace Editor
	{
		class LevelEditor final : public GUI::UIWindow
		{
		public:
			LevelEditor() = delete;
			LevelEditor(flecs::world& world);

		public:
			virtual void Draw() override;
			void DrawObjects();
			void DrawObject(size_t i, World::LevelObject& levelObject);
			virtual void Update(float dt) override;
			virtual const char* GetName() const override { return "Level Editor"; }

		private:
			void Save();

		private:
			Core::Timer m_SaveButtonMessageTimer;
			bool m_SaveButtonPressed = false;
			bool m_IsCtrlPressed = false;
			float m_TimeToShowSaveButtonMessage = 3.f;

			std::optional<World::Level> m_Level = std::nullopt;
			flecs::world& m_World;

			std::set<std::string> m_Foldernames;
			char searchFilter[256] = "";
			std::vector<size_t> m_SelectedIndexes;
			std::optional<size_t> m_LastSelectedIndex;
			std::optional<size_t> m_nameEditingIndex;
		};
	}
}