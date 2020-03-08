#include "BuoyancySystem.h"

namespace Reality
{
	BuoyancySystem::BuoyancySystem()
	{
		requireComponent<TransformComponentV2>();
		requireComponent<RigidBodyComponent>();
		requireComponent<Buoyancy>();
	}

	void BuoyancySystem::Update(float deltaTime)
	{
		for (auto e : getEntities())
		{
			auto &rigidbody = e.getComponent<RigidBodyComponent>();
			auto &transform = e.getComponent<TransformComponentV2>();
			auto &BuoyancyComp = e.getComponent<Buoyancy>();

			rigidbody.AddForce(Vector3(0, 1, 0));
		}
	}
}