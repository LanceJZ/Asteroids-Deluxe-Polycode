#include "Asteroids.h"
std::shared_ptr<Player> p_Player(new Player);
std::shared_ptr<RockControl> p_Rocks(new RockControl);
std::shared_ptr<UFOControl> p_UFOs(new UFOControl);
std::shared_ptr<EnemyController> p_Enemy(new EnemyController);

Asteroids::Asteroids(PolycodeView *view) : EventHandler()
{
	Random::Setup();
	// 	Core (int xRes, int yRes, bool fullScreen, bool vSync, int aaLevel, int anisotropyLevel, int frameRate, int monitorIndex)
	pCore = new POLYCODE_CORE(view, 800, 720, false, true, 0, 0, 240, 0, true);
	p_Scene = std::shared_ptr<CollisionScene>(new CollisionScene());

	m_Exit = false;
	m_Paused = false;
	m_FiredShot = false;
	m_Hyper = false;
	//  Core::resizeTo 	(int xRes, int yRes);	
	p_Scene->clearColor = Color(0.06, 0.02, 0.1, 0.05);
	p_Scene->useClearColor = true;

	p_Scene->getDefaultCamera()->setPosition(0, 0, -80);
	p_Scene->getDefaultCamera()->lookAt(Vector3(0, 0, 0));

	pCore->getInput()->addEventListener(this, InputEvent::EVENT_KEYDOWN);
	pCore->getInput()->addEventListener(this, InputEvent::EVENT_KEYUP);

	p_Player->Setup(p_Scene);
	p_UFOs->Setup(p_Scene, p_Player);
	p_Enemy->Setup(p_Scene, p_Player, p_UFOs);
	p_Rocks->Setup(p_Scene, p_Player, p_UFOs, p_Enemy);
}

Asteroids::~Asteroids()
{
	
}

void Asteroids::handleEvent(Event *event)
{
	if (event->getDispatcher() == pCore->getInput())
	{
		InputEvent *inputEvent = (InputEvent*)event;

		if (inputEvent->keyCode() == KEY_ESCAPE)
		{
			m_Exit = true;
		}

		if (event->getEventCode() == InputEvent::EVENT_KEYDOWN)
		{
			if (!p_Player->m_Active)
			{
				if (inputEvent->keyCode() == KEY_n || inputEvent->keyCode() == KEY_RETURN)
				{
					p_Player->NewGame();
					p_UFOs->NewGame();
					p_Rocks->NewGame();
					p_Enemy->NewGame();
				}

				if (p_Player->p_HUD->m_NewHighScore)
				{
					if (inputEvent->keyCode() == KEY_LCTRL || inputEvent->keyCode() == KEY_SPACE)
						p_Player->p_HUD->SelectNextLetter();

					if (inputEvent->keyCode() == KEY_RIGHT || inputEvent->keyCode() == KEY_d)
						p_Player->p_HUD->SelectLetterUp();

					if (inputEvent->keyCode() == KEY_LEFT || inputEvent->keyCode() == KEY_a)
						p_Player->p_HUD->SelectLetterDown();
				}
			}
			else
			{
				if (inputEvent->keyCode() == KEY_p)
				{
					if (m_Paused)
						m_Paused = false;
					else
						m_Paused = true;

					p_UFOs->Pause(m_Paused);
					p_Rocks->Pause(m_Paused);
					p_Player->Pause(m_Paused);
					p_Enemy->Pause(m_Paused);
				}
			}
		}
	}
}

void Asteroids::handlePlayerInput(void)
{
	//Main keys.
	bool key_thrust = pCore->getInput()->getKeyState(KEY_UP);
	bool key_left = pCore->getInput()->getKeyState(KEY_LEFT);
	bool key_right = pCore->getInput()->getKeyState(KEY_RIGHT);
	bool key_fire = pCore->getInput()->getKeyState(KEY_LCTRL);
	bool key_shield = pCore->getInput()->getKeyState(KEY_DOWN);
	//Alternative keys.
	bool key_thrustalt = pCore->getInput()->getKeyState(KEY_w);
	bool key_leftalt = pCore->getInput()->getKeyState(KEY_a);
	bool key_rightalt = pCore->getInput()->getKeyState(KEY_d);
	bool key_firealt = pCore->getInput()->getKeyState(KEY_SPACE);
	bool key_shieldalt = pCore->getInput()->getKeyState(KEY_s);

	if (key_thrust || key_thrustalt)
	{
		p_Player->ThrustOn();
	}
	else
	{
		p_Player->ThrustOff();
	}

	if (key_shieldalt || key_shield)
	{
		p_Player->ShieldOn();
	}
	else
	{
		p_Player->ShieldOff();
	}
	
	if (key_left || key_leftalt || key_right || key_rightalt)
	{
		if (key_left || key_leftalt)
			p_Player->TurnLeft();

		if (key_right || key_rightalt)
			p_Player->TurnRight();
	}
	else
	{
		p_Player->TurnOff();
	}

	if (key_fire || key_firealt)
	{
		if (!m_FiredShot)
		{
			p_Player->FireShot();
			m_FiredShot = true;
		}
	}
	else
	{
		m_FiredShot = false;
	}
}

bool Asteroids::Update()
{
	if (!m_Paused)
	{
		pCore->doSleep();

		Number frameelapsed = pCore->getElapsed();

		if (p_Player->m_Active)
		{
			p_Player->Update(&frameelapsed);
			handlePlayerInput();
		}
		else
		{
			p_Player->UpdateGameOver();
		}

		p_Player->UpdateShots(&frameelapsed);
		p_Rocks->Update(&frameelapsed);
		p_UFOs->Update(&frameelapsed);
		p_Enemy->Update(&frameelapsed);

		p_UFOs->p_UFO->EnemyPodActive(p_Enemy->p_Pod->m_Active);
		p_UFOs->p_UFO->EnemyPodLocation(p_Enemy->p_Pod->m_Position);
	}

	if (m_Exit)
	{
		return false;
	}

	return pCore->updateAndRender();
}