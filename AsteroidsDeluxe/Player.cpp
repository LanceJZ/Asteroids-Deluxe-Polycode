#include "Player.h"

Player::Player(void)
{
	m_Hit = false;
	m_GameOver = true;
	m_GameOverDisplay = 1;

	m_ClearTimerAmount = 2.5;
	m_ExplodeTimerAmount = 2.75;
	m_GameOverTimerAmount = 6.66f;
	m_FlameTimerAmount = 0.00666f;
	p_ExplodeTimer = std::unique_ptr<Timer>(new Timer(false, m_ExplodeTimerAmount));
	p_ClearTimer = std::unique_ptr<Timer>(new Timer(false, m_ClearTimerAmount));
	p_GameOverTimer = std::unique_ptr<Timer>(new Timer(false, m_GameOverTimerAmount));
	p_FlameTimer = std::unique_ptr<Timer>(new Timer(false, m_FlameTimerAmount));

	for (int i = 0; i < 6; i++)
	{
		p_ExpLoc[i] = std::unique_ptr<Location>(new Location());
	}
}

void Player::Setup(std::shared_ptr<CollisionScene> scene)
{
	p_Scene = scene;
	//Player original ship is 23 (11.5 x 2) pixels long, and 14 (8 x 2) pixels wide.
	//The bottom is not flat. It starts 4 pixels from the side and 4 from the bottom and goes down at an angle to the bottom corner.
	//The bottom is flat only from the upper part to the other side point 4 from the other side.
	//The points are with the front at positive X with the center at zero. Positive X is to the left of the screen.
	//The left of screen, positive X is the direction for rotation zero.
	//The top of the screen, positive Y is the direction for rotation zero.

	m_ShipMesh = new SceneMesh(Mesh::LINE_LOOP_MESH);
	m_ShipWingMesh = new SceneMesh(Mesh::LINE_LOOP_MESH);
	m_ShipLivesMesh = new SceneMesh(Mesh::LINE_LOOP_MESH);
	m_ShieldMesh = new SceneMesh(Mesh::LINE_LOOP_MESH);
	m_FlameMesh = new SceneMesh(Mesh::LINE_MESH);

	for (int i = 0; i < 6; i++)
	{
		m_ShipExplosionMesh[i] = new SceneMesh(Mesh::LINE_MESH);		
	}

	m_ShipExplosionMesh[0]->getMesh()->addVertex(-1.15, 0.8, 0.0); //Top back tip.
	m_ShipExplosionMesh[0]->getMesh()->addVertex(-0.1, 0.4, 0.0); // Half way to nose pointing to the left of screen.

	m_ShipExplosionMesh[1]->getMesh()->addVertex(-0.1, 0.4, 0.0); // Half way to nose pointing to the left of screen.
	m_ShipExplosionMesh[1]->getMesh()->addVertex(1.15, 0, 0.0); //Nose pointing to the left of screen.

	m_ShipExplosionMesh[2]->getMesh()->addVertex(-1.15, -0.8, 0.0); //Bottom back tip.
	m_ShipExplosionMesh[2]->getMesh()->addVertex(0.1, -0.4, 0.0); // Half way to nose pointing to the left of screen.

	m_ShipExplosionMesh[3]->getMesh()->addVertex(0.1, -0.4, 0.0); // Half way to nose pointing to the left of screen.
	m_ShipExplosionMesh[3]->getMesh()->addVertex(1.15, 0, 0.0); //Nose pointing to the left of screen.

	m_ShipExplosionMesh[4]->getMesh()->addVertex(-0.95, -0.4, 0.0); //Bottom inside back.
	m_ShipExplosionMesh[4]->getMesh()->addVertex(-0.95, 0, 0.0); // Half way to bottom back tip.

	m_ShipExplosionMesh[5]->getMesh()->addVertex(-0.95, 0.4, 0.0); //Top inside back.
	m_ShipExplosionMesh[5]->getMesh()->addVertex(-0.95, 0, 0.0); //Half way to top back tip.

	for (int i = 0; i < 6; i++)
	{
		m_ShipExplosionMesh[i]->cacheToVertexBuffer(true);
		m_ShipExplosionMesh[i]->lineSmooth = true;
		p_Scene->addChild(m_ShipExplosionMesh[i]);
		m_ShipExplosionMesh[i]->enabled = false;
	}

	m_ShipLivesMesh->cacheToVertexBuffer(true);
	m_ShipLivesMesh->lineSmooth = true;
	m_ShipLivesMesh->setColor(0.9, 0.9, 1.0, 1.0);

	//Ship mesh front pointing to left of screen.
	m_ShipMesh->getMesh()->addVertex(-1.15, 0.8, 0.0); //Top back tip.
	m_ShipMesh->getMesh()->addVertex(1.15, 0, 0.0); //Nose pointing to the left of screen.
	m_ShipMesh->getMesh()->addVertex(-1.15, -0.8, 0.0); //Bottom back tip.
	m_ShipMesh->getMesh()->addVertex(-0.95, -0.4, 0.0); //Bottom inside back.
	m_ShipMesh->getMesh()->addVertex(-0.95, 0.4, 0.0); //Top inside back.
	m_ShipMesh->cacheToVertexBuffer(true);
	m_ShipMesh->lineSmooth = true;
	m_ShipMesh->setColor(0.9, 0.9, 1.0, 1.0);

	m_ShipWingMesh->getMesh()->addVertex(-0.55, 1.25, 0); //Top wing tip.
	m_ShipWingMesh->getMesh()->addVertex(0.15, 0, 0); // Front of wing middle of ship.
	m_ShipWingMesh->getMesh()->addVertex(-0.55, -1.25, 0); // Bottom wing tip.
	m_ShipWingMesh->getMesh()->addVertex(-1.15, -0.4, 0); // Connect to bottom back tip.
	m_ShipWingMesh->getMesh()->addVertex(-1.15, 0.4, 0); // Connect to top back tip.
	m_ShipWingMesh->cacheToVertexBuffer(true);
	m_ShipWingMesh->lineSmooth = true;
	m_ShipWingMesh->setColor(0.9, 0.9, 1.0, 1.0);

	m_ShipMesh->addChild(m_ShipWingMesh);

	m_ShipLivesMesh->addChild(m_ShipMesh->Clone(false, true));
	m_ShipLivesMesh->addChild(m_ShipWingMesh);

	m_Radius = 2.5f;

	m_FlameMesh->getMesh()->addVertex(-0.95, -0.7, 0.0); //Bottom inside back.
	m_FlameMesh->getMesh()->addVertex(-1.6, 0.0, 0.0); //Tip of flame.
	m_FlameMesh->getMesh()->addVertex(-0.95, 0.7, 0.0); //Top inside back.
	m_FlameMesh->getMesh()->addVertex(-1.6, 0.0, 0.0); //Tip of flame.
	m_FlameMesh->cacheToVertexBuffer(true);
	m_FlameMesh->lineSmooth = true;
	m_FlameMesh->setColor(0.9, 0.9, 1.0, 1.0);

	m_ShipMesh->addChild(m_FlameMesh);
	m_FlameMesh->enabled = false;

	for (int i = 0; i < 8; i++)
	{
		float rad = 1.75;
		float rot = PI / 8;

		m_ShieldMesh->getMesh()->addVertex(rad * cos(2 * PI * i / 8 + rot), rad * sin(2 * PI * i / 8 + rot), 0);
	}

	m_ShieldMesh->cacheToVertexBuffer(true);
	m_ShieldMesh->lineSmooth = true;
	m_ShieldMesh->setColor(0.9, 0.9, 1.0, 1.0);
	
	for (int i = 0; i < 4; i++)
	{
		p_Shots[i] = std::unique_ptr<Shot>(new Shot());
		p_Shots[i]->Setup(scene);
	}

	p_HUD = std::unique_ptr<HUD>(new HUD());
	p_HUD->Setup(scene);

	p_Scene->addCollisionChild(m_ShipMesh, CollisionEntity::SHAPE_MESH);
	p_Scene->addCollisionChild(m_ShieldMesh, CollisionEntity::SHAPE_MESH);
	m_ShipMesh->enabled = false;
	m_ShieldMesh->enabled = false;
	ResetGameOverTimer();

	//Sounds -----
	p_ShotSound = std::unique_ptr<Sound>(new Sound("audio/PlayerShot.ogg"));
	p_ShotSound->setVolume(0.15);
	p_ShotSound->setPitch(1.25);
	p_ExplodeSound = std::unique_ptr<Sound>(new Sound("audio/PlayerExplosion.wav"));
	p_ExplodeSound->setVolume(0.1);
	p_ExplodeSound->setPitch(0.5);
	p_ThrustSound = std::unique_ptr<Sound>(new Sound("audio/Thrust.ogg"));
	p_ThrustSound->setVolume(0.5);
	p_ThrustSound->setPitch(0.75);
}

float Player::ShotRadius(int shot)
{
	return p_Shots[shot]->m_Radius;
}

void Player::Activate(void)
{
	m_ShipMesh->enabled = true;
	m_Active = true;
	m_GameOver = false;
	m_Hit = false;
	m_ThrustOn = false;
}

void Player::Deactivate(void)
{
	m_Active = false;
	m_ShipMesh->enabled = false;
}

void Player::NewGame(void)
{
	m_Rotation.Amount = 90;
	Reset();
	Activate();
	p_HUD->NewGame();
	p_HUD->Add(0);
	
	UpdateLivesDisplay();
}

void Player::Reset(void)
{
	m_Hit = false;	
	m_ShipMesh->setRotationEuler(Vector3(0, 0, m_Rotation.Amount));
	m_Position = Vector3(0, 0, 0);
	m_Velocity = Vector3(0, 0, 0);
	m_Acceleration = Vector3(0, 0, 0);
	m_ShipMesh->enabled = true;
	m_ShieldMesh->enabled = false;
	m_ShieldPower = 100;
}

void Player::DeactivateShot(int shot)
{
	p_Shots[shot]->Deactivate();
}

void Player::Pause(bool paused)
{
	for (int i = 0; i < 4; i++)
	{
		p_Shots[i]->Pause(paused);
	}
}

bool Player::ShotActive(int shot)
{
	return p_Shots[shot]->m_Active;
}

Vector3 Player::Position(void)
{
	return m_Position;
}

Vector3 Player::ShotPosition(int shot)
{
	return p_Shots[shot]->m_Position;
}

SceneMesh *Player::ShotMesh(int shot)
{
	return p_Shots[shot]->m_ShotMesh;
}

void Player::Update(Number *elapsed)
{
	Location::Update(elapsed);

	if (m_Hit)
	{
		UpdateExplode(elapsed);

		if (p_ExplodeTimer->getElapsedf() > m_ExplodeTimerAmount)
		{
			m_Spawn = true;

			for (int i = 0; i < 6; i++)
				m_ShipExplosionMesh[i]->enabled = false;

			if (m_GameOver)
			{
				Deactivate();
				p_HUD->GameOver();
			}
			else if (m_ClearToSpawn)
				Reset();
		}
	}
	else
	{
		if (m_ThrustOn)
			ApplyThrust();
		else
		{
			m_Acceleration = 0;

			if (m_Velocity.x > 0 || m_Velocity.y > 0)
			{
				if (m_Velocity.x > 0)
					m_Acceleration.x = -0.025;

				if (m_Velocity.y > 0)
					m_Acceleration.y = -0.025;
			}

			if (m_Velocity.x < 0 || m_Velocity.y < 0)
			{
				if (m_Velocity.x < 0)
					m_Acceleration.x = 0.025;

				if (m_Velocity.y < 0)
					m_Acceleration.y = 0.025;
			}
		}

		if (m_ShieldOn)
		{
			if (m_ShieldPower > 0)
			{
				m_ShieldPower += -1 * *elapsed;

				if (m_ShieldPower > 10)
					m_ShieldMesh->setColor(1, 1, 1, (m_ShieldPower - 10) * .01);
			}
			else
			{
				m_ShieldOn = false;
				m_ShieldMesh->enabled = false;
			}
		}
	}

	m_ShipMesh->setPosition(m_Position);
	m_ShieldMesh->setPosition(m_Position);
	m_ShipMesh->setRotationEuler(Vector3(0, 0, m_Rotation.Amount));
	m_ShieldMesh->setRotationEuler(Vector3(0, 0, m_Rotation.Amount));

	CheckForEdge();
}

void Player::UpdateShots(Number * elapsed)
{
	for (int i = 0; i < 4; i++)
	{
		if (p_Shots[i]->m_Active)
			p_Shots[i]->Update(elapsed);
	}
}

void Player::UpdateGameOver(void)
{
	if (!p_HUD->m_NewHighScore)
	{
		if (p_GameOverTimer->getElapsedf() > m_GameOverTimerAmount)
		{
			ResetGameOverTimer();
			p_HUD->DisplayHighScores(m_GameOverDisplay, true);

			if (m_GameOverDisplay == 0)
				m_GameOverDisplay = 1;
			else
				m_GameOverDisplay = 0;

		}
	}
}

void Player::TurnLeft(void)
{

	if (!m_Hit)
		m_Rotation.Velocity = -200;
	else
		m_Rotation.Velocity = 0;
}

void Player::TurnRight(void)
{
	if (!m_Hit)
		m_Rotation.Velocity = 200;
	else
		m_Rotation.Velocity = 0;
}

void Player::TurnOff(void)
{
	m_Rotation.Velocity = 0;
}

void Player::Hyperspace(void)
{
	if (!m_Hit)
	{
		m_Velocity = Vector3(0, 0, 0);
		m_Position.x = Random::Number(3, m_WindowWidth * 2 - 3) - m_WindowWidth;
		m_Position.y = Random::Number(3, m_WindowHeight * 2 - 3) - m_WindowHeight;
	}
}

void Player::Hit(void)
{
	if (!m_Hit)
	{
		if (p_ThrustSound != NULL)
		{
			p_ThrustSound->Stop();
		}

		p_ExplodeSound->Play();
		p_HUD->LostLife();
		m_Hit = true;
		m_Spawn = false;
		m_ClearToSpawn = false;
		m_Velocity = m_Velocity * 0.1;
		m_Acceleration = 0;
		m_Rotation.Velocity = 0;

		ResetExplodeTimer();
		StartExplode();
		UpdateLivesDisplay();

		if (p_HUD->Lives() < 1)
		{
			m_GameOver = true;
		}
	}
}

void Player::ShieldHit(Vector3 velocity, bool shot)
{
	m_ShieldPower -= 10;

	if (shot)
		m_ShieldPower -= 10;
	else
		m_Velocity += velocity * 0.25 + (m_Velocity * 1.25) * -1;
}

void Player::UpdateLivesDisplay(void)
{
	for (size_t i = 0; i < p_ShipLives.size(); i++)
	{
		p_Scene->removeEntity(p_ShipLives.at(i));
	}

	p_ShipLives.clear();

	Vector3 livesPos = Vector3(m_WindowWidth - 4, m_WindowHeight + -5, 0);

	for (int i = 0; i < p_HUD->Lives(); i++)
	{
		p_ShipLives.push_back(m_ShipLivesMesh->Clone(true, true));
		p_ShipLives.at(p_ShipLives.size() - 1)->setColor(1.0, 1.0, 1.0, 0.95);
		p_ShipLives.at(p_ShipLives.size() - 1)->setPosition(livesPos);
		p_ShipLives.at(p_ShipLives.size() - 1)->setRotationEuler(Vector3(0, 0, 90));
		p_Scene->addChild(p_ShipLives.at(p_ShipLives.size() - 1));
		livesPos.x -= 3;
	}
}

void Player::GotPoints(int points)
{
	p_HUD->Add(points);

	UpdateLivesDisplay();
}

void Player::SetClear(void)
{
	m_ClearToSpawn = true;
}

bool Player::CheckClear(void)
{
	return false;
}

void Player::ThrustOn(void)
{
	m_ThrustOn = true;
}

void Player::ThrustOff(void)
{
	m_ThrustOn = false;
	m_FlameMesh->enabled = false;

	if (p_ThrustSound != NULL)
	{
		p_ThrustSound->Stop();
	}
}

void Player::ShieldOn(void)
{
	if (!m_Hit && !m_GameOver)
	{
		if (m_ShieldPower > 0 && !m_ShieldOn)
		{
			m_ShieldMesh->enabled = true;
			m_ShieldOn = true;
		}
	}
	else
		ShieldOff();
}

void Player::ShieldOff(void)
{
	m_ShieldMesh->enabled = false;
	m_ShieldOn = false;
}

void Player::ApplyThrust(void)
{
	// Convert degrees to radians. (Why wont people use radians in mesh math?)
	float rad = (m_Rotation.Amount) * TORADIANS;

	float thrustAmount = 0.45;

	m_Acceleration = Vector3(cos(rad) * thrustAmount, sin(rad) * thrustAmount, 0);

	float maxSp = 80;

	m_Velocity.x = clampf(m_Velocity.x, -maxSp, maxSp);
	m_Velocity.y = clampf(m_Velocity.y, -maxSp, maxSp);

	if (p_FlameTimer->getElapsedf() > m_FlameTimerAmount)
	{
		ResetFlameTimer();

		if (m_FlameMesh->enabled)
			m_FlameMesh->enabled = false;
		else
			m_FlameMesh->enabled = true;
	}

	if (p_ThrustSound != NULL)
	{
		if (!m_GameOver && !m_Hit)
			if (!p_ThrustSound->isPlaying())
				p_ThrustSound->Play(true);
	}
}

void Player::StartExplode(void)
{
	for (int i = 0; i < 6; i++)
	{
		p_ExpLoc[i]->m_Position = m_Position;
		p_ExpLoc[i]->m_Rotation = m_Rotation;
		p_ExpLoc[i]->m_Rotation.Velocity = Random::Number(1, 50);

		Vector3 vel = 0;
		float angle = Random::Number(0, PI * 2);
		float speed = Random::Number(0.666, 1.666);
		vel.x = cos(angle) * speed;
		vel.y = sin(angle) * speed;


		p_ExpLoc[i]->m_Velocity = m_Velocity + vel;

		m_ShipExplosionMesh[i]->setPosition(p_ExpLoc[i]->m_Position);
		m_ShipExplosionMesh[i]->setRotationEuler(Vector3(0, 0, p_ExpLoc[i]->m_Rotation.Amount));
		m_ShipExplosionMesh[i]->enabled = true;
	}

	m_ShipMesh->enabled = false;
}

void Player::UpdateExplode(Number *elapsed)
{
	//m_ShipMesh->setColor(1.0, 0.1, 0.1, 0.8);
	for (int i = 0; i < 6; i++)
	{
		p_ExpLoc[i]->Update(elapsed);

		m_ShipExplosionMesh[i]->setPosition(p_ExpLoc[i]->m_Position);
		m_ShipExplosionMesh[i]->setRotationEuler(Vector3(0, 0, p_ExpLoc[i]->m_Rotation.Amount));
	}

}

void Player::ResetExplodeTimer(void)
{
	p_ExplodeTimer->Reset();
	p_ExplodeTimer->setTimerInterval(m_ExplodeTimerAmount);
}

void Player::ResetClearTimer(void)
{
	p_ClearTimer->Reset();
	p_ClearTimer->setTimerInterval(m_ClearTimerAmount);
}

void Player::ResetGameOverTimer(void)
{
	p_GameOverTimer->Reset();
	p_GameOverTimer->setTimerInterval(m_GameOverTimerAmount);
}

void Player::ResetFlameTimer(void)
{
	p_FlameTimer->Reset();
	p_FlameTimer->setTimerInterval(m_FlameTimerAmount);
}

void Player::FireShot(void)
{
	if (!m_Hit)
	{
		for (int i = 0; i < 4; i++)
		{
			if (!p_Shots[i]->m_Active)
			{
				float speed = 35;
				float rad = (m_Rotation.Amount) * PI / 180.0;

				Vector3 direction = Vector3(cos(rad) * speed, sin(rad) * speed, 0);
				Vector3 position = Vector3(cos(rad) * 1.15, sin(rad) * 1.15, 0);

				p_Shots[i]->Fire(position + m_Position, direction + m_Velocity * 0.5, 1500);

				if (p_ShotSound != NULL)
					p_ShotSound->Play();
				break;
			}
		}
	}
}