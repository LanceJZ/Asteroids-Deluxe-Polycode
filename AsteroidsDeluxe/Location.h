#pragma once
#include <Polycode.h>
#include <PolycodeView.h>
#include <Polycode3DPhysics.h>

struct Rotate
{
	float Amount;
	float Velocity;
	float Acceleration;
};

class Location
{
public:
	Location();

	void Update(Number *elapsed);
	bool CirclesIntersect(Vector3 Target, float TargetRadius);
	bool m_Active;
	float m_Radius;
	Vector3 m_Position;
	Vector3 m_Velocity;
	Vector3 m_Acceleration;
	Rotate m_Rotation; //In Degrees

protected:
	int m_WindowHeight;
	int m_WindowWidth;

	void CheckForEdge(void);
};
