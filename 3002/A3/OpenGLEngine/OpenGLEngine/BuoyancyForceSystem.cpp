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
		srand(time(NULL));
		
		for (auto e : getEntities())
		{
			auto& forceAcc = e.getComponent<ForceAccumulatorComponent>();
			auto& gravity = e.getComponent<BuoyancyForceComponent>();

			if (forceAcc.inverseMass > 0)
			{
				
				int temp = rand() % 100 + 1;

				if (temp % 2 == 1)
				{
					forceAcc.AddForce((worldGravity * Vector3(0, 2, 0)) * gravity.gravityScale / forceAcc.inverseMass);
				}
				else
				{
					forceAcc.AddForce((worldGravity* Vector3(0, -2, 0)) * gravity.gravityScale  / forceAcc.inverseMass);
				}
				
				
			}
		}
	}
}
