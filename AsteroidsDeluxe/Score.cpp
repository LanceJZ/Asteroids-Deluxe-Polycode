#include "Score.h"

Score::Score()
{
	CoreServices::getInstance()->getResourceManager()->addArchive("Resources/default.pak");
	CoreServices::getInstance()->getResourceManager()->addDirResource("default", false);
	NewGame();
	Scene *scene = new Scene(Scene::SCENE_2D);
	scene->getActiveCamera()->setOrthoSize(1200, 800);
	m_Scene = scene;
	m_SceneLabel = new SceneLabel("Score:", 22);
	m_Scene->addChild(m_SceneLabel);
	m_SceneLabel->setColor(1.0, 1.0, 1.0, 0.8);
	Update();
}

void Score::Update(void)
{
	String display = "Score: " + std::to_string(m_Score) + " Lives: " + std::to_string(m_Lives);
	int size = display.length() + 1;
	m_SceneLabel->setPosition(Vector3(-600 + (size * 10), 350, 0));
	m_SceneLabel->setText(display);
}

void Score::Setup(Scene *scene)
{
}

void Score::Add(int score)
{
	m_Score += score;

	if (m_Score > 10000)
		m_Lives++;

	Update();
}

void Score::NewGame(void)
{
	m_Lives = 4;
	m_Score = 0;
}

void Score::LostLife(void)
{
	m_Lives--;

	if (m_Lives < 1)
		m_Lives = 0;

	Update();
}

int Score::Lives(void)
{
	return m_Lives;
}
