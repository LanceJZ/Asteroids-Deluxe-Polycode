#include "Dot.h"


Dot::Dot()
{
	m_TimerLife = std::unique_ptr<Timer>(new Timer(false, m_TimerAmount));
}

void Dot::Setup(std::shared_ptr<CollisionScene> scene)
{
	m_Scene = scene;

	//m_DotMesh = new SceneMesh(Mesh::POINT_MESH);
	//m_DotMesh->getMesh()->addVertex(0, 0, 0);

	m_DotMesh = new SceneMesh(Mesh::LINE_MESH);
	float size = 0.095;

	m_DotMesh->getMesh()->addVertex(-size, 0, 0);
	//m_DotMesh->getMesh()->addVertex(size, 0, 0);
	//m_DotMesh->getMesh()->addVertex(0, -size, 0);
	m_DotMesh->getMesh()->addVertex(0, size, 0);
	m_DotMesh->cacheToVertexBuffer(true);
	m_DotMesh->enabled = false;
	m_Active = false;
}

void Dot::Update(Number *elapsed)
{
	Location::Update(elapsed);

	m_DotMesh->setPosition(m_Position);

	m_DotMesh->setColor(1, 1, 1, m_Alpha);
	
	if (m_Alpha > 0)
		m_Alpha -= 0.666f ** elapsed;

	if (m_TimerLife->getElapsedf() > m_TimerAmount)
		Deactivate();
}

void Dot::Activate(Vector3 position, float size)
{
	m_Position = position;

	size *= 0.666f;

	if (Random::Number(0, 2) > 1)
		m_Position.x += Random::Number(0, size) - size / 2;
	else
		m_Position.x -= Random::Number(0, size) - size / 2;

	if (Random::Number(0, 2) > 1)
		m_Position.y += Random::Number(0, size) - size / 2;
	else
		m_Position.y -= Random::Number(0, size) - size / 2;
	
	float angle = Random::Number(0, PI * 2);
	float speed = Random::Number(1.5, 6.66);
	m_Velocity.x = cos(angle) * speed;
	m_Velocity.y = sin(angle) * speed;
	m_Velocity.z = 0;

	m_Active = true;
	m_DotMesh->setPosition(m_Position);
	m_DotMesh->enabled = true;
	m_Alpha = 0.85f;
	m_Scene->addChild(m_DotMesh);
	m_TimerLife->Reset();
	m_TimerAmount = Random::Number(0.5, 2);
	m_TimerLife->setTimerInterval(m_TimerAmount);
}

void Dot::Deactivate(void)
{
	m_Scene->removeEntity(m_DotMesh);
	m_DotMesh->enabled = false;
	m_Active = false;
}