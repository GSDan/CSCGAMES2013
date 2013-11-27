#include "PhysicsNode.h"

PhysicsNode::PhysicsNode(void)	{
	target = NULL;
}

PhysicsNode::PhysicsNode(Quaternion orientation, Vector3 position) {
	m_orientation	= orientation;
	m_position		= position;
}

PhysicsNode::~PhysicsNode(void)	{

}

//You will perform your per-object physics integration, here!
//I've added in a bit that will set the transform of the
//graphical representation of this object, too.
void	PhysicsNode::Update(float msec) {
	
	//semi-implicit Euler Integration
	//next velocity = this velocity + this acceleration * change in time
	//next position = this position + next velocity * change in time
	Vector3 finalAcceleration = m_acceleration;

	//apply air resistance, positive or negative depending on the current direction
	(finalAcceleration.x > 0)?(finalAcceleration.x -= m_airResistance):(finalAcceleration.x += m_airResistance);

	m_linearVelocity = m_linearVelocity + m_acceleration * msec;
	m_position = m_position + m_linearVelocity * msec;

	if(target) {
		target->SetTransform(BuildTransform());
	}
}

/*
This function simply turns the orientation and position
of our physics node into a transformation matrix, suitable
for plugging into our Renderer!

It is cleaner to work with matrices when it comes to rendering,
as it is what shaders expect, and allow us to keep all of our
transforms together in a single construct. But when it comes to
physics processing and 'game-side' logic, it is much neater to
have seperate orientations and positions.

*/
Matrix4		PhysicsNode::BuildTransform() {
	Matrix4 m = m_orientation.ToMatrix();

	m.SetPositionVector(m_position);

	return m;
}