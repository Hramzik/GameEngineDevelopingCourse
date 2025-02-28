#include <random>

#include <Camera.h>
#include <DefaultGeometry.h>
#include <Game.h>
#include <GameObject.h>
#include <JumpingObject.h>
#include <MovingObject.h>
#include <ControlledObject.h>
#include <Input/InputHandler.h>

namespace GameEngine
{
	Game::Game(
		std::function<bool()> PlatformLoopFunc
	) :
		PlatformLoop(PlatformLoopFunc)
	{
		Core::g_MainCamera = new Core::Camera();
		Core::g_MainCamera->SetPosition(Math::Vector3f(0.0f, 6.0f, -6.0f));
		Core::g_MainCamera->SetViewDir(Math::Vector3f(0.0f, -6.0f, 6.0f).Normalized());

		m_renderThread = std::make_unique<Render::RenderThread>();

		std::random_device rd;
        std::mt19937 gen(rd());
		std::uniform_int_distribution<> dist(0, 2);
        const float width = 10;
        const float spacing = 5;

        for (int x = -(width-1) / 2 * spacing; x <= (width-1) / 2 * spacing; x += spacing) {
        for (int z = -(width-1) / 2 * spacing; z <= (width-1) / 2 * spacing; z += spacing) {

            GameObject* object = nullptr;

            switch (dist(gen))
            {
            case 0:
                object = new MovingObject(); break;
            case 1:
                object = new JumpingObject(); break;
            case 2:
                object = new ControlledObject(); break;
            }

            Math::Vector3f position(x, -100, z + 100);
            object->SetPosition(position, 0);

            m_Objects.push_back(object);
            Render::RenderObject** renderObject = object->GetRenderObjectRef();
            m_renderThread->EnqueueCommand(Render::ERC::CreateRenderObject, RenderCore::DefaultGeometry::Cube(), renderObject);
        }}

		Core::g_InputHandler->RegisterCallback("GoForward", [&]() { Core::g_MainCamera->Move(Core::g_MainCamera->GetViewDir());   });
		Core::g_InputHandler->RegisterCallback("GoBack",    [&]() { Core::g_MainCamera->Move(-Core::g_MainCamera->GetViewDir());  });
		Core::g_InputHandler->RegisterCallback("GoRight",   [&]() { Core::g_MainCamera->Move(Core::g_MainCamera->GetRightDir());  });
		Core::g_InputHandler->RegisterCallback("GoLeft",    [&]() { Core::g_MainCamera->Move(-Core::g_MainCamera->GetRightDir()); });
        Core::g_InputHandler->RegisterCallback("MoveObjectsForward",  [&]() { GameEngine::ControlledObject::MoveForward();  });
		Core::g_InputHandler->RegisterCallback("MoveObjectsBackward", [&]() { GameEngine::ControlledObject::MoveBackward(); });
		Core::g_InputHandler->RegisterCallback("MoveObjectsRight",    [&]() { GameEngine::ControlledObject::MoveRight();    });
		Core::g_InputHandler->RegisterCallback("MoveObjectsLeft",     [&]() { GameEngine::ControlledObject::MoveLeft();     });
	}

	void Game::Run()
	{
		assert(PlatformLoop != nullptr);

		m_GameTimer.Reset();

		bool quit = false;
		while (!quit)
		{
			m_GameTimer.Tick();
			float dt = m_GameTimer.GetDeltaTime();

			Core::g_MainWindowsApplication->Update();
			Core::g_InputHandler->Update();
			Core::g_MainCamera->Update(dt);

			Update(dt);

			m_renderThread->OnEndFrame();

			// The most common idea for such a loop is that it returns false when quit is required, or true otherwise
			quit = !PlatformLoop();
		}
	}

	void Game::Update(float dt)
	{
		for (int i = 0; i < m_Objects.size(); ++i)
		{
			m_Objects[i]->Update(dt, m_renderThread->GetMainFrame());
		}
	}
}