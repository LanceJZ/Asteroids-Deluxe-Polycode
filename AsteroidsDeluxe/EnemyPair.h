#pragma once
#include "EnemyShip.h"

class EnemyPair : public EnemyShared
{
public:
	EnemyPair(void);

	void Setup(std::shared_ptr<Player> player, std::shared_ptr<UFOControl> ufo);
	void Setup(std::shared_ptr<CollisionScene> scene);
	void Update(Number *elapsed);
	void Spawn(Vector3 position, float rotation);
	void SetRotationPosition(void);
	void Pause(bool paused);
	void Deactivate(void);
	void Enable(void);
	bool CheckPlayerHit(void);
	bool CheckUFOHit(void);

	std::unique_ptr<EnemyShip> p_Ships[2];	
};

