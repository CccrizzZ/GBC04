#pragma once
#include "ECSConfig.h"
#include "ParticleContactEvent.h"

namespace Reality
{
	class ParticleContactResolutionSystem : public ECSSystem
	{
	public:
		ParticleContactResolutionSystem();
		void Update(float deltaTime);
		int positionIterations = 2;
		int velocityIterations = 2;
	private:
		float CalculateSeparationVelocity(ParticleContactEvent& contact);
		void ResolveVelocity(ParticleContactEvent& contact, float deltaTime);
		void ResolveInterPenetration(ParticleContactEvent& contact);
	};
}
