#pragma once
#include "Location.h"
#include "Player.h"
#include "UFOControl.h"

class EnemyShip : public Location
{
public:
	EnemyShip(void);

	void Setup(std::shared_ptr<CollisionScene> scene, std::shared_ptr<Player> player, std::shared_ptr<UFOControl> ufo);
	void Update(Number *elapsed);
	void Spawn(Vector3 position, float rotation);
	void Pause(bool paused);
	void Deactivate(void);
	bool PlayerNotClear(void);

	bool m_Hit;
	bool m_Done;
	int m_Points;
	int m_Size;

	SceneMesh *m_ShipMesh;

private:
	std::shared_ptr<CollisionScene> p_Scene;
	std::shared_ptr<Player> p_Player;
	std::shared_ptr<UFOControl> p_UFO;

	bool m_AimedShot;
	bool m_FromRight;
	bool m_ShieldHit;
	float m_Speed;

	void Enable(void);
	void AimAtTarget(void);
	void SetRotationPosition(void);
};

