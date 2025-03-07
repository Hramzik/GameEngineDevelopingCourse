#include <Camera.h>
#include <DefaultGeometry.h>
#include <ecsControl.h>
#include <ecsMesh.h>
#include <ecsPhys.h>
#include <ecsShoot.h>
#include <GameFramework/GameFramework.h>
#include <Input/Controller.h>
#include <RenderObject.h>
#include <flecs.h>
#include <Vector.h>

using namespace GameEngine;

void RegisterEcsShootSystems(flecs::world& world)
{
    world.system<AmmoShooter>()
    .each([&](AmmoShooter& shooter)
    {
        if (shooter.reloadTime <= 0) return;
        shooter.reloadTime -= world.delta_time();
    });

    world.system<Position, const BouncePlane, GroundHitLifetime>()
        .each([&](flecs::entity e, Position& pos, const BouncePlane& plane, GroundHitLifetime& lifetime)
    {
        if (lifetime.isTicking) { lifetime.timeRemaining -= world.delta_time(); }
        if (lifetime.timeRemaining <= 0.0f) { e.destruct(); return; }

        float dotPos = plane.value.x * pos.value.x + plane.value.y * pos.value.y + plane.value.z * pos.value.z;
        if (dotPos < plane.value.w) lifetime.isTicking = true;
    });

    world.system<Position, DestroyableByAmmo>()
        .each([&](flecs::entity e, Position& pos, DestroyableByAmmo& destroyable)
    {
        world.each<Position>([&](flecs::entity bulletEntity, Position& ballPos)
        {
            if (!bulletEntity.has<BulletTag>()) return;
            float distance = (pos.value - ballPos.value).GetLength();
            if (distance > 2) return;

            e.destruct();
        });
    });

    world.system<Position, GivingAmmoByCollision>()
        .each([&](flecs::entity e, Position& pos, GivingAmmoByCollision& pickup)
    {
        world.each<Position>([&](flecs::entity bulletEntity, Position& ballPos)
        {
            if (!bulletEntity.has<BulletTag>()) return;
            float distance = (pos.value - ballPos.value).GetLength();
            if (distance > 2) return;

            world.each<AmmoShooter>([&](AmmoShooter& shooter)
            {
                shooter.ammoCount += pickup.ammoToAdd;
            });
            e.destruct();
        });
    });
}

