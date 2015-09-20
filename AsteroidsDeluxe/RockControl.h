#pragma once
#include <vector>
#include "Rock.h"
#include "Explosion.h"
#include "EnemyController.h"

struct RockCount
{
	int numberActive;
	int numberHit;
	bool playerAllClear;
};

class RockControl
{
public:
	RockControl(void);

	void Setup(std::shared_ptr<CollisionScene> scene, std::shared_ptr<Player> player,
		std::shared_ptr<UFOControl> ufo, std::shared_ptr<EnemyController> enemy);
	void Update(Number *elapsed);
	void Pause(bool paused);
	void NewGame(void);

private:
	bool m_EnemySpawnOn;
	bool m_SoundOn;
	int m_NumberOfRocks;
	int m_NumberOfRocksLastFrame;
	int m_NumberOfRocksThisFrame;
	int m_NumberOfRocksHit;
	int m_Wave;

	std::shared_ptr<Player> p_Player;
	std::shared_ptr<UFOControl> p_UFO;
	std::shared_ptr<EnemyController> p_Enemy;

	std::vector<std::unique_ptr<Explosion>> p_Explosions;
	std::vector<std::unique_ptr<Rock>> p_LargeRocks;
	std::vector<std::unique_ptr<Rock>> p_MedRocks;
	std::vector<std::unique_ptr<Rock>> p_SmallRocks;

	std::shared_ptr<CollisionScene> p_Scene;

	std::unique_ptr<Sound> p_BackgroundSound;

	RockCount UpdateSmallRocks(Number *elapsed, int numberActive, bool clear);
	RockCount UpdateMedRocks(Number *elapsed, int numberActive, bool clear);
	RockCount UpdateLargeRocks(Number *elapsed, int numberActive, bool clear);

	void CreateRocks(void);
	void ClearAllRocks(void);
	void SpawnExplosion(Vector3 position, float size);
	void SpawnNewWave(int NumberOfRocks);
	void SpawnMedRocks(Vector3 position);
	void SpawnSmallRocks(Vector3 position);
};
