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
	//loop through all nodes, checking which collisions are possible

	for(vector<PhysicsNode*>::iterator i = allNodes.begin(); i != allNodes.end(); i++){
		for(vector<PhysicsNode*>::iterator j = i+1; j != allNodes.end(); j++){

			//collision between a sphere and a plane
			if((*i)->GetCollisionType() == COLLISION_SPHERE && (*j)->GetCollisionType() == COLLISION_PLANE )
            {
				CollisionSphere ball_1((*i)->GetPosition(),60.0f);
              

				Plane *p = new Plane(Vector3(0,-1,0),0);
                if( p->SphereInPlane((*i)->GetPosition(),60.0f))
                {
                    cout<<"COLLISION PLANE"<<endl;
                }
            }


			//collision between two spheres
			if((*i)->GetCollisionType() == COLLISION_SPHERE && (*j)->GetCollisionType() == COLLISION_SPHERE )
            {
				CollisionSphere ball_1((*i)->GetPosition(),10.0f);
                CollisionSphere ball_2((*j)->GetPosition(),10.0f);
				CollisionData thisCollision;
				if(SphereCollision(ball_1,ball_2, &thisCollision))
                {
					Vector3 relativeVelocity = (*i)->GetLinearVelocity() - (*j)->GetLinearVelocity();
					float compRelVelocity = Vector3::Dot(((*i)->GetLinearVelocity() - (*j)->GetLinearVelocity())*0.5, thisCollision.m_normal);
					//0.5 = placeholder elasticity (1 = full, 0 = none)
					Vector3 momentumLHS = (*i)->GetLinearVelocity() * (*i)->GetInverseMass() + (thisCollision.m_normal * compRelVelocity);
					Vector3 momentumRHS = (*i)->GetLinearVelocity() * (*i)->GetInverseMass() - (thisCollision.m_normal * compRelVelocity);

					float impulse = Vector3::Dot(((*i)->GetLinearVelocity() + (*j)->GetLinearVelocity()) * -(1 + 0.5),  thisCollision.m_normal)  /
									Vector3::Dot(thisCollision.m_normal, thisCollision.m_normal * (1/(*i)->GetInverseMass() + 1/(*j)->GetInverseMass()));

					(*i)->setLinearVelocity((*i)->GetLinearVelocity() + (thisCollision.m_normal * (impulse/(*i)->GetInverseMass()) ));
					(*j)->setLinearVelocity((*j)->GetLinearVelocity() + (thisCollision.m_normal * (impulse/(*j)->GetInverseMass()) ));
                }
            }
		}
	}

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

bool PhysicsSystem::SphereCollision(const CollisionSphere &s0, const CollisionSphere &s1, CollisionData *collisionData) const {
	const float distSq = ( s0.m_pos - s1.m_pos).LengthSq();

	const float sumRadius = ( s0.m_radius + s1.m_radius );

	//if the distance between the spheres is less than the sum of the objects' radii
	if ( distSq < sumRadius * sumRadius ){
		if ( collisionData ){
			collisionData -> m_penetration = sumRadius - sqrtf( distSq );
			collisionData -> m_normal = ( s0.m_pos - s1.m_pos );
			collisionData -> m_normal.Normalise();
			collisionData -> m_point = s0 . m_pos - collisionData -> m_normal * ( s0 . m_radius - collisionData -> m_penetration *0.5f );
		}
		return true ; // Collision
	}
	return false ; // No Collision
}

bool PhysicsSystem::AABBCollision(const CollisionAABB &cube0, const CollisionAABB &cube1) const{
	return false;
}

bool PhysicsSystem::PointInConvexPolygon(const Vector3 testPosition, Vector3 * convexShapePoints, int numPointsL) const {
	return false;
}