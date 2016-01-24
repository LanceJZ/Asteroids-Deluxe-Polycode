#include "EnemyController.h"

EnemyController::EnemyController() : Timer(false, 10000)
{
	m_SpawnTimerAmount = 600;

	p_Pod = std::unique_ptr<EnemyPod>(new EnemyPod());

	for (int pair = 0; pair < 3; pair++)
		p_Pair[pair] = std::unique_ptr<EnemyPair>(new EnemyPair());

	for (int ship = 0; ship < 6; ship++)
		p_Ship[ship] = std::unique_ptr<EnemyShip>(new EnemyShip());
}

void EnemyController::ResetTimer(void)
{
	m_SpawnTimer = Random::Number(Random::Clip((m_SpawnTimerAmount - (m_Wave * 0.25)), m_SpawnTimerAmount * 0.5, m_SpawnTimerAmount * 1.15),
		Random::Clip((m_SpawnTimerAmount - (m_Wave * 0.25)), m_SpawnTimerAmount * 1.15, m_SpawnTimerAmount * 2.15));

	Timer::Reset();
	Timer::setTimerInterval(m_SpawnTimer);
}

void EnemyController::SpawnPod(void)
{
	p_Pod->Spawn();	
}

void EnemyController::SpawnPairs(void)
{
	for (int pair = 0; pair < 3; pair++)
	{
		p_Pair[pair]->Spawn(p_Pod->p_Pairs[pair]->m_Position, p_Pod->p_Pairs[pair]->m_Rotation.Amount);
	}
}

void EnemyController::SpawnShips(int pair)
{
	int start = (1 * pair) + (1 * pair);

	for (int ship = 0; ship < + 2; ship++)
	{
		p_Ship[ship + start]->Spawn(p_Pair[pair]->p_Ships[ship]->m_Position, p_Pair[pair]->p_Ships[ship]->m_Rotation.Amount);
	}
}

void EnemyController::CheckPlayerHit(void)
{
	for (int pair = 0; pair < 3; pair++)
	{
		if (p_Pair[pair]->CheckPlayerHit() && p_Pair[pair]->m_Active)
		{
			SpawnShips(pair);
			p_Pair[pair]->Deactivate();
		}
	}

	if (p_Pod->m_Hit && p_Pod->m_Active)
	{
		SpawnPairs();
		p_Pod->Deactivate();
	}
}

void EnemyController::Setup(std::shared_ptr<CollisionScene> scene, std::shared_ptr<Player> player, std::shared_ptr<UFOControl> ufo)
{
	p_Scene = scene;
	p_Player = player;
	p_UFO = ufo;

	ResetTimer();

	p_Pod->Setup(scene);

	for (int pair = 0; pair < 3; pair++)
	{
		p_Pod->p_Pairs[pair]->Setup(player, ufo);
	}

	for (int pair = 0; pair < 3; pair++)
	{
		p_Pair[pair]->Setup(scene);
		p_Pair[pair]->Setup(player, ufo);
	}

	for (int ship = 0; ship < 6; ship++)
	{
		p_Ship[ship]->Setup(scene);
		p_Ship[ship]->Setup(player, ufo);
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
		if (Timer::elapsed > m_SpawnTimer && m_SpawnOn)
		{
			SpawnPod();
			ResetTimer();
			m_SpawnCounter++;
			m_SpawnOn = false;
		}
	}

	for (int pair = 0; pair < 3; pair++)
	{
		if (p_Pair[pair]->m_Active)
		{
			p_Pair[pair]->Update(elapsed);
			shipCount++;
		}
	}

	for (int ship = 0; ship < 6; ship++)
	{
		if (p_Ship[ship]->m_Active)
		{
			p_Ship[ship]->Update(elapsed);
			shipCount++;
		}
	}

	if (shipCount < 1)
	{
		m_SpawnOn = true;
	}
	else
	{
		m_SpawnOn = false;
	}

	CheckPlayerHit();
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

	for (int pair = 0; pair < 3; pair++)
		p_Pair[pair]->NewWave(activate);

	for (int ship = 0; ship < 6; ship++)
		p_Ship[ship]->NewWave(activate);
}

void EnemyController::Pause(bool paused)
{
	Timer::Pause(paused);
}

void EnemyController::NewGame(void)
{
	ResetTimer();
	m_SpawnCounter = 0;
	m_Wave = 0;
}