#pragma once
#include "EnemyPod.h"

class EnemyController : public Timer
{
public:
	EnemyController(void);

	void Setup(std::shared_ptr<CollisionScene> scene, std::shared_ptr<Player> player, std::shared_ptr<UFOControl> ufo);
	void Update(Number *elapsed);
	void WaveNumber(int wave);
	void TimeToSpawn(bool activate);
	void NewWave(bool activate);
	void Pause(bool paused);
	void HitRock(void);
	void Deactivate(void);
	void NewGame(void);

	SceneMesh *PodBody(void);
	SceneMesh *PairBody(void);
	SceneMesh *ShipBody(void);

	Vector3 PositionPod(void);
	Vector3 PositionPair(void);
	Vector3 PositionShip(void);

	bool Active(void);
	bool PlayerNotClear(void);

private:
	std::shared_ptr<CollisionScene> p_Scene;
	std::shared_ptr<Player> p_Player;
	std::shared_ptr<UFOControl> p_UFO;
	std::unique_ptr<EnemyPod> p_Pod;
	std::unique_ptr<EnemyPair> p_Pair[3];
	std::unique_ptr<EnemyShip> p_Ship[6];

	bool m_SpawnOn;
	bool m_NewWave;
	int m_Wave;
	int m_SpawnCounter;
	float m_SpawnTimerAmount;
	float m_SpawnTimer;

	void ResetTimer(void);
	void SpawnEnemyPod(void);
};