#pragma once

#include "GameObject.h"

namespace GameEngine
{
    class JumpingObject : public GameObject
    {
    public:
        void Update(float dt, size_t frame) override;
        
        private:
        void Move(float dt, size_t frame);
        void Jump();
        void EndJump(size_t frame);
    private:
        bool m_IsJumping = false;
        float m_GroundY = NAN;
        Math::Vector3f m_Velocity = Math::Vector3f::Zero();

    private:
        static const inline float DEFAULT_JUMP_SPEED = 5;
    };
}