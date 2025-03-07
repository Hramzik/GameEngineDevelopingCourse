#include <Camera.h>
#include <DefaultGeometry.h>
#include <ecsControl.h>
#include <ECS/ecsSystems.h>
#include <ecsMesh.h>
#include <ecsPhys.h>
#include <ecsShoot.h>
#include <flecs.h>
#include <Input/Controller.h>
#include <Input/InputHandler.h>
#include <RenderObject.h>
#include <Vector.h>

using namespace GameEngine;

void RegisterEcsControlSystems(flecs::world& world)
{
	world.system<Position, CameraPtr, const Speed, const ControllerPtr>()
		.each([&](flecs::entity e, Position& position, CameraPtr& camera, const Speed& speed, const ControllerPtr& controller)
	{
		Math::Vector3f currentMoveDir = Math::Vector3f::Zero();
		if (controller.ptr->IsPressed("GoLeft"))
		{
			currentMoveDir = currentMoveDir - camera.ptr->GetRightDir();
		}
		if (controller.ptr->IsPressed("GoRight"))
		{
			currentMoveDir = currentMoveDir + camera.ptr->GetRightDir();
		}
		if (controller.ptr->IsPressed("GoBack"))
		{
			currentMoveDir = currentMoveDir - camera.ptr->GetViewDir();
		}
		if (controller.ptr->IsPressed("GoForward"))
		{
			currentMoveDir = currentMoveDir + camera.ptr->GetViewDir();
		}
		position.value = position.value + currentMoveDir.Normalized() * speed * world.delta_time();
		camera.ptr->SetPosition(position.value);
	});

	world.system<const Position, Velocity, const ControllerPtr, const BouncePlane, const JumpSpeed>()
		.each([&](const Position& pos, Velocity& vel, const ControllerPtr& controller, const BouncePlane& plane, const JumpSpeed& jump)
	{
		constexpr float planeEpsilon = 0.1f;
		if (plane.value.x * pos.value.x + plane.value.y * pos.value.y + plane.value.z * pos.value.z < plane.value.w + planeEpsilon)
		{
			if (controller.ptr->IsPressed("Jump"))
			{
				vel.value.y = jump.value;
			}
		}
	});

    world.system<const ControllerPtr, const Position, const CameraPtr, AmmoShooter>()
        .each([&](const ControllerPtr& controller, const Position& pos, const CameraPtr& camera, AmmoShooter& shooter)
    {
        if (!controller.ptr->IsPressed("Shoot")) { shooter.isAbleToShoot = true; return; }
        if (!shooter.isAbleToShoot) return;
        shooter.isAbleToShoot = false;

        if (shooter.reloadTime > 0) return;
        if (shooter.ammoCount <= 0)
        {
            shooter.reloadTime = AmmoShooter::DEFAULT_RELOAD_TIME;
            shooter.ammoCount = AmmoShooter::DEFAULT_RELOAD_COUNT;
            return;
        }

        Math::Vector3f shootDirection = camera.ptr->GetViewDir();
        flecs::entity ball = world.entity()
            .set(BulletTag{})
            .set(Position{ pos.value })
            .set(Velocity{ shootDirection * AmmoShooter::DEFAULT_SPEED })
            .set(Gravity{ Math::Vector3f(0.f, -9.8065f, 0.f) })
            .set(BouncePlane{ Math::Vector4f(0.f, 1.f, 0.f, 5.f) })
            .set(Bounciness{ -1 })
            .set(GeometryPtr{ RenderCore::DefaultGeometry::Cube() })
            .set(RenderObjectPtr{ new Render::RenderObject() })
            .set(GroundHitLifetime{});

        shooter.ammoCount--;
    });
}

