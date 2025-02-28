#pragma once

#include "GameObject.h"

namespace GameEngine
{
    class MovingObject : public GameObject
    {
    public:
        void Update(float dt, size_t frame) override;

    private:
        float m_OriginX = NAN;
        float m_SinusArg = 0;
        static inline const float DEFAULT_AMPLITUDE = 2;
        static inline const float DEFAULT_FREQUENCY = 1;
    };
}