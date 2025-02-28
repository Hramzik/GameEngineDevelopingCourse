#include "JumpingObject.h"
#include "PhysicsManager.h"

namespace GameEngine
{
    void JumpingObject::Update(float dt)
    {
        if (!m_IsJumping) Jump();

        m_Velocity.y += PhysicsManager::GravityAcceleration * dt;

        Math::Vector3f position = GetPosition();
        position.y += m_Velocity.y * dt;
        SetPosition(position, 0);

        if (position.y > 0.0f) return;

        // end jump
        position.y = 0.0f;
        m_IsJumping = false;
        m_Velocity.y = 0.0f;
        SetPosition(position, 0);
    }

    void JumpingObject::Jump()
    {
        m_IsJumping = true;
        m_Velocity.y = DEFAULT_JUMP_SPEED;
    }
}