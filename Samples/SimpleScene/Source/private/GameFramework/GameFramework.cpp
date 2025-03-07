#include <Camera.h>
#include <DefaultGeometry.h>
#include <ecsControl.h>
#include <ecsMesh.h>
#include <ecsPhys.h>
#include <ecsShoot.h>
#include <GameFramework/GameFramework.h>
#include <Input/Controller.h>
#include <RenderObject.h>

using namespace GameEngine;

void GameFramework::Init()
{
	RegisterEcsMeshSystems(m_World);
	RegisterEcsControlSystems(m_World);
	RegisterEcsPhysSystems(m_World);
	RegisterEcsShootSystems(m_World);

	flecs::entity cubeControl = m_World.entity()
		.set(Position{ Math::Vector3f(-2.f, 0.f, 0.f) })
		.set(Velocity{ Math::Vector3f(0.f, 0.f, 0.f) })
		.set(Speed{ 10.f })
		.set(FrictionAmount{ 0.9f })
		.set(JumpSpeed{ 10.f })
		.set(Gravity{ Math::Vector3f(0.f, -9.8065f, 0.f) })
		.set(BouncePlane{ Math::Vector4f(0.f, 1.f, 0.f, 5.f) })
		.set(Bounciness{ 0.3f })
		.set(GeometryPtr{ RenderCore::DefaultGeometry::Cube() })
		.set(RenderObjectPtr{ new Render::RenderObject() })
		.set(ControllerPtr{ new Core::Controller(Core::g_FileSystem->GetConfigPath("Input_default.ini")) });

	flecs::entity cubeMoving = m_World.entity()
		.set(Position{ Math::Vector3f(2.f, 0.f, 0.f) })
		.set(Velocity{ Math::Vector3f(0.f, 3.f, 0.f) })
		.set(Gravity{ Math::Vector3f(0.f, -9.8065f, 0.f) })
		.set(BouncePlane{ Math::Vector4f(0.f, 1.f, 0.f, 5.f) })
		.set(Bounciness{ 1.f })
		.set(GeometryPtr{ RenderCore::DefaultGeometry::Cube() })
		.set(RenderObjectPtr{ new Render::RenderObject() });

        flecs::entity camera = m_World.entity()
        .set(Position{ Math::Vector3f(0.0f, 12.0f, -10.0f) })
        .set(Speed{ 10.f })
        .set(CameraPtr{ Core::g_MainCamera })
        .set(ControllerPtr{ new Core::Controller(Core::g_FileSystem->GetConfigPath("Input_default.ini")) })
        .set(AmmoShooter{});

        const int gridSize = 10;
        const float spacing = 3;
        Math::Vector3f destroyableBasePos(-2, 10, 0);
        Math::Vector3f pickUpBasePos(2, 10, 0);
        
        for (int i = 0; i < gridSize; ++i) {
        for (int j = 0; j < gridSize; ++j) {
            Math::Vector3f pos(
                destroyableBasePos.x - i * spacing,
                destroyableBasePos.y + j * spacing,
                destroyableBasePos.z
            );
    
            flecs::entity destroyableObject = m_World.entity()
                .set(Position{ pos })
                .set(DestroyableByAmmo{})
                .set(GeometryPtr{ RenderCore::DefaultGeometry::Cube() })
                .set(RenderObjectPtr{ new Render::RenderObject() });
        }}
        
        for (int i = 0; i < gridSize; ++i) {
        for (int j = 0; j < gridSize; ++j) {
                Math::Vector3f pos(
                    pickUpBasePos.x + i * spacing,
                    pickUpBasePos.y + j * spacing,
                    pickUpBasePos.z
                );
        
                flecs::entity ammoGivingObject = m_World.entity()
                    .set(Position{ pos })
                    .set(GivingAmmoByCollision{})
                    .set(GeometryPtr{ RenderCore::DefaultGeometry::Cube() })
                    .set(RenderObjectPtr{ new Render::RenderObject() });
        }}
}

void GameFramework::Update(float dt)
{

}