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
				
				eTransform.position -= eTransform.Up() * (deltaTime * 100);

				if (eTransform.eulerAngles.x > 260)
				{
					eTransform.eulerAngles.x -= 0.5;
					

				}
				flag = false;
			} 

			if (glfwGetKey(window, GLFW_KEY_W) == GLFW_RELEASE)
			{
				if (eTransform.eulerAngles.x < 269)
				{
					eTransform.eulerAngles.x += 0.5;
					flag = false;
				}
				else
				{
					flag = true;
				}
				
				
			}

			if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			{
				eTransform.position += eTransform.Up() * (deltaTime * 100);
				if (eTransform.eulerAngles.x < 280)
				{
					eTransform.eulerAngles.x += 0.5;

				}
				flag = false;

			}

			if (glfwGetKey(window, GLFW_KEY_S) == GLFW_RELEASE)
			{
				if (eTransform.eulerAngles.x > 269)
				{
					eTransform.eulerAngles.x -= 0.5;
					flag = false;
				}
				else
				{
					flag = true;
				}

			}

			if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			{
				eTransform.eulerAngles.y += 0.5;
			}

			if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			{
				eTransform.eulerAngles.y -= 0.5;

			}

			// Look
			auto mouseMoveEvents = getWorld().getEventManager().getEvents<MouseMoveEvent>();
			for (auto event : mouseMoveEvents)
			{
				// move boat eularangle with mouse
				eTransform.eulerAngles.y -= event.deltaX * 0.5;

				// // look down
				// if (eTransform.eulerAngles.x > 0)
				// {
				// 	eTransform.eulerAngles.x = 0;
				// 	break;
				// }

				// // look up
				// if (eTransform.eulerAngles.x < -180)
				// {
				// 	eTransform.eulerAngles.x = -180;
				// 	break;
				// }

				// eTransform.eulerAngles.x -= event.deltaY*0.5;				
			}








			// Attach camera to boat/plane
			// if "W" is pressed dont snap camera Y to boat
			if (flag == true)
			{
				camera.Front = -(eTransform.Up());
			}
			else
			{
				camera.Front.x = -(eTransform.Up().x);
				camera.Front.z = -(eTransform.Up().z);

			}
			
			
			// Vector3 deltaD = eTransform.position - camera.Position;
			// float tempPitch = 1 / tan(deltaD.y / deltaD.z);

			// camera.Pitch += tempPitch;

			camera.Right = -(eTransform.Right());
			camera.Up = (eTransform.Forward());
			camera.Position.x = eTransform.position.x;
			camera.Position.y = eTransform.position.y + 40;
			camera.Position.z = eTransform.position.z + 200;
		}
	}
}
