#pragma once

#include <Core/export.h>
#include <Matrix.h>
#include <Vector.h>

namespace GameEngine
{
	namespace Core
	{
		class CORE_API Camera final
		{
		public:
			Camera() = default;

		public:
			Math::Matrix4x4f GetViewMatrix();
			Math::Vector3f GetPosition() const { return m_Position; }
			void SetPosition(Math::Vector3f position) { m_Position = position; }
			Math::Vector3f GetViewDir() const { return m_ViewDir; }
			void SetViewDir(Math::Vector3f viewDir) { m_ViewDir = viewDir; }
			void Rotate(float yaw, float pitch);

		private:
			Math::Vector3f m_Position;
			Math::Vector3f m_ViewDir;
		};

        // speed.x = forward direction speed
        // speed.y = right   direction speed
        // speed.z = up      direction speed
        class CORE_API CameraMovementController final
		{
		public:
            CameraMovementController(Camera& camera);

		public:
            void StartForwardMove();
            void EndForwardMove();
            void StartBackwardMove();
            void EndBackwardMove();
            void StartRightMove();
            void EndRightMove();
            void StartLeftMove();
            void EndLeftMove();
            void StartUpMove();
            void EndUpMove();
            void StartDownMove();
            void EndDownMove();
            Math::Vector3f GetCameraSpeed();
            void SetCameraSpeed(Math::Vector3f speed);
            void MoveCamera(float dt);

		private:
            Camera& m_Camera;
            Math::Vector3f m_CameraSpeed;
            bool m_IsForwardMoveStarted  = false;
            bool m_IsBackwardMoveStarted = false;
            bool m_IsRightMoveStarted    = false;
            bool m_IsLeftMoveStarted     = false;
            bool m_IsUpMoveStarted       = false;
            bool m_IsDownMoveStarted     = false;

        private:
            static inline float DEFAULT_CAMERA_SPEED = 10;
		};

		extern CORE_API Camera* g_MainCamera;
		extern CORE_API CameraMovementController* g_MainCameraMovementController;
	}
}