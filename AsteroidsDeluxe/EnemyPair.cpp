#include "EnemyPair.h"

EnemyPair::EnemyPair()
{
	// When player explodes, heads off edge of screen. Pod spawns after player does.
	// If rocks are cleared, the new wave of rocks wont spawn until all of these are destroyed or player dies.

	m_Speed = 10;
	m_Active = false;

	for (int ship = 0; ship < 2; ship++)
	{
		p_Ships[ship] = std::unique_ptr<EnemyShip>(new EnemyShip());
	}
}

void EnemyPair::Setup(std::shared_ptr<Player> player, std::shared_ptr<UFOControl> ufo)
{
	p_Player = player;
	p_UFOs = ufo;

	for (int ship = 0; ship < 2; ship++)
		p_Ships[ship]->Setup(player, ufo);
}

void EnemyPair::Setup(std::shared_ptr<CollisionScene> scene)
{
	for (int ship = 0; ship < 2; ship++)
	{
		p_Ships[ship]->Setup(scene);
	}
}

void EnemyPair::Update(Number * elapsed)
{
	EnemyShared::Update(elapsed);
	Location::Update(elapsed);

	SetRotationPosition();

	if (m_OffScreen)
		Deactivate();
}

void EnemyPair::Spawn(Vector3 position, float rotation)
{
	EnemyShared::Spawn(position, rotation);

	SetRotationPosition();
	Enable();
}

void EnemyPair::Pause(bool paused)
{
}

void EnemyPair::Deactivate(void)
{
	EnemyShared::Deactivate();

	for (int ship = 0; ship < 2; ship++)
		p_Ships[ship]->Deactivate();
}

void EnemyPair::Enable(void)
{
	EnemyShared::Enable();

	for (int ship = 0; ship < 2; ship++)
		p_Ships[ship]->Enable();
}

void EnemyPair::SetRotationPosition(void)
{
	p_Ships[0]->m_Position = m_Position;

	float rotRad = m_Rotation.Amount * TORADIANS;
	p_Ships[1]->m_Position = Vector3(cos(rotRad) * -2.0, sin(rotRad) * -2.0, 0) + m_Position;

	p_Ships[0]->m_Rotation = m_Rotation;
	p_Ships[1]->m_Rotation.Amount = m_Rotation.Amount - 180;

	for (int ship = 0; ship < 2; ship++)
		p_Ships[ship]->SetRotationPosition();
}

bool EnemyPair::CheckPlayerHit(void)
{
	for (int ship = 0; ship < 2; ship++)
	{
		if (m_Hit = p_Ships[ship]->CheckPlayerHit())
			break;
	}

	return m_Hit;
}

bool EnemyPair::CheckUFOHit(void)
{
	for (int ship = 0; ship < 2; ship++)
	{
		if (m_Hit = p_Ships[ship]->CheckUFOHit())
			break;
	}

	return m_Hit;
}
