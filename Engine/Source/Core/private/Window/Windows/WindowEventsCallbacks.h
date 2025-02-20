#pragma once

#include <Camera.h>
#include <Constants.h>
#include <Window/IWindow.h>
#include <Window.h>

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
        switch (key)
        {
            case 'W': g_MainCameraMovementController->StartForwardMove();  break;
            case 'S': g_MainCameraMovementController->StartBackwardMove(); break;
            case 'A': g_MainCameraMovementController->StartLeftMove();     break;
            case 'D': g_MainCameraMovementController->StartRightMove();    break;
            case VK_SPACE:  g_MainCameraMovementController->StartUpMove();   break;
            case VK_SHIFT: g_MainCameraMovementController->StartDownMove(); break;
        }
    }

    void OnKeyUp(WPARAM key)
    {
        switch (key)
        {
            case 'W': g_MainCameraMovementController->EndForwardMove();  break;
            case 'S': g_MainCameraMovementController->EndBackwardMove(); break;
            case 'A': g_MainCameraMovementController->EndLeftMove();     break;
            case 'D': g_MainCameraMovementController->EndRightMove();    break;
            case VK_SPACE:  g_MainCameraMovementController->EndUpMove();   break;
            case VK_SHIFT: g_MainCameraMovementController->EndDownMove(); break;
        }
    }
}