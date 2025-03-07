#pragma once

#include <flecs.h>

struct BulletTag
{
    int NOT_BLANK_;
};

struct GroundHitLifetime
{
    bool isTicking = false;
    float timeRemaining = DEFAULT_TIME_REMAINING;
    static const float inline DEFAULT_TIME_REMAINING = 5;
};

struct AmmoShooter
{
    int ammoCount = DEFAULT_RELOAD_COUNT;
    float reloadTime = 0;
    bool isAbleToShoot = true;
    static const float inline DEFAULT_SPEED = 100;
    static const float inline DEFAULT_RELOAD_TIME  = 2;
    static const int   inline DEFAULT_RELOAD_COUNT = 6;
};

struct DestroyableByAmmo
{
    bool isDestroyed = false;
};

struct GivingAmmoByCollision
{
    int ammoToAdd = DEFAULT_AMMO_TO_ADD;
    static const int inline DEFAULT_AMMO_TO_ADD = 1;
};

void RegisterEcsShootSystems(flecs::world& world);

