#include "ParticleContactResolutionSystem.h"
#include "ParticleComponent.h"
#include "ForceAccumulatorSystem.h"
#include "TransformComponent.h"

namespace Reality
{
	ParticleContactResolutionSystem::ParticleContactResolutionSystem()
	{

	}

	void ParticleContactResolutionSystem::Update(float deltaTime)
	{
		auto contactEvents = getWorld().getEventManager().getEvents<ParticleContactEvent>();

		if (contactEvents.size() > 0)
		{
			for (int i = 0; i < velocityIterations; i++)
			{
				std::sort(contactEvents.begin(), contactEvents.end(),
					[this](auto a, auto b){
					return CalculateSeparationVelocity(a) < CalculateSeparationVelocity(b);
					});
				ResolveVelocity(contactEvents[0], deltaTime);
			}

			for (int i = 0; i < positionIterations; i++)
			{
				std::sort(contactEvents.begin(), contactEvents.end(),
					[](auto a, auto b){
						return a.penetration < b.penetration;	
					});
				ResolveInterPenetration(contactEvents[0]);
			}
		}


		// for (auto& contact : contactEvents)
		// {
		// 	ResolveVelocity(contact, deltaTime);
		// 	ResolveInterPenetration(contact);
		// }
	}

	float ParticleContactResolutionSystem::CalculateSeparationVelocity(ParticleContactEvent & contact)
	{
		Vector3 velocityA = contact.entityA.hasComponent<ParticleComponent>() ?
			contact.entityA.getComponent<ParticleComponent>().velocity : Vector3(0, 0, 0);

		Vector3 velocityB = contact.entityB.hasComponent<ParticleComponent>() ?
			contact.entityB.getComponent<ParticleComponent>().velocity : Vector3(0, 0, 0);

		Vector3 separationVelocity = velocityA - velocityB;

		return glm::dot(separationVelocity, contact.normal);
	}

	void ParticleContactResolutionSystem::ResolveVelocity(ParticleContactEvent & contact, float deltaTime)
	{
		float initialVelocity = CalculateSeparationVelocity(contact);

		if (initialVelocity > 0)
		{
			return;
		}

		float finalVelocity = -initialVelocity * contact.restitution;

		float deltaVelocity = finalVelocity - initialVelocity;

		float invMA = contact.entityA.hasComponent<ForceAccumulatorComponent>() ?
			contact.entityA.getComponent<ForceAccumulatorComponent>().inverseMass : 0;

		float invMB = contact.entityB.hasComponent<ForceAccumulatorComponent>() ?
			contact.entityB.getComponent<ForceAccumulatorComponent>().inverseMass : 0;

		float totalInverseMass = invMA + invMB;

		if (totalInverseMass <= 0)
		{
			return;
		}

		float impulse = deltaVelocity / totalInverseMass;
		Vector3 impulsePerIMass = impulse * contact.normal;

		if (contact.entityA.hasComponent<ParticleComponent>())
		{
			contact.entityA.getComponent<ParticleComponent>().velocity += impulsePerIMass * invMA;
		}

		if (contact.entityB.hasComponent<ParticleComponent>())
		{
			contact.entityB.getComponent<ParticleComponent>().velocity -= impulsePerIMass * invMB;
		}
	}
	void ParticleContactResolutionSystem::ResolveInterPenetration(ParticleContactEvent & contact)
	{
		if (contact.penetration < 0)
		{
			return;
		}

		float invMassA = contact.entityA.hasComponent<ForceAccumulatorComponent>() ?
			contact.entityA.getComponent<ForceAccumulatorComponent>().inverseMass : 0;

		float invMassB = contact.entityB.hasComponent<ForceAccumulatorComponent>() ?
			contact.entityB.getComponent<ForceAccumulatorComponent>().inverseMass : 0;

		float totalInverseMass = invMassA + invMassB;

		if (totalInverseMass <= 0)
		{
			return;
		}

		Vector3 movePerUnitIMass = contact.normal * (contact.penetration / totalInverseMass);

		if (contact.entityA.hasComponent<TransformComponent>())
		{
			contact.entityA.getComponent<TransformComponent>().position += movePerUnitIMass * invMassA;
		}

		if (contact.entityB.hasComponent<TransformComponent>())
		{
			contact.entityB.getComponent<TransformComponent>().position -= movePerUnitIMass * invMassB;
		}

	}
}
