#include <Camera.h>
#include <Input/Controller.h>
#include <ecsPhys.h>
#include <ecsControl.h>
#include <ecsShoot.h>
#include <ECS/ecsSystems.h>
#include <flecs.h>
#include <DefaultGeometry.h>
#include <RenderThread.h>
#include <RenderObject.h>

using namespace GameEngine;

void RegisterEcsShootSystems(flecs::world& world)
{
    world.system<const ControllerPtr, const Position, const CameraPtr, BulletShooter>()
        .each([&](const ControllerPtr& controller, const Position& pos, const CameraPtr& camera, BulletShooter& shooter)
    {
        if (!controller.ptr->IsPressed("Shoot")) { shooter.isAbleToShoot = true; return; }
        if (!shooter.isAbleToShoot) return;
        shooter.isAbleToShoot = false;

        Math::Vector3f shootDirection = camera.ptr->GetViewDir();
        Math::Vector3f bulletSpeed = shootDirection * BulletShooter::DEFAULT_SPEED;
        flecs::entity bullet = world.entity()
            .set(Bullet{ 50 })
            .set(HitboxSize{ 1, 1, 1 })
            .set(Position{ pos.x, pos.y, pos.z })
            .set(Velocity{ bulletSpeed.x, bulletSpeed.y, bulletSpeed.z })
            .set(Gravity{ 0, -9.8065, 0 })
            .set(EntitySystem::ECS::GeometryPtr{ RenderCore::DefaultGeometry::Cube() })
            .set(EntitySystem::ECS::RenderObjectPtr{ new Render::RenderObject() })
            .set(DestroyTimer{ 5 })
            ;
    });
}

