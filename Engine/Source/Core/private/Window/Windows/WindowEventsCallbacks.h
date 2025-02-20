#pragma once

#include <Camera.h>
#include <Constants.h>
#include <Window/IWindow.h>
#include <Window.h>
#include <ControlMapper.h>

namespace GameEngine::Core
{
    void OnMouseDown(WPARAM btnState, int x, int y, Window* window)
    {
        window->SetMousePos(x, y);

        SetCapture(GetPlatformWindowHandle(window->GetWindowHandle()));
    }

    void OnMouseUp(WPARAM btnState, int x, int y)
    {
        ReleaseCapture();
    }

    void OnMouseMove(WPARAM btnState, int x, int y, Camera* camera, Window* window)
    {
        Math::Vector2i mousePos = window->GetMousePos();
        if ((btnState & MK_LBUTTON) != 0)
        {
            float dx = 0.25 * static_cast<float>(x - mousePos.x) * Math::Constants::PI / 180.f;
            float dy = 0.25 * static_cast<float>(y - mousePos.y) * Math::Constants::PI / 180.f;

            dy = -dy; // To avoid inverse movement

            camera->Rotate(dx, dy);
        }
        else if ((btnState & MK_RBUTTON) != 0)
        {
            float dx = 0.05f * static_cast<float>(x - mousePos.x);
            float dy = 0.05f * static_cast<float>(y - mousePos.y);

            Math::Vector3f offset = camera->GetViewDir() * (dx - dy);

            Math::Vector3f position = camera->GetPosition();
            position = position + offset;

            camera->SetPosition(position);
        }

        window->SetMousePos(x, y);
    }

    void OnKeyDown(WPARAM key)
    {
        if (key == ControlMapper::Instance()->GetForwardVKCode())  g_MainCameraMovementController->StartForwardMove();
        if (key == ControlMapper::Instance()->GetBackwardVKCode()) g_MainCameraMovementController->StartBackwardMove();
        if (key == ControlMapper::Instance()->GetRightVKCode())    g_MainCameraMovementController->StartRightMove();
        if (key == ControlMapper::Instance()->GetLeftVKCode())     g_MainCameraMovementController->StartLeftMove();
        if (key == ControlMapper::Instance()->GetUpVKCode())       g_MainCameraMovementController->StartUpMove();
        if (key == ControlMapper::Instance()->GetDownVKCode())     g_MainCameraMovementController->StartDownMove();
    }

    void OnKeyUp(WPARAM key)
    {
        if (key == ControlMapper::Instance()->GetForwardVKCode())  g_MainCameraMovementController->EndForwardMove();
        if (key == ControlMapper::Instance()->GetBackwardVKCode()) g_MainCameraMovementController->EndBackwardMove();
        if (key == ControlMapper::Instance()->GetRightVKCode())    g_MainCameraMovementController->EndRightMove();
        if (key == ControlMapper::Instance()->GetLeftVKCode())     g_MainCameraMovementController->EndLeftMove();
        if (key == ControlMapper::Instance()->GetUpVKCode())       g_MainCameraMovementController->EndUpMove();
        if (key == ControlMapper::Instance()->GetDownVKCode())     g_MainCameraMovementController->EndDownMove();
    }
}