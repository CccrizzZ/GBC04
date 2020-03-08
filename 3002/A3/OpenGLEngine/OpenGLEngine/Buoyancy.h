#pragma once
#include "RigidbodyGravityForceGeneratorSystem.h"
#include "ECSConfig.h"

namespace Reality {

	class Buoyancy : public ECSSystem
	{
	public:
		Buoyancy() {};
		Buoyancy(const Vector3& center, float maxdepth = 100, float vol = 1000, float waterheight = 100, float liquiddensity = 1000.0f):
			maxDepth(maxdepth), volume(vol), waterHeight(waterheight), liquidDensity(liquiddensity), centerOfBuoyancy(center)
		{

		}


		// when sumbmerged to maxDepth buoyancy force is at maximum
		float maxDepth;
		
		// volum of the object
		float volume;

		// hight of the water surface
		float waterHeight;
		
		// density
		float liquidDensity;

		// center of the force
		Vector3 centerOfBuoyancy;
	};
}

