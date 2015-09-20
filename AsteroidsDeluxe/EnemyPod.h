#pragma once
#include "EnemyPair.h"

class EnemyPod : public Location
{
public:
	EnemyPod(void);

	void Setup(std::shared_ptr<CollisionScene> scene);
	void Update(Number *elapsed);
	void Pause(bool paused);
	void NewWave(bool activated);
	void Spawn();
	void Deactivate(void);
	bool PlayerNotClear(void);

	bool m_Hit;
	bool m_Done;

private:
	std::shared_ptr<CollisionScene> p_Scene;

	bool m_ShieldHit;
	bool m_NewWave;
	float m_Speed;
	int m_Points;
	int m_Size;

	std::unique_ptr<EnemyPair> p_Pairs[3];

	void Enable(void);
	void SetPosition(void);
};

