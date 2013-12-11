#include "PhysicsSystem.h"

PhysicsSystem* PhysicsSystem::instance = 0;

PhysicsSystem::PhysicsSystem(void)	{

}

PhysicsSystem::~PhysicsSystem(void)	{

}

void	PhysicsSystem::Update(float msec, Vector3& OGGravity) {	
	BroadPhaseCollisions();
	NarrowPhaseCollisions();

	gravity = OGGravity;

	for(vector<PhysicsNode*>::iterator i = allNodes.begin(); i != allNodes.end(); ++i) {
		(*i)->Update(msec);
	}
}

void	PhysicsSystem::BroadPhaseCollisions() {
	//loop through all nodes, checking which collisions are possible
	Plane *p = new Plane(Vector3(0,-1,0),0);
	for(vector<PhysicsNode*>::iterator i = allNodes.begin(); i != allNodes.end(); i++){
		for(vector<PhysicsNode*>::iterator j = i+1; j != allNodes.end(); j++){
			
			//collision between a sphere and a plane
			if((*i)->GetCollisionType() == COLLISION_PLANE && (*j)->GetCollisionType() == COLLISION_SPHERE )
            {
				CollisionSphere ball_1((*j)->GetPosition(),(*j)->getSize());
				CollisionData thisCollision;
				
				if( p->SphereInPlane((*j)->GetPosition(),(*j)->getSize(), &thisCollision))
                {
					
					AddCollisionImpulse(*(*i), *(*j), thisCollision.m_point, thisCollision.m_normal, thisCollision.m_penetration);
					//(*j)->setForce(Vector3(0,5,0));
					//cout<< "Plane collision!" << endl;
                }
            }


			//collision between two spheres
			if((*i)->GetCollisionType() == COLLISION_SPHERE && (*j)->GetCollisionType() == COLLISION_SPHERE )
            {
				CollisionSphere ball_1((*i)->GetPosition(),(*i)->getSize());
                CollisionSphere ball_2((*j)->GetPosition(),(*j)->getSize());
				CollisionData thisCollision;
				if(SphereCollision(ball_1,ball_2, &thisCollision))
                {			
					(*i)->damage(); (*j)->damage();
					
					AddCollisionImpulse(*(*i), *(*j), thisCollision.m_point, thisCollision.m_normal, thisCollision.m_penetration);
					
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
			collisionData -> m_point = s0.m_pos - collisionData -> m_normal * ( s0.m_radius - collisionData->m_penetration *0.5f );
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

void PhysicsSystem::AddCollisionImpulse ( PhysicsNode& c0, PhysicsNode& c1, const Vector3 & hitPoint, const Vector3& normal, float penetration ) {
	// Some simple check code .
	float invMass0 = c0.GetInverseMass();//( c0.GetMass() > 1000.0f ) ? 0.0f : (1.0f / c0.GetMass() );
	float invMass1 = c1.GetInverseMass();//( c1.GetMass() >1000.0f ) ? 0.0f : (1.0f / c1.GetMass() );

	//invMass0 = (! c0.m_awake ) ? 0.0f : invMass0 ;
	//invMass1 = (! c1.m_awake ) ? 0.0f : invMass1 ;

	const Matrix4 worldInvInertia0 = c0.getInertia();
	const Matrix4 worldInvInertia1 = c1.getInertia();

	// Both objects are non movable
	if ( ( invMass0 + invMass1 )==0.0 ) return ;

	Vector3 r0 = hitPoint - c0.GetPosition();
	Vector3 r1 = hitPoint - c1.GetPosition();

	Vector3 v0 = c0.GetLinearVelocity() + Vector3::Cross( c0.GetAngularVelocity() , r0 );	Vector3 v1 = c1.GetLinearVelocity() + Vector3::Cross( c1.GetAngularVelocity() , r1 );

	// Relative Velocity
	Vector3 dv = v0 - v1 ;

	// If the objects are moving away from each other
	// we dont need to apply an impulse
	float relativeMovement = - Vector3::Dot( dv , normal );

	if ( relativeMovement < -0.01f )
		return ;
	

	// NORMAL Impulse
	{
	// Coefficient of Restitution
	float e = 0.5f ;

	float normDiv = Vector3::Dot(normal,normal)* (invMass0 + invMass1) + (Vector3::Dot(Vector3::Cross((worldInvInertia0 *(Vector3::Cross(r0,normal))),r0)+Vector3::Cross((worldInvInertia1 * (Vector3::Cross(r1,normal))),r1),normal));

	float jn = -1*(1+ e )* Vector3::Dot ( dv , normal ) / normDiv ;

	// Hack fix to stop sinking
	// bias impulse proportional to penetration distance
	jn = jn + ( penetration * 0.0003f );

	if(c0.GetCollisionType() != COLLISION_PLANE){
		c0.setLinearVelocity(c0.GetLinearVelocity() +  normal * (invMass0 * jn) );
		c0.setAngularVelocity(c0.GetAngularVelocity() +  ( worldInvInertia0 * Vector3::Cross (r0 , normal * jn) ));
		//c0.setAngularVelocity(Vector3(1,0,0));
	}
	if(c1.GetCollisionType() != COLLISION_PLANE){
		c1.setLinearVelocity(c1.GetLinearVelocity() - normal * (invMass1 * jn));
		c1.setAngularVelocity(c1.GetAngularVelocity() - (worldInvInertia1 *  Vector3::Cross (r1 , normal * jn)));
		
	}
	}

	// TANGENT Impulse Code
	{
	// Work out our tangent vector , with is perpendicular
	// to our collision normal
	Vector3 tangent = Vector3 (0 ,0 ,0);
	tangent = dv - (normal  * Vector3::Dot (dv , normal));
	tangent.Normalise();

	float tangDiv =  invMass0 + invMass1 + 
		Vector3::Dot ( tangent , Vector3::Cross (( c0.getInertia() * Vector3::Cross (r0 , tangent )  ), r0)
                                         + Vector3::Cross ((c1.getInertia()  *  Vector3::Cross (r1 , tangent )  ), r1) );

	float jt = -1 * Vector3::Dot(dv,tangent) / tangDiv ;
	// Clamp min /max tangental component

	// Apply contact impulse
	if(c0.GetCollisionType() != COLLISION_PLANE){
		
		c0.setLinearVelocity(c0.GetLinearVelocity() +  tangent * (invMass0 * jt));
		c0.setAngularVelocity(c0.GetAngularVelocity() + ( worldInvInertia0 * Vector3::Cross(r0, tangent * jt) ));
		//c0.setAngularVelocity(c0.GetAngularVelocity() + (Vector3::Cross(r0, tangent * jt)));
	}
	if(c1.GetCollisionType() != COLLISION_PLANE){
		
		c1.setLinearVelocity(c1.GetLinearVelocity() -  tangent * (invMass1 * jt));
		c1.setAngularVelocity(c1.GetAngularVelocity() - ( worldInvInertia1 * Vector3::Cross(r1, tangent * jt) ));
	}
	} // TANGENT
}