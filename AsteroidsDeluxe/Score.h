#pragma once
#include <Polycode.h>
#include <PolycodeView.h>

class Score
{
public:
	Score();

	void Update(void);
	void Setup(Scene *scene);
	void Add(int score);
	void NewGame(void);
	void LostLife(void);
	int Lives(void);

private:
	unsigned int m_Score;
	int m_Lives;

	Scene *m_Scene;
	SceneLabel *m_SceneLabel;
};

