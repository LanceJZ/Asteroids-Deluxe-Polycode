#include "EnemyShared.h"

EnemyShared::EnemyShared()
{
}

void EnemyShared::Update(Number * elapsed)
{
	if (m_NewWave || m_Done)
	{
		m_Rotation.Velocity = 0;

		if (m_Position.x > m_WindowWidth || m_Position.x < -m_WindowWidth
			|| m_Position.y > m_WindowHeight || m_Position.y < -m_WindowHeight)
			m_OffScreen = true;
	}
	else
	{
		if (!p_Player->m_Hit && p_Player->m_Active)
			m_Rotation.Velocity = AimAtTarget(m_Position, p_Player->m_Position, m_Rotation.Amount);
		else if (p_UFOs->p_UFO->m_Active)
			m_Rotation.Velocity = AimAtTarget(m_Position, p_UFOs->p_UFO->m_Position, m_Rotation.Amount);
		else
			m_Done = true;

		float rad = (m_Rotation.Amount) * TORADIANS;
		m_Velocity = Vector3(cos(rad) * m_Speed, sin(rad) * m_Speed, 0);
		CheckForEdge();
	}
}

void EnemyShared::Spawn(Vector3 position, float rotation)
{
	m_Position = position;
	m_Rotation.Amount = rotation;
}

void EnemyShared::Pause(bool paused)
{
}

void EnemyShared::NewWave(bool activated)
{
	m_NewWave = activated;
}

void EnemyShared::Enable(void)
{
	m_Active = true;
	m_NewWave = false;
	m_Done = false;
	m_Hit = false;
	m_OffScreen = false;
}

void EnemyShared::Deactivate(void)
{
	m_Active = false;
}