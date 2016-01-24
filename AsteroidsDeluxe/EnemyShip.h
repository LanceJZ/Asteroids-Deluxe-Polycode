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
	void NewWave(bool activated);
	void Enable(void);
	void Deactivate(void);
	bool CheckPlayerHit(void);

	bool m_Hit;
	bool m_Done;
	bool m_InPair;

	SceneMesh *m_ShipMesh;

private:
	std::shared_ptr<CollisionScene> p_Scene;
	std::shared_ptr<Player> p_Player;
	std::shared_ptr<UFOControl> p_UFO;

	bool m_ShieldHit;
	bool m_NewWave;
	float m_Speed;
	int m_Points;
};

