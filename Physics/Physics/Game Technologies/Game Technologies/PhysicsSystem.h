/******************************************************************************
Class:PhysicsSystem
Implements:
Author:Rich Davison	<richard.davison4@newcastle.ac.uk> and YOU!
Description: A very simple physics engine class, within which to implement the
material introduced in the Game Technologies module. This is just a rough 
skeleton of how the material could be integrated into the existing codebase -
it is still entirely up to you how the specifics should work. Now C++ and
graphics are out of the way, you should be starting to get a feel for class
structures, and how to communicate data between systems.

It is worth poinitng out that the PhysicsSystem is constructed and destructed
manually using static functions. Why? Well, we probably only want a single
physics system to control the entire state of our game world, so why allow 
multiple systems to be made? So instead, the constructor / destructor are 
hidden, and we 'get' a single instance of a physics system with a getter.
This is known as a 'singleton' design pattern, and some developers don't like 
it - but for systems that it doesn't really make sense to have multiples of, 
it is fine!

-_-_-_-_-_-_-_,------,   
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""   

*//////////////////////////////////////////////////////////////////////////////


#pragma once

#include "PhysicsNode.h"
#include <vector>

using std::vector;

/*
Rich: 
There are a few ways of integrating the collision volumes
discussed in this module into your little physics engine.
You could keep pointers to all types inside a physics node,
and check to see which ones are NULL. Or you could make all
collision volumes inherit from a base class, so we only need
a single pointer inside each Physics Node. We can then either
use a process of dynamic casting to determine which exact type
of volume the pointer is, or just cheat and use an enum member
variable (I do this in my own work, you can do whichever you
feel comfortable with!).

enum CollisionVolumeType {
	COLLISION_SPHERE,
	COLLISION_AABB,
	COLLISION_PLANE
}


class CollisionVolume {
public:
	CollisionVolumeType GetType() { return type;}


protected:
	CollisionVolumeType type;
};

*/

//
class CollisionSphere {

};

class CollisionAABB {

};

class CollisionData {

};


class PhysicsSystem	{
public:
	friend class GameClass;

	void		Update(float msec);

	void		BroadPhaseCollisions();
	void		NarrowPhaseCollisions();

	//Statics
	static void Initialise() {
		instance = new PhysicsSystem();
	}

	static void Destroy() {
		delete instance;
	}

	static PhysicsSystem& GetPhysicsSystem() {
		return *instance;
	}

	void	AddNode(PhysicsNode* n);

	void	RemoveNode(PhysicsNode* n);

protected:
	PhysicsSystem(void);
	~PhysicsSystem(void);

	bool SphereSphereCollision(const CollisionSphere &s0, const CollisionSphere &s1, CollisionData *collisionData = NULL) const;
	bool AABBCollision(const CollisionAABB &cube0, const CollisionAABB &cube1) const;

	//bool SphereAABBCollision(const CollisionSphere &sphere, const CollisionAABB &cube, CollisionData *collisionData = NULL) const; //Research!!!! :-)


	//Sphere plane collisions we started in the previous module, and expand upon via the Plane class..

	bool PointInConvexPolygon(const Vector3 testPosition, Vector3 * convexShapePoints, int numPointsL) const;



//Statics
	static PhysicsSystem* instance;

	vector<PhysicsNode*> allNodes;
};

