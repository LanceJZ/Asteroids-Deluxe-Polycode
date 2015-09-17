#pragma once
#include "Location.h"
#include "Random.h"

class Dot :	public Location
{
public:
	Dot(void);

	void Setup(std::shared_ptr<CollisionScene> scene);
	void Update(Number *elapsed);
	void Activate(Vector3 position, float size);
	void Deactivate(void);

private:
	float m_TimerAmount;
	float m_Alpha;
	
	std::unique_ptr<Timer> m_TimerLife;
	std::shared_ptr<CollisionScene> m_Scene;
	SceneMesh *m_DotMesh;
};

