#include "EnemyController.h"

EnemyController::EnemyController() : Timer(false, 10000)
{
	m_SpawnTimerAmount = 60;

	p_Pod = std::unique_ptr<EnemyPod>(new EnemyPod());

	for (int i = 0; i < 3; i++)
		p_Pair[i] = std::unique_ptr<EnemyPair>(new EnemyPair());

	for (int i = 0; i < 6; i++)
		p_Ship[i] = std::unique_ptr<EnemyShip>(new EnemyShip());
}

void EnemyController::ResetTimer(void)
{
	m_SpawnTimer = Random::Number(Random::Clip((m_SpawnTimerAmount - (m_Wave * 0.25)), m_SpawnTimerAmount * 0.5, m_SpawnTimerAmount * 1.15),
		Random::Clip((m_SpawnTimerAmount - (m_Wave * 0.25)), m_SpawnTimerAmount * 1.15, m_SpawnTimerAmount * 2.15));

	Timer::Reset();
	Timer::setTimerInterval(m_SpawnTimer);
}

void EnemyController::SpawnEnemyPod(void)
{
	p_Pod->Spawn();	
}

void EnemyController::Setup(std::shared_ptr<CollisionScene> scene, std::shared_ptr<Player> player, std::shared_ptr<UFOControl> ufo)
{
	p_Scene = scene;
	p_Player = player;
	p_UFO = ufo;

	ResetTimer();

	p_Pod->Setup(scene);	

	for (int i = 0; i < 3; i++)
	{
		p_Pair[i]->Setup(scene);
		p_Pair[i]->Setup(player, ufo);
	}

	for (int i = 0; i < 6; i++)
	{
		p_Ship[i]->Setup(scene);
		p_Ship[i]->Setup(player, ufo);
	}
}

void EnemyController::Update(Number * elapsed)
{
	int shipCount = 0;

	if (p_Pod->m_Active)
	{
		p_Pod->Update(elapsed);
		shipCount = 1;
	}
	else if (m_SpawnOn && !m_NewWave)
	{
		if (Timer::elapsed > m_SpawnTimer && !p_Pod->m_Active)
		{
			SpawnEnemyPod();
			ResetTimer();
			m_SpawnCounter++;
			m_SpawnOn = false;
		}
	}

	for (int i = 0; i < 3; i++)
	{
		if (p_Pair[i]->m_Active)
		{
			p_Pair[i]->Update(elapsed);		
			shipCount++; 
		}
	}


	for (int i = 0; i < 6; i++)
	{
		if (p_Ship[i]->m_Active)
		{
			p_Ship[i]->Update(elapsed);
			shipCount++;
		}
	}

	if (shipCount < 1)
		m_SpawnOn = true;
}

void EnemyController::WaveNumber(int Wave)
{
	m_Wave = Wave;
}

void EnemyController::TimeToSpawn(bool activate)
{
	m_SpawnOn = activate;
	ResetTimer();
}

void EnemyController::NewWave(bool activate)
{
	m_NewWave = activate;
	
	p_Pod->NewWave(activate);

	for (int i = 0; i < 3; i++)
		p_Pair[i]->NewWave(activate);

	for (int i = 0; i < 6; i++)
		p_Ship[i]->NewWave(activate);
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
