#include "PlayerChase.h"

PlayerChase::PlayerChase()
{
}

// Return the rotation velocity needed to chase target.
float PlayerChase::AimAtTarget(Vector3 origin, Vector3 target, float rotation)
{
	// Figure out what direction to turn. Get the angles in degrees.
	float targetAngle = atan2(target.y - origin.y, target.x - origin.x) * TODEGREES;
	float facingAngle = rotation;
	float targetLessFacing = targetAngle - facingAngle;
	float facingLessTarget = facingAngle - targetAngle;
	float turnVelocity;

	// Figure out if it is right or left to turn.
	if (abs(targetLessFacing) > 180)
	{
		if (facingAngle > targetAngle)
		{
			facingLessTarget = ((360 - facingAngle) + targetAngle) * -1;
		}
		else
		{
			facingLessTarget = (360 - targetAngle) + facingAngle;
		}
	}

	if (facingLessTarget > 0)
	{
		turnVelocity = -50;
	}
	else
	{
		turnVelocity = 50;
	}

	return turnVelocity;
}
