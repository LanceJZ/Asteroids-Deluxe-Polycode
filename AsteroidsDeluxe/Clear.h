#pragma once
#include "Location.h"

class Clear :
	public Location
{
public:
	Clear();

	void Update(Number *elapsed);
	void Deactivate(void);
};

