#include "EnemyPod.h"

EnemyPod::EnemyPod()
{
	// When player explodes, stays on screen, like a rock, leaves off edge of screen.

	m_Speed = 10;
	m_Active = false;

	for (int pair = 0; pair < 3; pair++)
	{
		p_Pairs[pair] = std::unique_ptr<EnemyPair>(new EnemyPair());
	}
}

void EnemyPod::Setup(std::shared_ptr<CollisionScene> scene)
{
	for (int pair = 0; pair < 3; pair++)
	{
		p_Pairs[pair]->Setup(scene);
	}

	p_Pairs[0]->m_Rotation.Amount = m_Rotation.Amount = 90;
	p_Pairs[1]->m_Rotation.Amount = m_Rotation.Amount = 30;
	p_Pairs[2]->m_Rotation.Amount = m_Rotation.Amount = -30;
}

void EnemyPod::Update(Number * elapsed)
{
	Location::Update(elapsed);
	
	SetPosition();
	CheckPlayerHit();

	if (m_NewWave)
	{
		if (m_Position.x > m_WindowWidth || m_Position.x < -m_WindowWidth
			|| m_Position.y > m_WindowHeight || m_Position.y < -m_WindowHeight)
			Deactivate();
	}
	else
		CheckForEdge();
}

void EnemyPod::Pause(bool paused)
{
}

void EnemyPod::NewWave(bool activated)
{
	m_NewWave = activated;
}

void EnemyPod::Spawn()
{
	m_Position = Vector3(m_WindowWidth, Random::Number(0, m_WindowHeight) - Random::Number(0, m_WindowHeight), 0);
	SetPosition();
	Enable();

	for (int pair = 0; pair < 3; pair++)
	{
		p_Pairs[pair]->Enable();
	}

	float rad = Random::Number(0.75, (PI * 2) - 0.75);
	float speed = 3;

	m_Velocity = Vector3(cos(rad) * speed, sin(rad) * speed, 0);
}

void EnemyPod::Enable(void)
{
	m_Active = true;
	m_NewWave = false;
	m_Hit = false;
}

void EnemyPod::SetPosition(void)
{
	p_Pairs[0]->m_Position = m_Position;
	p_Pairs[1]->m_Position = Vector3(2.7, -1.5, 0) + m_Position;
	p_Pairs[2]->m_Position = Vector3(2.7, -0.5, 0) + m_Position;

	for (int pair = 0; pair < 3; pair++)
		p_Pairs[pair]->SetRotationPosition();
}

bool EnemyPod::CheckPlayerHit(void)
{
	for (int pair = 0; pair < 3; pair++)
	{
		if (m_Hit = p_Pairs[pair]->CheckPlayerHit())
			break;
	}

	return m_Hit;
}

bool EnemyPod::CheckUFOHit(void)
{
	for (int pair = 0; pair < 3; pair++)
	{
		if (m_Hit = p_Pairs[pair]->CheckUFOHit())
			break;
	}

	return m_Hit;
}

void EnemyPod::Deactivate(void)
{
	m_Active = false;
	m_Done = false;

	for (int pair = 0; pair < 3; pair++)
		p_Pairs[pair]->Deactivate();
}

bool EnemyPod::PlayerNotClear(void)
{
	if (m_Active)
		return CirclesIntersect(Vector3(0, 0, 0), 10);
	else
		return false;
}
