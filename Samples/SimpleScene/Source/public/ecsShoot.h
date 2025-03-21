#pragma once

#include <flecs.h>

struct BulletShooter {

    bool isAbleToShoot = true;
    static const float inline DEFAULT_SPEED = 10;
};

// This define is essential for the scripts to expose the ECS components to lua syntax
// There is a task to rework this behavior
#undef ECS_META_IMPL
#ifndef GAME_FRAMEWORK
#define ECS_META_IMPL EXTERN // Ensure meta symbols are only defined once
#endif

ECS_STRUCT(Bullet,
{
    float damage;
});

ECS_STRUCT(Health,
{
    float value;
});

ECS_STRUCT(HitboxSize,
{
    float x;
    float y;
    float z;
});

ECS_STRUCT(DestroyTimer,
{
    float time;
});

void RegisterEcsShootSystems(flecs::world& world);

