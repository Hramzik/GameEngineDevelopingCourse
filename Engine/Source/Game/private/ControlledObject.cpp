#include "ControlledObject.h"

namespace GameEngine
{
    void ControlledObject::Update(float dt, size_t frame)
    {
        if (m_OffsetUpdatedOnFrame != frame)
        {
            m_OffsetUpdatedOnFrame = frame;
            s_CurOffset = s_NextOffset;
            s_NextOffset = Math::Vector3f::Zero();
        }

        Math::Vector3f pos = GetPosition();
        Math::Vector3f step = s_CurOffset.Normalized() * dt * 1e2;
        SetPosition(pos + step, frame);
    }

    void ControlledObject::MoveForward()
    {
        s_NextOffset.z += s_Velocity;
    }

    void ControlledObject::MoveBackward()
    {
        s_NextOffset.z -= s_Velocity;
    }

    void ControlledObject::MoveLeft()
    {
        s_NextOffset.x -= s_Velocity;
    }

    void ControlledObject::MoveRight()
    {
        s_NextOffset.x += s_Velocity;
    }
}