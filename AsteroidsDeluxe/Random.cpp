#include "Random.h"
//Random Number Generator
static std::mt19937 m_Random;

void Random::Setup(void)
{
	// Initiate Random Number Generator
	m_Random.seed(unsigned long(time(0)));
}

float Random::Number(float Min, float Max)
{
	int iMin = int(Min * 100000.0);
	int iMax = int(Max * 100000.0);

	std::uniform_int_distribution<uint32_t> roll(iMin, iMax);
	return float(roll(m_Random) * 0.00001);
}

float Random::Clip(float number, float lower, float upper)
{
	return std::max(lower, std::min(number, upper));
}