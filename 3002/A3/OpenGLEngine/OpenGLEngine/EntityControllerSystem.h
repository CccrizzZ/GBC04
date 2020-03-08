#pragma once
#include "ECSConfig.h"
#include "EntityController.h"

namespace Reality
{
	class EntityControllerSystem : public ECSSystem
	{
	public:
		EntityControllerSystem();
		void Update(float deltaTime);
		bool flag;
	};
}

