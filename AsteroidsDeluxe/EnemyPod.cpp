#include "EnemyPod.h"

EnemyPod::EnemyPod()
{
}

void EnemyPod::Setup(std::shared_ptr<CollisionScene> scene, std::shared_ptr<Player> player)
{
	p_Scene = scene;
	p_Player = player;
}

void EnemyPod::Update(Number * elapsed)
{
}

void EnemyPod::Pause(bool paused)
{
}

void EnemyPod::Spawn(void)
{
}

void EnemyPod::Enable(void)
{
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
