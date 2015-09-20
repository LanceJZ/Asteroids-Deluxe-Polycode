#pragma once
#include "EnemyShip.h"

class EnemyPair : public Location, PlayerChase
{
public:
	EnemyPair(void);

	void Setup(std::shared_ptr<Player> player, std::shared_ptr<UFOControl> ufo);
	void Setup(std::shared_ptr<CollisionScene> scene);
	void Update(Number *elapsed);
	void Spawn(Vector3 position, float rotation);
	void SetRotationPosition(void);
	void Pause(bool paused);
	void NewWave(bool activated);
	void Deactivate(void);
	void Enable(void);
	bool PlayerNotClear(void);

	std::unique_ptr<EnemyShip> p_Ships[2];

	bool m_Hit;
	bool m_Done;
	bool m_InPod;
	
private:
	bool m_ShieldHit;
	bool m_NewWave;
	float m_Speed;
	int m_Points;

	std::shared_ptr<CollisionScene> p_Scene;
	std::shared_ptr<Player> p_Player;
	std::shared_ptr<UFOControl> p_UFO;

	void CheckPlayerHit(void);

};

