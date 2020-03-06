#pragma once
#include "ECSConfig.h"
#include "ForceAccumulatorComponent.h"
#include "BuoyancyComponent.h"

namespace Reality
{
	class BuoyancyForceSystem : public ECSSystem
	{
	public:
		BuoyancyForceSystem();
		void Update(float deltaTime);
		Vector3 worldGravity = Vector3(0.0f, 1.0f, 0.0f);
	};
}
