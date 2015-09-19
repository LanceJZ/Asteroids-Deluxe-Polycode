#include "EnemyPod.h"

EnemyPod::EnemyPod()
{
	m_Points = 50;
	m_Speed = 10;
	m_Radius = 12;

	for (int i = 0; i < 3; i++)
	{
		p_Pairs[i] = std::unique_ptr<EnemyPair>(new EnemyPair());
	}
}

void EnemyPod::Setup(std::shared_ptr<CollisionScene> scene)
{
	p_Scene = scene;

	for (int i = 0; i < 3; i++)
	{
		p_Pairs[i]->Setup(scene);
		p_Pairs[i]->m_InPod = true;
	}

	p_Pairs[0]->m_Rotation.Amount = m_Rotation.Amount = 90;
	p_Pairs[1]->m_Rotation.Amount = m_Rotation.Amount = 30;
	p_Pairs[2]->m_Rotation.Amount = m_Rotation.Amount = -30;
}

void EnemyPod::Update(Number * elapsed)
{
	Location::Update(elapsed);

	CheckForEdge();
	SetPosition();
}

void EnemyPod::Pause(bool paused)
{
}

void EnemyPod::Spawn(Vector3 position)
{
	m_Position = position;
	SetPosition();
	Enable();

	for (int i = 0; i < 3; i++)
	{
		p_Pairs[i]->Enable();
	}

	float rad = Random::Number(0, PI * 2);
	float speed = 3;

	m_Velocity = Vector3(cos(rad) * speed, sin(rad) * speed, 0);
}

void EnemyPod::Enable(void)
{
	m_Active = true;
	m_ShieldHit = false;
}

void EnemyPod::SetPosition(void)
{
	p_Pairs[0]->m_Position = m_Position;
	p_Pairs[1]->m_Position = Vector3(2.7, -1.5, 0) + m_Position;
	p_Pairs[2]->m_Position = Vector3(2.7, -0.5, 0) + m_Position;

	for (int i = 0; i < 3; i++)
		p_Pairs[i]->SetRotationPosition();
}

void EnemyPod::Deactivate(void)
{
	m_Active = false;
	m_Hit = false;
	m_Done = false;
}

bool EnemyPod::PlayerNotClear(void)
{
	if (m_Active)
		return CirclesIntersect(Vector3(0, 0, 0), 10);
	else
		return false;
}
