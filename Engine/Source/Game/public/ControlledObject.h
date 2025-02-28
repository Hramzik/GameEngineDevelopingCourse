#pragma once

#include "GameObject.h"

namespace GameEngine
{
    class ControlledObject : public GameObject
    {
    public:
        void Update(float dt, size_t frame) override;
        
    public:
        static void MoveForward();
        static void MoveBackward();
        static void MoveLeft();
        static void MoveRight();
    
    private:
        static inline const float s_Velocity = 2 * 1e-3;
        static inline Math::Vector3f s_CurOffset = Math::Vector3f::Zero();
        static inline Math::Vector3f s_NextOffset = Math::Vector3f::Zero();
        static inline size_t m_OffsetUpdatedOnFrame = 0;
    };
}