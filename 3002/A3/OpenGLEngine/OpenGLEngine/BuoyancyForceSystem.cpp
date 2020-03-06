#include "BuoyancyForceSystem.h"
namespace Reality
{
	BuoyancyForceSystem::BuoyancyForceSystem()
	{
		requireComponent<ForceAccumulatorComponent>();
		requireComponent<BuoyancyForceComponent>();
	}

	void BuoyancyForceSystem::Update(float deltaTime)
	{
		for (auto e : getEntities())
		{
			auto& forceAcc = e.getComponent<ForceAccumulatorComponent>();
			auto& gravity = e.getComponent<BuoyancyForceComponent>();

			if (forceAcc.inverseMass > 0)
			{
				forceAcc.AddForce(worldGravity * gravity.gravityScale / forceAcc.inverseMass);
			}
		}
	}
}
