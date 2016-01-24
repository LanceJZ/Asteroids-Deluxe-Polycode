#include "RockControl.h"

RockControl::RockControl(void)
{
}

void RockControl::Setup(std::shared_ptr<CollisionScene> scene, std::shared_ptr<Player> player,
	std::shared_ptr<UFOControl> ufo, std::shared_ptr<EnemyController> enemy)
{
	p_Scene = scene;
	p_Player = player;
	p_UFO = ufo;
	p_Enemy = enemy;

	NewGame();

	//Sound ------
	p_BackgroundSound = std::unique_ptr<Sound>(new Sound("audio/Background.ogg"));
	p_BackgroundSound->setVolume(0.25);
	m_SoundOn = false;
}

void RockControl::Update(Number *elapsed)
{
	m_NumberOfRocksLastFrame = m_NumberOfRocksThisFrame;

	RockCount lRC = UpdateLargeRocks(elapsed, 0, true);

	RockCount mRC = UpdateMedRocks(elapsed, lRC.numberActive, lRC.playerAllClear);

	RockCount rocksCounted = UpdateSmallRocks(elapsed, mRC.numberActive, mRC.playerAllClear);

	if (!m_EnemySpawnOn)
	{
		if (rocksCounted.numberActive < 4)
		{
			p_Enemy->TimeToSpawn(true);
			p_Enemy->NewWave(false);
			m_EnemySpawnOn = true;
		}
	}

	if (p_Player->m_Hit && p_Player->m_Spawn)
	{
		if (rocksCounted.playerAllClear && !p_UFO->p_UFO->p_Shot->m_Active && !p_UFO->p_UFO->PlayerNotClear())
		{
			p_Player->SetClear();
		}
	}

	m_NumberOfRocksThisFrame = rocksCounted.numberActive;
	m_NumberOfRocksHit += lRC.numberHit + mRC.numberHit + rocksCounted.numberHit;

	if (m_NumberOfRocksLastFrame != m_NumberOfRocksThisFrame)
	{
		Number pitch = 1;
		pitch = clampf(sqrtf((clampf(m_NumberOfRocksHit - m_Wave * 9, 0, 162)) * 0.15), 1, 4);

		p_BackgroundSound->setPitch(pitch);
	}
	
	if (rocksCounted.numberActive < 1)
	{
		p_Enemy->TimeToSpawn(false);
		p_Enemy->NewWave(true);

		if (!p_Enemy->p_Pod->m_Active)
		{
			if (m_NumberOfRocks < 18)
				m_NumberOfRocks++;

			m_Wave++;

			p_UFO->WaveNumber(m_Wave);

			SpawnNewWave(m_NumberOfRocks);
			m_NumberOfRocksHit = 0;
		}
	}

	for (int i = 0; i < p_Explosions.size(); i++)
	{
		if (p_Explosions[i]->m_Active)
		{
			p_Explosions[i]->Update(elapsed);
		}
	}

	if (m_SoundOn)
	{
		if (p_Player->m_GameOver)
		{
			p_BackgroundSound->Stop();
			m_SoundOn = false;
		}
	}
}

void RockControl::Pause(bool paused)
{
	if (!p_Player->m_GameOver)
	{
		if (paused)
			p_BackgroundSound->Stop();
		else
			p_BackgroundSound->Play();
	}
}

void RockControl::NewGame(void)
{
	for (size_t rock = 0; rock < p_LargeRocks.size(); rock++)
	{
		p_LargeRocks.at(rock)->Deactivate();
	}

	for (size_t rock = 0; rock < p_MedRocks.size(); rock++)
	{
		p_MedRocks.at(rock)->Deactivate();
	}

	for (size_t rock = 0; rock < p_SmallRocks.size(); rock++)
	{
		p_SmallRocks.at(rock)->Deactivate();
	}

	for (size_t exp = 0; exp < p_Explosions.size(); exp++)
	{
		p_Explosions.at(exp)->Deactivate();
	}

	m_NumberOfRocks = 4;
	m_NumberOfRocksHit = 0;
	m_Wave = 0;
	SpawnNewWave(m_NumberOfRocks);

	if (p_BackgroundSound != NULL)
	{
		if (!p_Player->m_GameOver)
		{
			p_BackgroundSound->Play(true);
			m_SoundOn = true;
		}
	}
}

RockCount RockControl::UpdateSmallRocks(Number * elapsed, int numberActive, bool clear)
{
	RockCount rocks;
	rocks.playerAllClear = clear;
	rocks.numberActive = numberActive;
	rocks.numberHit = 0;

	for (size_t rock = 0; rock < p_SmallRocks.size(); rock++)
	{
		if (p_SmallRocks.at(rock)->m_Active)
		{
			p_SmallRocks.at(rock)->Update(elapsed);

			if (p_SmallRocks.at(rock)->m_Hit)
			{
				SpawnExplosion(p_SmallRocks.at(rock)->m_Position, p_SmallRocks.at(rock)->m_Radius / 2);
				p_SmallRocks.at(rock)->Deactivate();
				rocks.numberHit++;
			}

			rocks.numberActive++;
		}
	}

	if (p_Player->m_Hit)
	{
		if (rocks.playerAllClear)
		{
			for (size_t rock = 0; rock < p_SmallRocks.size(); rock++)
			{
				if (p_SmallRocks.at(rock)->PlayerNotClear())
				{
					rocks.playerAllClear = false;
					break;
				}
			}
		}
	}

	return rocks;
}

RockCount RockControl::UpdateMedRocks(Number * elapsed, int numberActive, bool clear)
{
	RockCount rocks;
	rocks.playerAllClear = clear;
	rocks.numberActive = numberActive;
	rocks.numberHit = 0;

	for (size_t rock = 0; rock < p_MedRocks.size(); rock++)
	{
		if (p_MedRocks.at(rock)->m_Active)
		{
			p_MedRocks.at(rock)->Update(elapsed);

			if (p_MedRocks.at(rock)->m_Hit)
			{
				SpawnSmallRocks(p_MedRocks.at(rock)->m_Position);
				SpawnExplosion(p_MedRocks.at(rock)->m_Position, p_MedRocks.at(rock)->m_Radius / 2);
				p_MedRocks.at(rock)->Deactivate();
				rocks.numberHit++;
			}

			rocks.numberActive++;
		}
	}

	if (p_Player->m_Hit)
	{
		if (rocks.playerAllClear)
		{
			for (size_t rock = 0; rock < p_MedRocks.size(); rock++)
			{
				if (p_MedRocks.at(rock)->PlayerNotClear())
				{
					rocks.playerAllClear = false;
					break;
				}
			}
		}
	}

	return rocks;
}

RockCount RockControl::UpdateLargeRocks(Number * elapsed, int numberActive, bool clear)
{
	RockCount rocks;
	rocks.playerAllClear = clear;
	rocks.numberActive = numberActive;
	rocks.numberHit = 0;

	for (size_t rock = 0; rock < p_LargeRocks.size(); rock++)
	{
		if (p_LargeRocks.at(rock)->m_Active)
		{
			p_LargeRocks.at(rock)->Update(elapsed);

			if (p_LargeRocks.at(rock)->m_Hit)
			{
				SpawnMedRocks(p_LargeRocks.at(rock)->m_Position);
				SpawnExplosion(p_LargeRocks.at(rock)->m_Position, p_LargeRocks.at(rock)->m_Radius / 2);
				p_LargeRocks.at(rock)->Deactivate();
				rocks.numberHit++;
			}

			rocks.numberActive++;
		}
	}

	if (p_Player->m_Hit)
	{
		if (rocks.playerAllClear)
		{
			for (size_t rock = 0; rock < p_LargeRocks.size(); rock++)
			{
				if (p_LargeRocks.at(rock)->PlayerNotClear())
				{
					rocks.playerAllClear = false;
					break;
				}
			}
		}
	}

	return rocks;
}

void RockControl::CreateRocks(void)
{
}

void RockControl::ClearAllRocks(void)
{
}

void RockControl::SpawnExplosion(Vector3 position, float size)
{
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

void RockControl::SpawnNewWave(int NumberOfRocks)
{
	m_EnemySpawnOn = false;

	for (int rock = 0; rock < NumberOfRocks; rock++)
	{
		bool spawnnewrock = true;

		for (int rockcheck = 0; rockcheck < p_LargeRocks.size(); rockcheck++)
		{
			if (!p_LargeRocks[rockcheck]->m_Active)
			{
				spawnnewrock = false;
				p_LargeRocks[rockcheck]->Spawn();
				break;
			}
		}

		if (spawnnewrock)
		{
			p_LargeRocks.push_back(std::unique_ptr<Rock>(new Rock()));
			p_LargeRocks[p_LargeRocks.size() - 1]->Setup(p_Scene, 0, p_Player, p_UFO);
			p_LargeRocks[p_LargeRocks.size() - 1]->Spawn();
		}
	}
}

void RockControl::SpawnMedRocks(Vector3 position)
{
	for (int rock = 0; rock < 2; rock++)
	{
		bool spawnnewrock = true;

		for (int rockcheck = 0; rockcheck < p_MedRocks.size(); rockcheck++)
		{
			if (!p_MedRocks[rockcheck]->m_Active)
			{
				spawnnewrock = false;
				p_MedRocks[rockcheck]->Spawn(position);
				break;
			}
		}

		if (spawnnewrock)
		{
			p_MedRocks.push_back(std::unique_ptr<Rock>(new Rock()));
			p_MedRocks[p_MedRocks.size() - 1]->Setup(p_Scene, 1, p_Player, p_UFO);
			p_MedRocks[p_MedRocks.size() - 1]->Spawn(position);
		}
	}
}

void RockControl::SpawnSmallRocks(Vector3 position)
{
	for (int rock = 0; rock < 2; rock++)
	{
		bool spawnnewrock = true;

		for (int rockcheck = 0; rockcheck < p_SmallRocks.size(); rockcheck++)
		{
			if (!p_SmallRocks[rockcheck]->m_Active)
			{
				spawnnewrock = false;
				p_SmallRocks[rockcheck]->Spawn(position);
				break;
			}
		}

		if (spawnnewrock)
		{
			p_SmallRocks.push_back(std::unique_ptr<Rock>(new Rock()));
			p_SmallRocks[p_SmallRocks.size() - 1]->Setup(p_Scene, 2, p_Player, p_UFO);
			p_SmallRocks[p_SmallRocks.size() - 1]->Spawn(position);
		}
	}
}