#include <Neon/Component/NeonCameraManipulator.h>
#include <Neon/Component/NeonCamera.h>
#include <Neon/Component/NeonTransform.h>
#include <Neon/NeonEntity.h>

namespace Neon
{
	CameraManipulator::CameraManipulator(const string& name, Entity* cameraEntity, Camera* camera)
		: ComponentBase(name), cameraEntity(cameraEntity), camera(camera), transform(cameraEntity->GetComponent<Transform>(0))
	{
	}

	CameraManipulator::~CameraManipulator()
	{
	}

	void CameraManipulator::OnKeyEvent(const KeyEvent& event)
	{
		if (event.key == GLFW_KEY_LEFT_SHIFT || event.key == GLFW_KEY_RIGHT_SHIFT)
		{
			if (event.action == GLFW_PRESS)
			{
				isShiftDown = true;
			}
			else
			{
				isShiftDown = false;
			}
		}

		NeonObject::OnKeyEvent(event);
	}

	void CameraManipulator::OnMouseButtonEvent(const MouseButtonEvent& event)
	{
		if (event.button == GLFW_MOUSE_BUTTON_1) // Left Button
		{
			if (event.action == GLFW_PRESS)
			{
				isLButtonPressed = true;
				lbuttonPressX = lastCursorPosX;
				lbuttonPressY = lastCursorPosY;
			}
			else if (event.action == GLFW_RELEASE)
			{
				isLButtonPressed = false;
			}
		}
		else if (event.button == GLFW_MOUSE_BUTTON_2) // Right Button
		{
			if (event.action == GLFW_PRESS)
			{
				isRButtonPressed = true;

				rbuttonPressX = lastCursorPosX;
				rbuttonPressY = lastCursorPosY;
			}
			else if (event.action == GLFW_RELEASE)
			{
				isRButtonPressed = false;
			}
		}
		else if (event.button == GLFW_MOUSE_BUTTON_3) // Middle Button
		{
			if (event.action == GLFW_PRESS)
			{
				isMButtonPressed = true;

				mbuttonPressX = lastCursorPosX;
				mbuttonPressY = lastCursorPosY;
			}
			else if (event.action == GLFW_RELEASE)
			{
				isMButtonPressed = false;
			}
		}

		NeonObject::OnMouseButtonEvent(event);
	}

	void CameraManipulator::OnCursorPosEvent(const CursorPosEvent& event)
	{
		auto dx = event.xpos - lastCursorPosX;
		auto dy = event.ypos - lastCursorPosY;

		if (isRButtonPressed)
		{
			camera->angleH -= float(dx);
			camera->angleV += float(dy);
		}

		if (isMButtonPressed)
		{
			auto xAxis = glm::vec3(glm::row(camera->viewMatrix, 0));
			auto yAxis = glm::vec3(glm::row(camera->viewMatrix, 1));

			transform->position += glm::normalize(xAxis) * (float)-dx * 0.001f;
			camera->centerPosition += glm::normalize(xAxis) * (float)-dx * 0.001f;
			transform->position += glm::normalize(yAxis) * (float)dy * 0.001f;
			camera->centerPosition += glm::normalize(yAxis) * (float)dy * 0.001f;
		}

		lastCursorPosX = event.xpos;
		lastCursorPosY = event.ypos;

		NeonObject::OnCursorPosEvent(event);
	}

	void CameraManipulator::OnScrollEvent(const ScrollEvent& event)
	{
		camera->fovy -= float(event.xoffset) * 0.01f;
		if (0.01f > camera->fovy) camera->fovy = 0.01f;
		if (89.99f < camera->fovy) camera->fovy = 89.99f;

		if (event.yoffset > 0)
		{
			camera->distance *= 0.9f;
		}
		else
		{
			camera->distance *= 1.1f;
		}

		NeonObject::OnScrollEvent(event);
	}
}