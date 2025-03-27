#pragma once

struct DestroyTimer
{
	float timeLeft;
};

struct Hitbox
{
    float hitboxRadius;
};

struct CollisionStatus
{
    bool isActive;
    bool isResolved;
};

struct MovementRestrictor
{
    bool restrictX;
    bool restrictY;
    bool restrictZ;

    float minX;
    float maxX;

    float minY;
    float maxY;

    float minZ;
    float maxZ;
};