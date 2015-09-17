#pragma once
#include "EnemyShip.h"
#include "EnemyPair.h"
#include <memory>

class EnemyPod : public Location
{
public:
	EnemyPod(void);

	void Setup(std::shared_ptr<CollisionScene> scene, std::shared_ptr<Player> player);
	void Update(Number *elapsed);
	void Pause(bool paused);
	void Spawn(void);
	void Deactivate(void);
	bool PlayerNotClear(void);

	bool m_Hit;
	bool m_Done;
	int m_Points;
	int m_Size;

private:
	std::shared_ptr<CollisionScene> p_Scene;
	std::shared_ptr<Player> p_Player;

	bool m_ShieldHit;
	float m_Speed;

	void Enable(void);
};

