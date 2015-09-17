#include "Shot.h"

Shot::Shot() : Timer(false, 2000)
{
}

void Shot::Update(Number *elapsed)
{
	Location::Update(elapsed);

	m_ShotMesh->setPosition(m_Position);

	CheckForEdge();

	if (Timer::elapsed > m_TimerAmount)
		Deactivate();
}

void Shot::Setup(std::shared_ptr<CollisionScene> scene)
{
	p_Scene = scene;
	m_ShotMesh = new SceneMesh(Mesh::LINE_LOOP_MESH);
	float size = 0.05;

	m_ShotMesh->getMesh()->addVertex(-size, -size, 0);
	m_ShotMesh->getMesh()->addVertex(size, -size, 0);
	m_ShotMesh->getMesh()->addVertex(size, size, 0);
	m_ShotMesh->getMesh()->addVertex(-size, size, 0);
	m_ShotMesh->cacheToVertexBuffer(true);
	p_Scene->addCollisionChild(m_ShotMesh, CollisionEntity::SHAPE_MESH);
	m_ShotMesh->setColor(1, 1, 1, 1);
	m_ShotMesh->enabled = false;
	m_Active = false;
	m_Radius = 1;
}

void Shot::Fire(Vector3 position, Vector3 velocity, float timer)
{
	m_Position = position;
	m_Velocity = velocity;
	m_TimerAmount = timer;
	m_ShotMesh->enabled = true;
	m_Active = true;

	m_ShotMesh->setPosition(m_Position);

	Timer::Reset();
	Timer::setTimerInterval(m_TimerAmount);
}

void Shot::Pause(bool paused)
{
	Timer::Pause(paused);
}

void Shot::Deactivate()
{
	m_ShotMesh->enabled = false;
	m_Active = false;
}