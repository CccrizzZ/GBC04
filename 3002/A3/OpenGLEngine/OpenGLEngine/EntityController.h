
#pragma once
namespace Reality
{
	struct EntityController
	{
		EntityController()
		{}
		float forwardSpeed = 1;
		float sidewaysSpeed = 1;
		float cameraXSpeed = 1;
		float cameraYSpeed = 1;
		bool inverted = false;
	};
}
