#include "EnemyPair.h"

EnemyPair::EnemyPair()
{
	// When player explodes, heads off edge of screen. Pod spawns after player does.
	// If rocks are cleared, the new wave of rocks wont spawn until all of these are destroyed or player dies.

	m_Points = 100;
	m_Speed = 10;
	m_Radius = 6;
	m_InPod = false;
	m_Active = false;

	for (int i = 0; i < 2; i++)
	{
		p_Ships[i] = std::unique_ptr<EnemyShip>(new EnemyShip());
	}
}

void EnemyPair::Setup(std::shared_ptr<Player> player, std::shared_ptr<UFOControl> ufo)
{
	p_Player = player;
	p_UFO = ufo;
}

void EnemyPair::Setup(std::shared_ptr<CollisionScene> scene)
{
	p_Scene = scene;

	for (int i = 0; i < 2; i++)
	{
		p_Ships[i]->Setup(scene);
		p_Ships[i]->m_InPair = true;
	}
}

void EnemyPair::Update(Number * elapsed)
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

void EnemyPair::Spawn(Vector3 position, float rotation)
{
	m_Position = position;
	m_Rotation.Amount = rotation;
	SetRotationPosition();
	Enable();
}

void EnemyPair::Pause(bool paused)
{
}

void EnemyPair::NewWave(bool activated)
{
	m_NewWave = activated;
}

void EnemyPair::Deactivate(void)
{
	for (int i = 0; i < 2; i++)
		p_Ships[i]->Deactivate();

	m_Active = false;
	m_Done = false;
}

void EnemyPair::Enable(void)
{
	for (int i = 0; i < 2; i++)
		p_Ships[i]->Enable();

	m_Active = true;
	m_ShieldHit = false;
	m_NewWave = false;
	m_Hit = false;
}

void EnemyPair::SetRotationPosition(void)
{
	p_Ships[0]->m_Position = m_Position;

	float rotRad = m_Rotation.Amount * TORADIANS;
	p_Ships[1]->m_Position = Vector3(cos(rotRad) * -2.0, sin(rotRad) * -2.0, 0) + m_Position;

	p_Ships[0]->m_Rotation = m_Rotation;
	p_Ships[1]->m_Rotation.Amount = m_Rotation.Amount - 180;

	for (int i = 0; i < 2; i++)
		p_Ships[i]->SetRotationPosition();
}

bool EnemyPair::CheckPlayerHit(void)
{
	if (p_Player->m_Active && !p_Player->m_Hit)
	{
		if (CirclesIntersect(p_Player->m_Position, p_Player->m_Radius))
		{
			for (int i = 0; i < 2; i++)
			{
				if (p_Player->m_ShieldOn)
				{
					CollisionResult *vsShield = &p_Scene->testCollision(p_Ships[i]->m_ShipMesh, p_Player->m_ShieldMesh);

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
					CollisionResult *vsPlayer = &p_Scene->testCollision(p_Ships[i]->m_ShipMesh, p_Player->m_ShipMesh);

					if (vsPlayer->collided)
					{
						if (!m_InPod)
						{
							p_Player->GotPoints(m_Points);
							m_InPod = false;
						}

						p_Player->Hit();
						m_Hit = true;
					}
				}

			}
		}
	}

	for (int s = 0; s < 4; s++)
	{
		if (p_Player->p_Shots[s]->m_Active)
		{
			if (CirclesIntersect(p_Player->p_Shots[s]->m_Position, p_Player->p_Shots[s]->m_Radius))
			{
				for (int i = 0; i < 2; i++)
				{
					CollisionResult *vsPlayerShot = &p_Scene->testCollision(p_Ships[i]->m_ShipMesh, p_Player->p_Shots[s]->m_ShotMesh);

					if (vsPlayerShot->collided)
					{
						if (!m_InPod)
						{
							p_Player->GotPoints(m_Points);
							m_InPod = false;
						}

						m_Hit = true;
						p_Player->DeactivateShot(i);
						break;
					}
				}
					
			}
		}
	}

	return m_Hit;
}
