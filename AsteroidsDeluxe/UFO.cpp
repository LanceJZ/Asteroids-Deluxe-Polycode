#include "UFO.h"

UFO::UFO(void)
{
	p_FireTimer = std::unique_ptr<Timer>(new Timer(false, 2000));
	p_VectorTimer = std::unique_ptr<Timer>(new Timer(false, 2500));
	m_VectorTimerAmount = 3;
	m_FireTimerAmount = 2;
	m_Speed = 5;

	p_Shot = std::unique_ptr<Shot>(new Shot());
}

void UFO::Setup(std::shared_ptr<CollisionScene> scene, std::shared_ptr<Player> player)
{
	p_Scene = scene;
	p_Player = player;
	
	//The left of screen, positive X is the direction for rotation zero.
	//The top of the screen, positive Y is the direction for rotation zero.

	m_ShipMesh = new SceneMesh(Mesh::LINE_LOOP_MESH);

	m_ShipMesh->getMesh()->addVertex(1.9, -0.4, 0.0); // Left edge Bottom line
	m_ShipMesh->getMesh()->addVertex(0.7, 0.4, 0.0); // Top line left edge
	m_ShipMesh->getMesh()->addVertex(0.3, 1.1, 0.0);
	m_ShipMesh->getMesh()->addVertex(-0.3, 1.1, 0.0);
	m_ShipMesh->getMesh()->addVertex(-0.7, 0.4, 0.0); // Top line right edge
	m_ShipMesh->getMesh()->addVertex(-1.9, -0.4, 0.0); // Right edge bottom line
	m_ShipMesh->getMesh()->addVertex(-0.8, -1.1, 0.0);
	m_ShipMesh->getMesh()->addVertex(0.8, -1.1, 0.0);
	m_ShipMesh->cacheToVertexBuffer(true);
	m_ShipMesh->setColor(0.7, 0.7, 0.9, 0.9);
	m_ShipMesh->lineSmooth = true;

	SceneMesh *m_ShipInsideMesh = new SceneMesh(Mesh::LINE_MESH);

	m_ShipInsideMesh->getMesh()->addVertex(0.7, 0.4, 0.0); // Top inside line
	m_ShipInsideMesh->getMesh()->addVertex(-0.7, 0.4, 0.0);
	m_ShipInsideMesh->getMesh()->addVertex(1.9, -0.4, 0.0); // Bottom inside line
	m_ShipInsideMesh->getMesh()->addVertex(-1.9, -0.4, 0.0);
	m_ShipInsideMesh->cacheToVertexBuffer(true);
	m_ShipInsideMesh->setColor(0.7, 0.7, 0.9, 0.9);
	m_ShipInsideMesh->lineSmooth = true;

	p_Shot->Setup(scene);

	//Sound -------------
	p_ExplodeSound = std::unique_ptr<Sound>(new Sound("audio/UFOExplosion.wav"));
	p_EngineLargeSound = std::unique_ptr<Sound>(new Sound("audio/UFOLarge.wav"));
	p_EngineSmallSound = std::unique_ptr<Sound>(new Sound("audio/UFOSmall.wav"));
	p_ShotSound = std::unique_ptr<Sound>(new Sound("audio/UFOShot.wav"));

	p_ShotSound->setVolume(0.05);
	p_ShotSound->setPitch(1.5);
	p_EngineLargeSound->setVolume(0.075);
	p_EngineSmallSound->setVolume(0.075);
	p_ExplodeSound->setVolume(0.1);
	p_ExplodeSound->setPitch(0.75);

	m_ShipMesh->addChild(m_ShipInsideMesh);
	p_Scene->addCollisionChild(m_ShipMesh, CollisionEntity::SHAPE_MESH);
	m_ShipMesh->enabled = false;
}

void UFO::Update(Number * elapsed)
{
	Location::Update(elapsed);

	m_ShipMesh->setPosition(m_Position);

	if (m_Position.x > m_WindowWidth || m_Position.x < -m_WindowWidth)
	{
		m_Done = true;
	}

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
					if (p_ExplodeSound != NULL)
						p_ExplodeSound->Play();

					p_Player->Hit();
					p_Player->GotPoints(m_Points);
					m_Hit = true;
				}
			}
		}
	}

	for (int i = 0; i < 4; i++)
	{
		if (p_Player->ShotActive(i))
		{
			if (CirclesIntersect(p_Player->ShotPosition(i), p_Player->ShotRadius(i)))
			{
				CollisionResult *rockVsPlayerShot = &p_Scene->testCollision(m_ShipMesh, p_Player->ShotMesh(i));

				if (rockVsPlayerShot->collided)
				{
					if (p_ExplodeSound != NULL)
						p_ExplodeSound->Play();

					p_Player->DeactivateShot(i);
					p_Player->GotPoints(m_Points);
					m_Hit = true;
					break;
				}
			}
		}
	}

	if (p_FireTimer->getElapsedf() > m_FireTimerAmount)
	{
		if (!p_Shot->m_Active)
		{
			if (m_AimedShot && p_Player->m_Active)
			{
				float var = Random::Number(0, 10);

				if (var < 7.5)
				{
					FireAimedShot();
				}
				else
				{
					FireRandomShot();
				}
			}
			else
			{
				FireRandomShot();
			}
		}
	}

	if (p_VectorTimer->getElapsedf() > m_VectorTimerAmount)
	{
		ChangeVector();
	}

}

void UFO::UpdateShot(Number * elapsed)
{
	p_Shot->Update(elapsed);

	if (p_Player->m_Active && !p_Player->m_Hit)
	{
		if (p_Shot->CirclesIntersect(p_Player->Position(), p_Player->m_Radius))
		{
			if (p_Player->m_ShieldOn)
			{
				CollisionResult *shotvsShield = &p_Scene->testCollision(p_Shot->m_ShotMesh, p_Player->m_ShieldMesh);

				if (shotvsShield)
				{
					if (!m_ShotHitShield)
					{
						p_Player->ShieldHit(0, true);
						p_Shot->Deactivate();
					}
				}
				else
					m_ShotHitShield = false;
			}
			else
			{
				CollisionResult *shotvsPlayer = &p_Scene->testCollision(p_Shot->m_ShotMesh, p_Player->m_ShipMesh);

				if (shotvsPlayer->collided)
				{
					p_Player->Hit();
					p_Shot->Deactivate();
				}
			}
		}
	}
}

void UFO::Spawn(int size)
{
	m_Size = size;
	m_Hit = false;
	float var = Random::Number(0, 10);

	if (var > 5)
	{
		m_Position.x = -m_WindowWidth;
		m_Velocity.x = m_Speed;
	}
	else
	{
		m_Position.x = m_WindowWidth;
		m_Velocity.x = -m_Speed;
	}

	m_Position.y = Random::Number(0, m_WindowHeight * 2) - m_WindowHeight;

	Enable();

	if (m_Size == 0)
	{
		m_AimedShot = false;
		m_Points = 200;
		m_ShipMesh->setScale(Vector3(1, 1, 1));

		m_Radius = 5.5f;

		if (!p_Player->m_GameOver)
		{
			if (p_EngineLargeSound != NULL)
				p_EngineLargeSound->Play(true);
		}
	}
	else if (m_Size == 1)
	{
		m_AimedShot = true;
		m_Points = 1000;
		m_ShipMesh->setScale(Vector3(0.5, 0.5, 0.5));

		m_Radius = 3.5f;

		if (!p_Player->m_GameOver)
		{
			if (p_EngineSmallSound != NULL)
				p_EngineSmallSound->Play(true);
		}
	}

	ChangeVector();
}

void UFO::Pause(bool paused)
{
	if (m_Active && !p_Player->m_GameOver)
	{
		if (paused)
		{
			if (m_Size == 0)
				if (p_EngineLargeSound != NULL)
					p_EngineLargeSound->Stop();

			if (m_Size == 1)
				if (p_EngineSmallSound != NULL)
					p_EngineSmallSound->Stop();
		}
		else
		{
			if (m_Size == 0)
				if (p_EngineLargeSound != NULL)
					p_EngineLargeSound->Play(true);

			if (m_Size == 1)
				if (p_EngineSmallSound != NULL)
					p_EngineSmallSound->Play(true);
		}

		p_FireTimer->Pause(paused);
		p_VectorTimer->Pause(paused);
	}

	p_Shot->Pause(paused);
}

float UFO::ShotRadius(void)
{
	return p_Shot->m_Radius;
}

bool UFO::ShotActive(void)
{
	return p_Shot->m_Active;
}

bool UFO::PlayerNotClear(void)
{
	if (m_Active)
		return CirclesIntersect(Vector3(0, 0, 0), 10);
	else
		return false;
}

void UFO::DeactivateShot(void)
{
	p_Shot->Deactivate();
}

SceneMesh * UFO::ShotMesh(void)
{
	return p_Shot->m_ShotMesh;
}

void UFO::Enable(void)
{
	m_ShipMesh->setPosition(m_Position);
	m_ShipMesh->enabled = true;
	m_Active = true;
	m_ShieldHit = false;
	m_ShotHitShield = false;
	ResetFireTimer();
}

void UFO::ResetFireTimer(void)
{
	p_FireTimer->Reset();
	p_FireTimer->setTimerInterval(m_FireTimerAmount);
}

void UFO::ResetVectorTimer(void)
{
	p_VectorTimer->Reset();
	p_VectorTimer->setTimerInterval(m_VectorTimerAmount);
}

void UFO::ChangeVector(void)
{
	// if going strait go up or down. If not going strait go strait.

	float var = Random::Number(0, 10);

	if (var < 5)
	{
		if (m_Velocity.y == 0 && var < 2.5)
			m_Velocity.y = m_Speed;
		else if (m_Velocity.y == 0)
			m_Velocity.y = -m_Speed;
		else
			m_Velocity.y = 0;
	}

	ResetVectorTimer();
}

void UFO::FireShot(float directionInRadians)
{
	float speed = 27;

	p_Shot->Fire(Vector3(cos(directionInRadians) * 1.9, sin(directionInRadians) * 1.9, 0) + m_Position,
		Vector3(cos(directionInRadians) * speed, sin(directionInRadians) * speed, 0) + (m_Velocity * 0.25), 1300);

	ResetFireTimer();

	if (p_ShotSound != NULL)
	{
		if (!p_Player->m_GameOver)
			p_ShotSound->Play();
	}
}

void UFO::FireAimedShot(void)
{
	FireShot(atan2(p_Player->Position().y - m_Position.y, p_Player->Position().x - m_Position.x));
}

void UFO::FireRandomShot(void)
{
	FireShot(Random::Number(0, PI * 2));
}

void UFO::Deactivate(void)
{
	m_ShipMesh->enabled = false;
	m_Active = false;
	m_Hit = false;
	m_Done = false;

	if (p_EngineLargeSound != NULL)
		p_EngineLargeSound->Stop();

	if (p_EngineSmallSound != NULL)
		p_EngineSmallSound->Stop();
}