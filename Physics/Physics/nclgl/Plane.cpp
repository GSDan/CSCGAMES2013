#include "Plane.h"

Plane::Plane(const Vector3 &normal, float distance, bool normalise) {
	if(normalise) {
		float length = sqrt(Vector3::Dot(normal, normal));

		this->normal = normal / length;
		this->distance = distance / length;
	}
	else {
		this->normal = normal;
		this->distance = distance;
	}
}

/*bool Plane::SphereInPlane(const Vector3 &position, float radius) const {
	if(Vector3::Dot(position, normal) + distance <= -radius)
		return false;
	else return true;
}*/

bool Plane :: SphereInPlane ( const Vector3 & position , float radius , CollisionData * collisionData) const
{
	float separation = Vector3 :: Dot ( position , normal ) + distance ;

	if ( separation <= - radius ){
		return false ;
	}

	separation = -separation;

	if ( collisionData ){
		collisionData -> m_penetration = radius - separation ;
		collisionData -> m_normal = normal ;
		collisionData -> m_point = position - normal * separation ;
	}
	return true ;
}