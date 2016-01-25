#pragma once
#include "Location.h"
#include "Player.h"
#include "UFOControl.h"
#include "PlayerChase.h"

class EnemyShared :	public Location, public PlayerChase
{
public:
	EnemyShared();
	
	void Update(Number *elapsed);
	void Spawn(Vector3 position, float rotation);
	void Pause(bool paused);
	void NewWave(bool activated);
	void Enable(void);
	void Deactivate(void);

	bool m_Hit;

protected:
	std::shared_ptr<Player> p_Player;
	std::shared_ptr<UFOControl> p_UFOs;
	bool m_NewWave;
	float m_Speed;
	bool m_Done;
	bool m_OffScreen;
};

