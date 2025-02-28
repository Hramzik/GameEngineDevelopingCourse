#include "MovingObject.h"
#include <cmath>

namespace GameEngine
{
    void MovingObject::Update(float dt, size_t frame)
    {
        if (isnan(m_OriginX)) { m_OriginX = GetPosition().x; }
        m_SinusArg += dt;

        Math::Vector3f position = GetPosition();
        position.x = m_OriginX + DEFAULT_AMPLITUDE * std::sin(DEFAULT_FREQUENCY * m_SinusArg);
        SetPosition(position, frame);
    }
}