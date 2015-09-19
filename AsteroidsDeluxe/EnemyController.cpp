#include "EnemyController.h"

EnemyController::EnemyController() : Timer(false, 10000)
{
	p_Pod = std::unique_ptr<EnemyPod>(new EnemyPod());
	p_Ship = std::unique_ptr<EnemyShip>(new EnemyShip());
	p_Pair = std::unique_ptr<EnemyPair>(new EnemyPair());
}

void EnemyController::ResetTimer(void)
{
	m_SpawnTimer = Random::Number(Random::Clip((m_SpawnTimerAmount - (m_Wave * 0.25)), m_SpawnTimerAmount * 0.5, m_SpawnTimerAmount * 1.15),
		Random::Clip((m_SpawnTimerAmount - (m_Wave * 0.25)),
		m_SpawnTimerAmount * 1.15, m_SpawnTimerAmount * 2.15));

	Timer::Reset();
	Timer::setTimerInterval(m_SpawnTimer);
}

void EnemyController::SpawnEnemyPod(void)
{
}

void EnemyController::Setup(std::shared_ptr<CollisionScene> scene, std::shared_ptr<Player> player, std::shared_ptr<UFOControl> ufo)
{
	p_Scene = scene;
	p_Player = player;
	p_UFO = ufo;

	ResetTimer();

	p_Ship->Setup(scene);
	p_Ship->Setup(player, ufo);
	p_Ship->Spawn(Vector3(-30, -20, 0), 0);
	p_Pair->Setup(scene);
	p_Pair->Setup(player, ufo);
	p_Pair->Spawn(Vector3(-30, 20, 0), 0);
	p_Pod->Setup(scene);
	p_Pod->Spawn(Vector3(-30, 0, 0));
}

void EnemyController::Update(Number * elapsed)
{
	p_Pod->Update(elapsed);
	p_Ship->Update(elapsed);
	p_Pair->Update(elapsed);
}

void EnemyController::WaveNumber(int Wave)
{
	m_Wave = Wave;
}

void EnemyController::Pause(bool paused)
{
	Timer::Pause(paused);
}

void EnemyController::HitRock(void)
{
	ResetTimer();
	Deactivate();
}

void EnemyController::Deactivate(void)
{
}

void EnemyController::NewGame(void)
{
	ResetTimer();
	Deactivate();
	m_SpawnCounter = 0;
	m_Wave = 0;
}

SceneMesh * EnemyController::PodBody(void)
{
	return nullptr;
}

SceneMesh * EnemyController::PairBody(void)
{
	return nullptr;
}

SceneMesh * EnemyController::ShipBody(void)
{
	return nullptr;
}

Vector3 EnemyController::PositionPod(void)
{
	return Vector3();
}

Vector3 EnemyController::PositionPair(void)
{
	return Vector3();
}

Vector3 EnemyController::PositionShip(void)
{
	return Vector3();
}

bool EnemyController::Active(void)
{
	return false;
}

bool EnemyController::PlayerNotClear(void)
{
	return false;
}
