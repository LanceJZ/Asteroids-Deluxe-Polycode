#include "EnemyShip.h"

EnemyShip::EnemyShip()
{
	// When player explodes, heads off edge of screen. Pod spawns after player does.
	// If rocks are cleared, the new wave of rocks wont spawn until all of these are destroyed or player dies.

	m_Points = 200;
	m_InPair = false;
	m_Speed = 10;
	m_Radius = 3;
	m_Active = false;
}

void EnemyShip::Setup(std::shared_ptr<Player> player, std::shared_ptr<UFOControl> ufo)
{
	p_Player = player;
	p_UFO = ufo;
}

void EnemyShip::Setup(std::shared_ptr<CollisionScene> scene)
{
	p_Scene = scene;

	//The left of screen, positive X is the direction for rotation zero.
	//The top of the screen, positive Y is the direction for rotation zero.
	//Enemy ship width is 24 pixels. Length 21 pixels. Back indent 6 pixels.

	m_ShipMesh = new SceneMesh(Mesh::LINE_LOOP_MESH);

	m_ShipMesh->getMesh()->addVertex(-1.0, 1.2, 0.0); // Top back tip.
	m_ShipMesh->getMesh()->addVertex(1.0, 0.0, 0.0); // Front point, nose of ship pointing to the left of the screen.
	m_ShipMesh->getMesh()->addVertex(-1.0, -1.2, 0.0); // Bottom back tip.
	m_ShipMesh->getMesh()->addVertex(-0.4, 0.0, 0.0); // Back inside indent.

	m_ShipMesh->cacheToVertexBuffer(true);
	m_ShipMesh->setColor(0.7, 0.7, 0.9, 1.0);
	m_ShipMesh->lineSmooth = true;

	Deactivate();

	p_Scene->addChild(m_ShipMesh);
}

void EnemyShip::Update(Number * elapsed)
{
	Location::Update(elapsed);	

	SetRotationPosition();

	if (m_NewWave)
	{
		m_Rotation.Velocity = 0;

		if (m_Position.x > m_WindowWidth || m_Position.x < -m_WindowWidth
			|| m_Position.y > m_WindowHeight || m_Position.y < -m_WindowHeight)
			Deactivate();
	}
	else
	{
		if (p_Player->m_Active)
			m_Rotation.Velocity = AimAtTarget(m_Position, p_Player->m_Position, m_Rotation.Amount);

		float rad = (m_Rotation.Amount) * TORADIANS;
		m_Velocity = Vector3(cos(rad) * m_Speed, sin(rad) * m_Speed, 0);
		CheckForEdge();
	}
}

void EnemyShip::Spawn(Vector3 position, float rotation)
{
	m_Position = position;
	m_Rotation.Amount = rotation;
	SetRotationPosition();
	Enable();
}

void EnemyShip::Pause(bool paused)
{
}

void EnemyShip::NewWave(bool activated)
{
	m_NewWave = activated;
}

void EnemyShip::Deactivate(void)
{
	m_ShipMesh->enabled = false;
	m_Active = false;
	m_Hit = false;
	m_Done = false;
	p_Scene->removeCollision(m_ShipMesh);
	p_Scene->removeEntity(m_ShipMesh);
}

void EnemyShip::Enable(void)
{
	m_ShipMesh->enabled = true;
	m_Active = true;
	m_ShieldHit = false;
	m_NewWave = false;
	p_Scene->addCollisionChild(m_ShipMesh, CollisionEntity::SHAPE_MESH);
}

void EnemyShip::SetRotationPosition(void)
{
	m_ShipMesh->setPosition(m_Position);
	m_ShipMesh->setRotationEuler(Vector3(0, 0, m_Rotation.Amount));
}

bool EnemyShip::CheckPlayerHit(void)
{
	if (p_Player->m_Active && !p_Player->m_Hit)
	{
		if (CirclesIntersect(p_Player->m_Position, p_Player->m_Radius))
		{
			if (p_Player->m_ShieldOn)
			{
				CollisionResult *vsShield = &p_Scene->testCollision(m_ShipMesh, p_Player->m_ShieldMesh);

				if (vsShield->collided)
				{
					if (!m_ShieldHit)
					{
						p_Player->ShieldHit(m_Velocity * 3, false);
						m_ShieldHit = true;
					}
				}
				else
					m_ShieldHit = false;
			}
			else
			{
				CollisionResult *vsPlayer = &p_Scene->testCollision(m_ShipMesh, p_Player->m_ShipMesh);

				if (vsPlayer->collided)
				{
					if (!m_InPair)
						p_Player->GotPoints(m_Points);

					p_Player->Hit();
					m_Hit = true;
				}
			}
		}
	}

	for (int ps = 0; ps < 4; ps++)
	{
		if (p_Player->p_Shots[ps]->m_Active)
		{
			if (CirclesIntersect(p_Player->p_Shots[ps]->m_Position, p_Player->p_Shots[ps]->m_Radius))
			{
				CollisionResult *vsPlayerShot = &p_Scene->testCollision(m_ShipMesh, p_Player->p_Shots[ps]->m_ShotMesh);

				if (vsPlayerShot->collided)
				{
					if (!m_InPair)
						p_Player->GotPoints(m_Points);

					m_Hit = true;
					p_Player->DeactivateShot(ps);
					break;
				}
			}
		}
	}

	return m_Hit;
}
