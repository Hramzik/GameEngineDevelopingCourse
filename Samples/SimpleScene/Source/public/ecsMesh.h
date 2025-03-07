#pragma once

#include <flecs.h>
#include <Geometry.h>

namespace GameEngine::Render
{
	class RenderObject;
}

struct GeometryPtr
{
	GameEngine::RenderCore::Geometry::Ptr ptr;
};

struct RenderObjectPtr
{
	GameEngine::Render::RenderObject* ptr = nullptr;
    bool isActive = true;
};

void RegisterEcsMeshSystems(flecs::world& world);
