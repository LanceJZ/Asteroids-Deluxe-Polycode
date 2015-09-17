#include "EnemyShip.h"

EnemyShip::EnemyShip()
{
	m_Points = 200;
}

void EnemyShip::Setup(std::shared_ptr<CollisionScene> scene, std::shared_ptr<Player> player, std::shared_ptr<UFOControl> ufo)
{
	p_Scene = scene;
	p_Player = player;
	p_UFO = ufo;

	//The left of screen, positive X is the direction for rotation zero.
	//The top of the screen, positive Y is the direction for rotation zero.
	// Enemy ship width, 24 pixels. Length 22 pixels. Back indent 6 pixels. 

	m_ShipMesh = new SceneMesh(Mesh::LINE_LOOP_MESH);

	m_ShipMesh->getMesh()->addVertex(-1.1, 1.2, 0.0); // Top back tip.
	m_ShipMesh->getMesh()->addVertex(1.1, 0.0, 0.0); // Front point, nose of ship pointing to the left of the screen.
	m_ShipMesh->getMesh()->addVertex(-1.1, -1.2, 0.0); // Bottom back tip.
	m_ShipMesh->getMesh()->addVertex(-0.5, 0.00, 0.0); // Back inside indent.

	m_ShipMesh->cacheToVertexBuffer(true);
	m_ShipMesh->setColor(0.7, 0.7, 0.9, 0.9);
	m_ShipMesh->lineSmooth = true;
	
	Deactivate();
	
	Spawn(Vector3(-m_WindowWidth, m_WindowHeight - 10, 0), 0);

	p_Scene->addChild(m_ShipMesh);
}

void EnemyShip::Update(Number * elapsed)
{
	Location::Update(elapsed);	

	AimAtTarget();

	float rad = (m_Rotation.Amount) * TORADIANS;
	float speed = 10;

	m_Velocity = Vector3(cos(rad) * speed, sin(rad) * speed, 0);

	if (m_Position.x > m_WindowWidth || m_Position.x < -m_WindowWidth)
	{
		m_Done = true;
	}

	SetRotationPosition();

	CheckForEdge();

	if (p_Player->m_Active && !p_Player->m_Hit)
	{
		if (CirclesIntersect(p_Player->Position(), p_Player->m_Radius))
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
					p_Player->Hit();
					p_Player->GotPoints(m_Points);
					m_Hit = true;
				}
			}
		}
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

void EnemyShip::Deactivate(void)
{
	m_ShipMesh->enabled = false;
	m_Active = false;
	m_Hit = false;
	m_Done = false;
}

bool EnemyShip::PlayerNotClear(void)
{
	if (m_Active)
		return CirclesIntersect(Vector3(0, 0, 0), 10);
	else
		return false;
}

void EnemyShip::Enable(void)
{
	m_ShipMesh->enabled = true;
	m_Active = true;
	m_ShieldHit = false;
}

void EnemyShip::AimAtTarget(void)
{
	if (p_Player->m_Active)
	{
		// Figure out what direction to turn. Get the angles in degrees.
		float targetAngle = atan2(p_Player->m_Position.y - m_Position.y, p_Player->m_Position.x - m_Position.x) * TODEGREES;
		float facingAngle = m_Rotation.Amount;
		float targetLessFacing = targetAngle - facingAngle;
		float facingLessTarget = facingAngle - targetAngle;

		// Figure out if it is right or left to turn.
		if (abs(targetLessFacing) > 180)
		{
			if (facingAngle > targetAngle)
			{
				facingLessTarget = ((360 - facingAngle) + targetAngle) * -1;
			}
			else
			{
				facingLessTarget = (360 - targetAngle) + facingAngle;
			}
		}

		if (facingLessTarget > 0)
		{
			m_Rotation.Velocity = -50;
		}
		else if (facingLessTarget < 0)
		{
			m_Rotation.Velocity = 50;
		}
	}
}

void EnemyShip::SetRotationPosition(void)
{
	m_ShipMesh->setPosition(m_Position);
	m_ShipMesh->setRotationEuler(Vector3(0, 0, m_Rotation.Amount));
}