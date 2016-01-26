#include "EnemyController.h"

EnemyController::EnemyController() : Timer(false, 10000)
{
	m_SpawnTimerAmount = 30000;

	p_Pod = std::unique_ptr<EnemyPod>(new EnemyPod());

	for (int pair = 0; pair < 3; pair++)
		p_Pair[pair] = std::unique_ptr<EnemyPair>(new EnemyPair());

	for (int ship = 0; ship < 6; ship++)
		p_Ship[ship] = std::unique_ptr<EnemyShip>(new EnemyShip());

	p_SpawnSound = std::unique_ptr<Sound>(new Sound("audio/PodSpawn.wav"));
	p_ExplosionSound = std::unique_ptr<Sound>(new Sound("audio/PodExplosion.wav"));
	p_ExplosionSound->setVolume(0.1);
	p_SpawnSound->setVolume(0.25);
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
	if (p_SpawnSound != NULL && !p_Player->m_GameOver)
		p_SpawnSound->Play();

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
	for (int ship = 0; ship < 6; ship++)
	{
		if (p_Ship[ship]->m_Active)
		{
			if (p_Ship[ship]->CheckPlayerHit() || p_Ship[ship]->CheckUFOHit())
			{
				SpawnExplosion(p_Ship[ship]->m_Position, 1);
				p_Ship[ship]->Deactivate();
			}

			if (p_Ship[ship]->CheckPlayerHit())
				p_Player->GotPoints(200);
		}

	}


	for (int pair = 0; pair < 3; pair++)
	{
		if (p_Pair[pair]->m_Active)
		{
			if (p_Pair[pair]->CheckPlayerHit() || p_Pair[pair]->CheckUFOHit())
			{
				SpawnExplosion(p_Pair[pair]->m_Position, 2);
				SpawnShips(pair);
				p_Pair[pair]->Deactivate();
			}

			if (p_Pair[pair]->CheckPlayerHit())
				p_Player->GotPoints(100);
		}
	}

	if (p_Pod->m_Active)
	{
		if (p_Pod->CheckPlayerHit() || p_Pod->CheckUFOHit())
		{
			SpawnExplosion(p_Pod->m_Position, 3);
			SpawnPairs();
			p_Pod->Deactivate();
		}

		if (p_Pod->CheckPlayerHit())
			p_Player->GotPoints(50);
	}
}

void EnemyController::SpawnExplosion(Vector3 position, float size)
{
	if (p_ExplosionSound != NULL && !p_Player->m_GameOver)
		p_ExplosionSound->Play();

	bool spawnExp = true;

	for (int expCheck = 0; expCheck < p_Explosions.size(); expCheck++)
	{
		if (!p_Explosions[expCheck]->m_Active)
		{
			spawnExp = false;
			p_Explosions[expCheck]->Activate(position, size);
			break;
		}
	}

	if (spawnExp)
	{
		p_Explosions.push_back(std::unique_ptr<Explosion>(new Explosion()));
		p_Explosions[p_Explosions.size() - 1]->Setup(p_Scene);
		p_Explosions[p_Explosions.size() - 1]->Activate(position, size);
	}
}

void EnemyController::Setup(std::shared_ptr<CollisionScene> scene, std::shared_ptr<Player> player, std::shared_ptr<UFOControl> ufo)
{
	p_Scene = scene;
	p_Player = player;
	p_UFOs = ufo;

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
			shipCount = 1;
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

	for (int exp = 0; exp < p_Explosions.size(); exp++)
	{
		if (p_Explosions[exp]->m_Active)
		{
			p_Explosions[exp]->Update(elapsed);
		}
	}
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
	NewWave(true);
	ResetTimer();
	m_SpawnCounter = 0;
	m_Wave = 0;

	if (p_Pod->m_Active)
		p_Pod->Deactivate();

	for (int pair = 0; pair < 3; pair++)
	{
		if (p_Pair[pair]->m_Active)
			p_Pair[pair]->Deactivate();
	}

	for (int ship = 0; ship < 6; ship++)
	{
		if (p_Ship[ship]->m_Active)
			p_Ship[ship]->Deactivate();
	}
}