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
	void Spawn(void);
	void Deactivate(void);
	bool PlayerNotClear(void);
	bool CheckPlayerHit(void);
	bool CheckUFOHit(void);

	std::unique_ptr<EnemyPair> p_Pairs[3];

	bool m_Hit;
	bool m_Done;

private:
	bool m_NewWave;
	float m_Speed;
	int m_Size;

	void Enable(void);
	void SetPosition(void);
};

