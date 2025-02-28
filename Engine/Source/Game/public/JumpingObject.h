#pragma once

#include "GameObject.h"

namespace GameEngine
{
    class JumpingObject : public GameObject
    {
    public:
        void Update(float dt) override;
        void Jump();

    private:
        bool m_IsJumping = false;
        Math::Vector3f m_Velocity = {};
    private:
        static const inline float DEFAULT_JUMP_SPEED = 5;
    };
}