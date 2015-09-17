#pragma once
#include "Location.h"
#include <memory>

class Shot : public Location, Timer
{
public:
	Shot(void);

	SceneMesh *m_ShotMesh;

	void Setup(std::shared_ptr<CollisionScene> scene);
	void Update(Number *elapsed);
	void Fire(Vector3 position, Vector3 velocity, float timer);
	void Pause(bool paused);
	void Deactivate();

private:
	std::shared_ptr<CollisionScene> p_Scene;
	float m_TimerAmount;
};