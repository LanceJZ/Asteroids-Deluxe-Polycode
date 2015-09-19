#pragma once
#include "Location.h"
#include "Player.h"
#include "UFOControl.h"
#include "PlayerChase.h"

class EnemyShip : public Location, PlayerChase
{
public:
	EnemyShip(void);

	void Setup(std::shared_ptr<Player> player, std::shared_ptr<UFOControl> ufo);
	void Setup(std::shared_ptr<CollisionScene> scene);
	void Update(Number *elapsed);
	void Spawn(Vector3 position, float rotation);
	void SetRotationPosition(void);
	void Pause(bool paused);
	void Deactivate(void);
	bool PlayerNotClear(void);

	bool m_Hit;
	bool m_Done;
	bool m_InPair;

	SceneMesh *m_ShipMesh;

private:
	std::shared_ptr<CollisionScene> p_Scene;
	std::shared_ptr<Player> p_Player;
	std::shared_ptr<UFOControl> p_UFO;

	bool m_FromRight;
	bool m_ShieldHit;
	float m_Speed;
	int m_Points;

	void Enable(void);
	void CheckPlayerHit(void);
};

