#pragma once
#include "EnemyShared.h"

class EnemyShip : public EnemyShared
{
public:
	EnemyShip(void);

	void Setup(std::shared_ptr<Player> player, std::shared_ptr<UFOControl> ufo);
	void Setup(std::shared_ptr<CollisionScene> scene);
	void Update(Number *elapsed);
	void Spawn(Vector3 position, float rotation);
	void SetRotationPosition(void);
	void Pause(bool paused);
	void Enable(void);
	void Deactivate(void);
	bool CheckPlayerHit(void);
	bool CheckUFOHit(void);

	SceneMesh *m_ShipMesh;

private:
	std::shared_ptr<CollisionScene> p_Scene;

	bool m_ShieldHit;
};

