#pragma once

#include <Level.h>
#include <UIWindow.h>
#include <Timer.h>
#include <flecs.h>

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
			virtual void Update(float dt) override;
			virtual const char* GetName() const override { return "Level Editor"; }

		private:
			void RegisterNewLevelObject(World::LevelObject& levelObject);
			void Save();
			void DeleteAllObjects();

		private:
			Core::Timer m_SaveButtonMessageTimer;
			bool m_SaveButtonPressed = false;
			float m_TimeToShowSaveButtonMessage = 3.f;

            Core::Timer m_DeleteButtonMessageTimer;
            bool m_DeleteButtonPressed = false;
            int m_DeletedObjectsCount = 0;
            float m_TimeToShowDeleteMessage = 3;

			std::optional<World::Level> m_Level = std::nullopt;
			flecs::world* m_World = nullptr;
		};
	}
}