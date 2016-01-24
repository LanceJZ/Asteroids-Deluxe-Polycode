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
	void NewGame(void);

	std::unique_ptr<EnemyPair> p_Pair[3];
	std::unique_ptr<EnemyShip> p_Ship[6];
	std::unique_ptr<EnemyPod> p_Pod;

private:
	std::shared_ptr<CollisionScene> p_Scene;
	std::shared_ptr<Player> p_Player;
	std::shared_ptr<UFOControl> p_UFO;

	bool m_SpawnOn;
	bool m_NewWave;
	int m_Wave;
	int m_SpawnCounter;
	float m_SpawnTimerAmount;
	float m_SpawnTimer;

	void ResetTimer(void);
	void SpawnPod(void);
	void SpawnPairs(void);
	void SpawnShips(int pair);
	void CheckPlayerHit(void);
};