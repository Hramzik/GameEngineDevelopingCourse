#include "JumpingObject.h"
#include "PhysicsManager.h"

namespace GameEngine
{
    void JumpingObject::Update(float dt, size_t frame)
    {
        if (isnan(m_GroundY)) { m_GroundY = GetPosition().y; }
        if (!m_IsJumping) Jump();

        Move(dt, frame);

        if (GetPosition().y < m_GroundY) EndJump(frame);
    }

    void JumpingObject::Move(float dt, size_t frame)
    {
        m_Velocity.y += PhysicsManager::GravityAcceleration * dt;

        Math::Vector3f position = GetPosition();
        position.y += m_Velocity.y * dt;
        SetPosition(position, frame);
    }

    void JumpingObject::Jump()
    {
        m_IsJumping = true;
        m_Velocity.y = DEFAULT_JUMP_SPEED;
    }

    void JumpingObject::EndJump(size_t frame)
    {
        Math::Vector3f position = GetPosition();
        position.y = m_GroundY;
        m_IsJumping = false;
        m_Velocity.y = 0.0f;
        SetPosition(position, frame);
    }
}