/******************************************************************************
Class:PhysicsNode
Implements:
Author:Rich Davison	<richard.davison4@newcastle.ac.uk> and YOU!
Description: This class represents the physical properties of your game's
entities - their position, orientation, mass, collision volume, and so on.
Most of the first few tutorials will be based around adding code to this class
in order to add correct physical integration of velocity / acceleration etc to
your game objects. 


In addition to the tutorial code, this class contains a pointer to a SceneNode.
This pointer is to the 'graphical' representation of your game object, just 
like the SceneNode's used in the graphics module. However, instead of 
calculating positions etc as part of the SceneNode, it should instead be up
to your 'physics' representation to determine - so we calculate a transform
matrix for your SceneNode here, and apply it to the pointer. 

Your SceneNode should still have an Update function, though, in order to
update any graphical effects applied to your object - anything that will end
up modifying a uniform in a shader should still be the job of the SceneNode. 

Note that since the SceneNode can still have children, we can represent our
entire CubeRobot with a single PhysicsNode, and a single SceneNode 'root'.

-_-_-_-_-_-_-_,------,   
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""   

*//////////////////////////////////////////////////////////////////////////////


#pragma once

#include "../../nclgl/Quaternion.h"
#include "../../nclgl/Vector3.h"
#include "../../nclgl/Matrix4.h"
#include "../../nclgl/SceneNode.h"


class PhysicsNode	{
public:
	PhysicsNode(void);
	PhysicsNode(Quaternion orientation, Vector3 position);
	~PhysicsNode(void);

	Vector3		GetPosition()			{ return m_position;}
	void		setPosition(Vector3 pos) { m_position = pos;}

	Vector3		GetLinearVelocity()		{ return m_linearVelocity;}
	void		setLinearVelocity(Vector3 vel) { m_linearVelocity = vel;}
	
	float		GetInverseMass()		{return m_invMass;}
	void		setInverseMass(float mass) { m_invMass = mass;}

	float		getAirResistance()		{return m_airResistance;}
	void		setAirResistance(float air) { m_airResistance = air;}

	Quaternion	GetOrientation()		{ return m_orientation;}
	void		setOrientation(Quaternion orient) { m_orientation = orient;}

	Vector3		GetAngularVelocity()	{ return m_angularVelocity;}
	void		setAngularVelocity(Vector3 angVel) { m_angularVelocity = angVel;}

	Vector3		getGravity() { return m_gravity; }
	void		setGravity(Vector3 accel) { m_gravity = accel; }

	Vector3		getForce() { return m_force; }
	void		setForce(Vector3 force) { m_force = force; }

	Matrix4		BuildTransform();

	virtual void		Update(float msec);

	void	SetTarget(SceneNode *s) { target = s;}

	void setIsCube(){ isCube = !isCube; }
	void setIsSphere(){ isSphere = !isSphere; }

	//Vector3	GetForce()	{ return m_force;}
	Vector3	GetTorque() { return m_torque;}
	void setTorque(Vector3 d, Vector3 f) { m_torque = Vector3::Cross(d, f); }

protected:

	bool isCube;
	bool isSphere;

	//<---------LINEAR-------------->
	Vector3		m_position;
	Vector3		m_linearVelocity;
	Vector3		m_force;
	float		m_invMass;

	Vector3		m_gravity;
	float		m_airResistance;

	//<----------ANGULAR--------------->
	Quaternion  m_orientation;
	Vector3		m_angularVelocity;
	Vector3		m_torque;
	Matrix4     m_invInertia;

	SceneNode*	target;  
};
