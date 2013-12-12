#include "PhysicsSystem.h"

PhysicsSystem* PhysicsSystem::instance = 0;

PhysicsSystem::PhysicsSystem(void)	{
	//aiInNeed = NULL;
}

PhysicsSystem::~PhysicsSystem(void)	{

}

void	PhysicsSystem::Update(float msec, Vector3& OGGravity) {	

	//START OCTREE SETUP
	
	//set octree boundaries
	octree = new Octree(Vector3(0,0,0), Vector3(10000,10000,10000));

	//insert nodes into octree
	for(vector<PhysicsNode*>::iterator i = allNodes.begin(); i != allNodes.end(); ++i) {
		if((*i)->GetCollisionType() == COLLISION_SPHERE || (*i)->GetCollisionType() == PLANE_ONLY_SPHERE)		{
			OctreePoint* tempPoint = new OctreePoint((*i)->GetPosition());
			tempPoint->setNode((*i));
			octree->insert(tempPoint);
		}
    }

	//END OCTREE SETUP

	BroadPhaseCollisions();

	gravity = OGGravity;

	for(vector<PhysicsNode*>::iterator i = allNodes.begin(); i != allNodes.end(); ++i) {
		(*i)->Update(msec);
	}

	/*if(aiInNeed != NULL){
		getAITarget(aiInNeed);
	}*/

	delete octree;
}

void	PhysicsSystem::BroadPhaseCollisions() {	
	//ground plane
	Plane *p = new Plane(Vector3(0,-1,0),0);

	for(vector<PhysicsNode*>::iterator i = allNodes.begin(); i != allNodes.end(); i++){		
		
		//radius around node to check
		int dist = (*i)->getSize() * 1.5;

		Vector3 qmin((*i)->GetPosition().x-dist,(*i)->GetPosition().y-dist,(*i)->GetPosition().z-dist);
        Vector3 qmax((*i)->GetPosition().x+dist,(*i)->GetPosition().y+dist,(*i)->GetPosition().z+dist);

		//make a vector of all nodes in that area
		vector<OctreePoint*>* results = new vector<OctreePoint*>;
		octree->getPointsInsideBox(qmin, qmax, (*results));	
		
		if(results->size() > 0)
			//do narrow phase on the resulting nodes		
			NarrowPhaseCollisions(results, p);

	}
}

void	PhysicsSystem::NarrowPhaseCollisions(vector<OctreePoint*>* results, Plane* p) {
	//loop through all nodes, checking which collisions are possible
	
	for(vector<OctreePoint*>::iterator i = results->begin(); i != results->end(); i++){

		if((*i)->getNode()->GetCollisionType() == COLLISION_SPHERE){
			for(vector<OctreePoint*>::iterator j = i+1; j != results->end(); j++){   
				//collision between two spheres
				if((*i)->getNode()->GetCollisionType() == COLLISION_SPHERE && (*j)->getNode()->GetCollisionType() == COLLISION_SPHERE )
				{
					CollisionSphere ball_1((*i)->getNode()->GetPosition(),(*i)->getNode()->getSize());
					CollisionSphere ball_2((*j)->getNode()->GetPosition(),(*j)->getNode()->getSize());
					CollisionData thisCollision;
					if(SphereCollision(ball_1,ball_2, &thisCollision))
					{			
						(*i)->getNode()->damage(); (*j)->getNode()->damage();
					
						AddCollisionImpulse(*(*i)->getNode(), *(*j)->getNode(), thisCollision.m_point, thisCollision.m_normal, thisCollision.m_penetration, false);
						if(!(*i)->getNode()->hasGravity())
							(*i)->getNode()->setGravity(gravity);
						if(!(*j)->getNode()->hasGravity())
							(*j)->getNode()->setGravity(gravity);
					}
				}
			}
		}

		//collision between a sphere and a plane
			
		CollisionSphere ball_1((*i)->getNode()->GetPosition(),(*i)->getNode()->getSize());
		CollisionData thisCollision;
		
		if( p->SphereInPlane((*i)->getNode()->GetPosition(),(*i)->getNode()->getSize(), &thisCollision)){

			AddCollisionImpulse((*allNodes[0]), *(*i)->getNode(), thisCollision.m_point, thisCollision.m_normal, thisCollision.m_penetration, true);
        }

	}
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

void PhysicsSystem::AddCollisionImpulse ( PhysicsNode& c0, PhysicsNode& c1, const Vector3 & hitPoint, const Vector3& normal, float penetration, bool plane0 ) {
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
	
	
	
	if(plane0){
		//we know this is a plane->sphere collision
		if(c1.getSize() == 128)
			jn = jn + ( penetration * 0.0000375f );
		else if(c1.getSize() == 64)
			jn = jn + ( penetration * 0.000075f );
		else if(c1.getSize() == 32)
			jn = jn + ( penetration * 0.00015f );
		else if(c1.getSize() == 16)
			jn = jn + ( penetration * 0.0003f );
		else if(c1.getSize() == 8)
			jn = jn + ( penetration * 0.0008f );
		else if(c1.getSize() == 4)
			jn = jn + ( penetration * 0.0016f );
		else if(c1.getSize() == 2)
			jn = jn + ( penetration * 0.0033f );
	}


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

/*void PhysicsSystem::getAITarget(PhysicsNode* ai){
	//pull nearest node from octree which != this ai
	//navigate above the node
	//if in position, attack

	//radius around node to check
	int dist = 500;

	Vector3 qmin(ai->GetPosition().x-dist,ai->GetPosition().y-dist,ai->GetPosition().z-dist);
    Vector3 qmax(ai->GetPosition().x+dist,ai->GetPosition().y+dist,ai->GetPosition().z+dist);

	//make a vector of all nodes in that area
	vector<OctreePoint*>* results = new vector<OctreePoint*>;
	octree->getPointsInsideBox(qmin, qmax, (*results));	

	bool targetFound = false;

	for(vector<OctreePoint*>::iterator i = results->begin(); i != results->end(); i++){
		//if this entity is not the AI and is a collision sphere, set target!
		if((*i)->getNode() != ai && (*i)->getNode()->GetCollisionType() == COLLISION_SPHERE){	
			ai->target = (*i)->getNode();
			break;
		}
	}

	ai = NULL;
}
*/
