#include "PhysicsSystem.h"

PhysicsSystem* PhysicsSystem::instance = 0;

PhysicsSystem::PhysicsSystem(void)	{

}

PhysicsSystem::~PhysicsSystem(void)	{

}

void	PhysicsSystem::Update(float msec) {	
	BroadPhaseCollisions();
	NarrowPhaseCollisions();

	for(vector<PhysicsNode*>::iterator i = allNodes.begin(); i != allNodes.end(); ++i) {
		(*i)->Update(msec);
	}
}

void	PhysicsSystem::BroadPhaseCollisions() {

}

void	PhysicsSystem::NarrowPhaseCollisions() {

}

void	PhysicsSystem::AddNode(PhysicsNode* n) {
	allNodes.push_back(n);
}

void	PhysicsSystem::RemoveNode(PhysicsNode* n) {
	for(vector<PhysicsNode*>::iterator i = allNodes.begin(); i != allNodes.end(); ++i) {
		if((*i) == n) {
			allNodes.erase(i);
			return;
		}
	}
}

bool PhysicsSystem::SphereSphereCollision(const CollisionSphere &s0, const CollisionSphere &s1, CollisionData *collisionData) const {
	return false;
}

bool PhysicsSystem::AABBCollision(const CollisionAABB &cube0, const CollisionAABB &cube1) const{
	return false;
}

bool PhysicsSystem::PointInConvexPolygon(const Vector3 testPosition, Vector3 * convexShapePoints, int numPointsL) const {
	return false;
}