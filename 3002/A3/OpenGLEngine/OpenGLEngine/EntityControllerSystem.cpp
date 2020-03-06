#include "EntityControllerSystem.h"
#include "MouseMoveEvent.h"
#include "TransformComponent.h"

namespace Reality
{
	EntityControllerSystem::EntityControllerSystem()
	{
		requireComponent<EntityController>();
	}


	void EntityControllerSystem::Update(float deltaTime)
	{
		GLFWwindow* window = getWorld().data.renderUtil->window->glfwWindow;



		Camera& camera = getWorld().data.renderUtil->camera;

		// Move
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);

		for (auto e : getEntities())
		{
			auto &eControl = getWorld().getEntity("boat").getComponent<EntityController>();
			auto &eTransform = getWorld().getEntity("boat").getComponent<TransformComponent>();

			if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			{
				eTransform.position -= eTransform.Up();
				// camera.ProcessKeyboard(FORWARD, fpsControl.forwardSpeed * deltaTime);

			}

			if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			{
				eTransform.position += eTransform.Up();
				// camera.ProcessKeyboard(BACKWARD, fpsControl.forwardSpeed  * deltaTime);
			}

			if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			{
				eTransform.position -= eTransform.Right();
				// camera.ProcessKeyboard(LEFT, fpsControl.sidewaysSpeed * deltaTime);
			}

			if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			{
				eTransform.position += eTransform.Right();
				// camera.ProcessKeyboard(RIGHT, fpsControl.sidewaysSpeed * deltaTime);
			}

			// Look
			auto mouseMoveEvents = getWorld().getEventManager().getEvents<MouseMoveEvent>();
			for (auto event : mouseMoveEvents)
			{

					eTransform.eulerAngles.x += event.deltaY*0.5;

	
					eTransform.eulerAngles.y += event.deltaX*0.5;

	

				camera.ProcessMouseMovement(event.deltaX, event.deltaY);
			}
		}
	}
}
