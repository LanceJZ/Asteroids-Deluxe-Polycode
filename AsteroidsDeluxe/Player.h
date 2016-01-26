#pragma once
#include "Location.h"
#include "Random.h"
#include "Shot.h"
#include "HUD.h"

class Player : public Location
{
public:
	Player(void);

	void Setup(std::shared_ptr<CollisionScene> scene);
	void Update(Number *elapsed);
	void UpdateShots(Number *elapsed);
	void GotPoints(int points);
	void DeactivateShot(int shot);
	void Pause(bool paused);
	void ShieldHit(Vector3 velocity, bool shot); //velocity of object that hit shield.
	void UpdateGameOver(void);
	void TurnLeft(void);
	void TurnRight(void);
	void TurnOff(void);
	void ThrustOn(void);
	void ThrustOff(void);
	void ShieldOn(void);
	void ShieldOff(void);
	void FireShot(void);
	void Hit(void);
	void UpdateLivesDisplay(void);
	void SetClear(void);
	void Activate(void);
	void Deactivate(void);
	void NewGame(void);
	void Reset(void);
	
	bool m_Hit;
	bool m_Spawn;
	bool m_GameOver;
	bool m_ShieldOn;

	std::unique_ptr<Shot> p_Shots[4];
	std::unique_ptr<HUD> p_HUD;
	SceneMesh *m_ShipMesh;
	SceneMesh *m_ShieldMesh;

private:
	bool m_ShipTurning;
	bool m_ClearToSpawn;
	bool m_ThrustOn;
	bool m_ThrustFlameOn;
	int m_GameOverDisplay;
	float m_ExplodeTimerAmount;
	float m_ClearTimerAmount;
	float m_GameOverTimerAmount;
	float m_FlameTimerAmount;
	float m_ShieldPower;

	const Color m_MeshColor = Color(0.7, 0.7, 1.0, 1.0);

	std::unique_ptr<Timer> p_ExplodeTimer;
	std::unique_ptr<Timer> p_ClearTimer;
	std::unique_ptr<Timer> p_GameOverTimer;
	std::unique_ptr<Timer> p_FlameTimer;

	std::shared_ptr<CollisionScene> p_Scene;
	std::unique_ptr<Sound> p_ShotSound;
	std::unique_ptr<Sound> p_ExplodeSound;
	std::unique_ptr<Sound> p_ThrustSound;
	std::unique_ptr<Sound> p_SpawnSound;
	std::unique_ptr<Sound> p_ShieldSound;
	std::unique_ptr<Location> p_ExpLoc[8];
	std::vector<Entity*> p_ShipLives;

	SceneMesh *m_ShipExplosionMesh[8];
	SceneMesh *m_ShipWingMesh;
	SceneMesh *m_FlameMesh;

	void ApplyThrust(void);
	void StartExplode(void);
	void UpdateExplode(Number *elapsed);
	void SetRotationPosition(void);
	void ResetExplodeTimer(void);
	void ResetClearTimer(void);
	void ResetGameOverTimer(void);
	void ResetFlameTimer(void);
	bool CheckClear(void);
};
