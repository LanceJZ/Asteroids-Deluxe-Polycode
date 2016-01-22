#pragma once
#include "Location.h"
#include "Random.h"
#include "Player.h"
#include "Shot.h"

class UFO :	public Location
{
public:
	UFO(void);
	
	void Setup(std::shared_ptr<CollisionScene> scene, std::shared_ptr<Player> player);
	void Update(Number *elapsed);
	void UpdateShot(Number *elapsed);
	void Spawn(int size);
	void Pause(bool paused);
	void Deactivate(void);
	bool PlayerNotClear(void);

	std::unique_ptr<Shot> p_Shot;
	SceneMesh *m_ShipMesh;

	bool m_Hit;
	bool m_Done;
	int m_Points;
	int m_Size;
	int m_NumberOfShots;
	int m_NumberOfShotsAtRocks;

private:
	std::shared_ptr<CollisionScene> p_Scene;
	std::shared_ptr<Player> p_Player;
	std::unique_ptr<Timer> p_FireTimer;
	std::unique_ptr<Timer> p_VectorTimer;
	std::unique_ptr<Sound> p_ShotSound;
	std::unique_ptr<Sound> p_ExplodeSound;
	std::unique_ptr<Sound> p_EngineLargeSound;
	std::unique_ptr<Sound> p_EngineSmallSound;

	bool m_FromRight;
	bool m_ShieldHit;
	bool m_ShotHitShield;
	float m_FireTimerAmount;
	float m_VectorTimerAmount;
	float m_Speed;

	void Enable(void);
	void ResetFireTimer(void);
	void ResetVectorTimer(void);
	void ChangeVector(void);
	void FireShotAt(float directionInRadians);
	void FireShot(void);
	void FireAimedShot(void);
	void FireRandomShot(void);
};

