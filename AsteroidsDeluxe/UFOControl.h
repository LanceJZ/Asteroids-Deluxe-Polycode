#pragma once
#include "UFO.h"
#include "Explosion.h"

class UFOControl : public Timer
{
public:
	UFOControl(void);
	void Setup(std::shared_ptr<CollisionScene> scene, std::shared_ptr<Player> player);
	void Update(Number *elapsed);
	void WaveNumber(int Wave);
	void Pause(bool paused);
	void HitRock(void);
	void Deactivate(void);
	void DeactivateShot(void);
	void NewGame(void);

	//float ShotRadius(void);
	//float Radius(void);
	//SceneMesh *ShotMesh(void);
	//SceneMesh *ShipBody(void);
	//Vector3 Position(void);
	//bool ShotActive(void);
	//bool Active(void);
	//bool PlayerNotClear(void);

	std::unique_ptr<UFO> p_UFO;

private:
	std::shared_ptr<CollisionScene> p_Scene;
	std::vector<std::unique_ptr<Explosion>> p_Explosions;

	int m_Wave;
	int m_SpawnCounter;
	float m_SpawnTimerAmount;
	float m_SpawnTimer;

	void SpawnUFO(void);
	void SpawnExplosion(Vector3 position, float size);
	void ResetTimer(void);
};