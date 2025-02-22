#include <Neon/System/NeonEventSystem.h>

#include <Neon/NeonDebugEntity.h>
#include <Neon/NeonEntity.h>
#include <Neon/NeonScene.h>
#include <Neon/NeonVertexBufferObject.hpp>
#include <Neon/NeonWindow.h>

#include <Neon/Component/NeonCamera.h>
#include <Neon/Component/NeonLight.h>
#include <Neon/Component/NeonMesh.h>
#include <Neon/Component/NeonShader.h>
#include <Neon/Component/NeonTexture.h>
#include <Neon/Component/NeonTransform.h>

namespace Neon
{
	set<EventSystem*> EventSystem::instances;

	EventSystem::EventSystem(Scene* scene)
		: SystemBase(scene)
	{
		if (0 == instances.size())
		{
			//struct {
			//	GLFWwindowposfun          pos;
			//	GLFWwindowsizefun         size;
			//	GLFWwindowclosefun        close;
			//	GLFWwindowrefreshfun      refresh;
			//	GLFWwindowfocusfun        focus;
			//	GLFWwindowiconifyfun      iconify;
			//	GLFWwindowmaximizefun     maximize;
			//	GLFWframebuffersizefun    fbsize;
			//	GLFWwindowcontentscalefun scale;
			//	GLFWmousebuttonfun        mouseButton;
			//	GLFWcursorposfun          cursorPos;
			//	GLFWcursorenterfun        cursorEnter;
			//	GLFWscrollfun             scroll;
			//	GLFWkeyfun                key;
			//	GLFWcharfun               character;
			//	GLFWcharmodsfun           charmods;
			//	GLFWdropfun               drop;
			//} callbacks;

			glfwSetKeyCallback(scene->GetWindow()->GetGLFWWindow(), EventSystem::KeyCallback);
			glfwSetMouseButtonCallback(scene->GetWindow()->GetGLFWWindow(), EventSystem::MouseButtonCallback);
			glfwSetCursorPosCallback(scene->GetWindow()->GetGLFWWindow(), EventSystem::CursorPosCallback);
			glfwSetScrollCallback(scene->GetWindow()->GetGLFWWindow(), EventSystem::ScrollCallback);
		}

		instances.insert(this);
	}

	EventSystem::~EventSystem()
	{
	}

	void EventSystem::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);

		KeyEvent event{ nullptr, window, key, scancode, action, mods };

		for (auto& instance : instances)
		{
			instance->OnKeyEvent(event);
		}
	}

	void EventSystem::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
	{
		ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);

		MouseButtonEvent event{ nullptr, window, button, action, mods };

		for (auto& instance : instances)
		{
			instance->OnMouseButtonEvent(event);
		}
	}

	void EventSystem::CursorPosCallback(GLFWwindow* window, double xpos, double ypos)
	{
		ImGui_ImplGlfw_CursorPosCallback(window, xpos, ypos);

		CursorPosEvent event{ nullptr, window, xpos, ypos };

		for (auto& instance : instances)
		{
			instance->OnCursorPosEvent(event);
		}
	}

	void EventSystem::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
	{
		ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);

		ScrollEvent event{ nullptr, window, xoffset, yoffset };

		for (auto& instance : instances)
		{
			instance->OnScrollEvent(event);
		}
	}

	void EventSystem::OnKeyEvent(const KeyEvent& event)
	{
		for (auto& kvp : scene->GetEntities())
		{
			KeyEvent e { kvp.second, event.window, event.key, event.scancode, event.action, event.mods };
			kvp.second->OnKeyEvent(e);
		}

		for (auto& entity : scene->GetDebugEntities())
		{
			KeyEvent e{ entity, event.window, event.key, event.scancode, event.action, event.mods };
			entity->OnKeyEvent(e);
		}
	}

	void EventSystem::OnMouseButtonEvent(const MouseButtonEvent& event)
	{
		auto now = glfwGetTime();

		bool doubleClicked = false;
		if (event.action == GLFW_RELEASE)
		{
			if (event.button == GLFW_MOUSE_BUTTON_1)
			{
				auto delta = now - lastLButtonReleaseTime;
				if (delta < doubleClickInterval)
				{
					doubleClicked = true;
				}

				lastLButtonReleaseTime = now;
			}
			else if (event.button == GLFW_MOUSE_BUTTON_2)
			{
				auto delta = now - lastRButtonReleaseTime;
				if (delta < doubleClickInterval)
				{
					doubleClicked = true;
				}

				lastRButtonReleaseTime = now;
			}
			else if (event.button == GLFW_MOUSE_BUTTON_3)
			{
				auto delta = now - lastMButtonReleaseTime;
				if (delta < doubleClickInterval)
				{
					doubleClicked = true;
				}

				lastMButtonReleaseTime = now;
			}
		}

		for (auto& kvp : scene->GetEntities())
		{
			MouseButtonEvent e{
				kvp.second,
				event.window,
				event.button,
				doubleClicked ? GLFW_DOUBLE_ACTION : event.action,
				event.mods,
				lastMouseX,
				lastMouseY };

			kvp.second->OnMouseButtonEvent(e);
		}

		for (auto& entity : scene->GetDebugEntities())
		{
			MouseButtonEvent e{
				entity,
				event.window,
				event.button,
				doubleClicked ? GLFW_DOUBLE_ACTION : event.action,
				event.mods,
				lastMouseX,
				lastMouseY };

			entity->OnMouseButtonEvent(e);
		}
	}

	void EventSystem::OnCursorPosEvent(const CursorPosEvent& event)
	{
		lastMouseX = event.xpos;
		lastMouseY = event.ypos;

		for (auto& kvp : scene->GetEntities())
		{
			CursorPosEvent e{ kvp.second, event.window, event.xpos, event.ypos };
			kvp.second->OnCursorPosEvent(e);
		}

		for (auto& entity : scene->GetDebugEntities())
		{
			CursorPosEvent e{ entity, event.window, event.xpos, event.ypos };
			entity->OnCursorPosEvent(e);
		}
	}

	void EventSystem::OnScrollEvent(const ScrollEvent& event)
	{
		for (auto& kvp : scene->GetEntities())
		{
			ScrollEvent e { kvp.second, event.window, event.xoffset, event.yoffset };
			kvp.second->OnScrollEvent(e);
		}

		for (auto& entity : scene->GetDebugEntities())
		{
			ScrollEvent e{ entity, event.window, event.xoffset, event.yoffset };
			entity->OnScrollEvent(e);
		}
	}
}
