#include "UFOControl.h"

UFOControl::UFOControl() : Timer(false, 10000)
{
	m_SpawnTimerAmount = 10015;
	m_SpawnCounter = 0;
	ResetTimer();
	p_UFO = std::unique_ptr<UFO>(new UFO());
}

void UFOControl::Setup(std::shared_ptr<CollisionScene> scene, std::shared_ptr<Player> player)
{
	p_Scene = scene;

	ResetTimer();

	p_UFO->Setup(scene, player);
}

void UFOControl::Update(Number * elapsed)
{
	if (p_UFO->p_Shot->m_Active)
		p_UFO->UpdateShot(elapsed);

	if (p_UFO->m_Active)
	{
		p_UFO->Update(elapsed);

		if (p_UFO->m_Hit)
		{
			SpawnExplosion(p_UFO->m_Position, p_UFO->m_Radius);
			p_UFO->Deactivate();
			ResetTimer();
		}

		if (p_UFO->m_Done)
		{
			p_UFO->Deactivate();
			ResetTimer();
		}
	}
	else
	{
		if (Timer::elapsed > m_SpawnTimer && !p_UFO->m_Active)
		{
			SpawnUFO();
			ResetTimer();
			m_SpawnCounter++;
		}
	}

	for (int i = 0; i < p_Explosions.size(); i++)
	{
		if (p_Explosions[i]->m_Active)
		{
			p_Explosions[i]->Update(elapsed);
		}
	}
}

void UFOControl::WaveNumber(int Wave)
{
	m_Wave = Wave;
}

void UFOControl::Pause(bool paused)
{
	p_UFO->Pause(paused);
	Timer::Pause(paused);
}

void UFOControl::HitRock(void)
{
	ResetTimer();
	Deactivate();
}

//Vector3 UFOControl::Position(void)
//{
//	return p_UFO->m_Position;
//}
//
//float UFOControl::ShotRadius(void)
//{
//	return p_UFO->ShotRadius();
//}
//
//float UFOControl::Radius(void)
//{
//	return p_UFO->m_Radius;
//}
//
//bool UFOControl::Active(void)
//{
//	return p_UFO->m_Active;
//}
//
//bool UFOControl::PlayerNotClear(void)
//{
//	return p_UFO->PlayerNotClear();
//}
//
//SceneMesh * UFOControl::ShipBody(void)
//{
//	return p_UFO->m_ShipMesh;
//}

void UFOControl::Deactivate(void)
{
	p_UFO->Deactivate();
}

void UFOControl::DeactivateShot(void)
{
	p_UFO->p_Shot->Deactivate();
}

void UFOControl::NewGame(void)
{
	ResetTimer();
	Deactivate();
	DeactivateShot();
	m_SpawnCounter = 0;
	m_Wave = 0;
}

//SceneMesh * UFOControl::ShotMesh(void)
//{
//	return p_UFO->ShotMesh();
//}
//
//bool UFOControl::ShotActive(void)
//{
//	return p_UFO->ShotActive();
//}

void UFOControl::SpawnUFO()
{
	float spawnPercent = (float)(pow(0.915, (m_SpawnCounter * 2) / ((m_Wave * 2) + 1)));
	int size;

	// Size 0 is the large one.
	if (Random::Number(0, 99) < spawnPercent * 100)
		size = 0;
	else
		size = 1;

	p_UFO->Spawn(size);
}

void UFOControl::SpawnExplosion(Vector3 position, float size)
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

void UFOControl::ResetTimer()
{
	m_SpawnTimer = Random::Number(Random::Clip((m_SpawnTimerAmount - (m_Wave * 0.25)), m_SpawnTimerAmount * 0.5, m_SpawnTimerAmount * 1.15),
		Random::Clip((m_SpawnTimerAmount - (m_Wave * 0.25)),
		m_SpawnTimerAmount * 1.15, m_SpawnTimerAmount * 2.15));

	Timer::Reset();
	Timer::setTimerInterval(m_SpawnTimer);
}
