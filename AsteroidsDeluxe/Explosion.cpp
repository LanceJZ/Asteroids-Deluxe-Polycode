#include "Explosion.h"


Explosion::Explosion()
{
	for (int i = 0; i < 12; i++)
		pDots[i] = std::unique_ptr<Dot>(new Dot());
}

void Explosion::Setup(std::shared_ptr<CollisionScene> scene)
{
	for (int i = 0; i < 12; i++)
		pDots[i]->Setup(scene);

	m_Active = false;
}

void Explosion::Update(Number * elapsed)
{
	bool active = false;

	for (int i = 0; i < 12; i++)
	{
		if (pDots[i]->m_Active)
		{
			pDots[i]->Update(elapsed);
			active = true;
		}
	}

	if (!active)
		m_Active = false;
}

void Explosion::Activate(Vector3 position, float size)
{
	for (int i = 0; i < 12; i++)
		pDots[i]->Activate(position, size);

	m_Active = true;
}

void Explosion::Deactivate(void)
{
	for (int i = 0; i < 12; i++)
		pDots[i]->Deactivate();

	m_Active = false;
}
