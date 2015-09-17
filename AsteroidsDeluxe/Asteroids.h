#pragma once
#include <memory>
#include <Polycode.h>
#include "Random.h"
#include "Player.h"
#include "RockControl.h"
#include "UFOControl.h"
#include "EnemyController.h"

class Asteroids : public EventHandler
{
public:
	Asteroids(PolycodeView *view);
	~Asteroids();

	void handleEvent(Event *events);
	void handlePlayerInput(void);
	bool Update(void);

private:	
	Core * pCore;
	std::shared_ptr<CollisionScene> p_Scene;

	bool m_Exit;
	bool m_Paused;
	bool m_FiredShot;
	bool m_Hyper;
};
