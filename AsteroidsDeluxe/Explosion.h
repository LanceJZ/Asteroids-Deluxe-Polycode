#pragma once
#include "Location.h"
#include "Dot.h"

class Explosion : public Location
{
public:
	Explosion(void);
	void Setup(std::shared_ptr<CollisionScene> scene);
	void Update(Number *elapsed);
	void Activate(Vector3 position, float size);
	void Deactivate(void);

private:
	std::shared_ptr<Dot> pDots[12];

};

