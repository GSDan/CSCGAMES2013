#pragma once

#include "Vector3.h"

class CollisionData{
public :
    Vector3 m_point ;
    Vector3 m_normal ;
    float m_penetration ;
};

class Plane {
public:
	Plane(void){};
	Plane(const Vector3 &normal, float distance, bool normalise = false);
	~Plane(void){};

	void SetNormal(const Vector3 &normal) {this->normal = normal;}
	Vector3 GetNormal() const {return normal;}

	void SetDistance(float dist) {distance = dist;}
	float GetDistance() const {return distance;}

	//bool SphereInPlane(const Vector3 &position, float radius) const;
	bool SphereInPlane ( const Vector3 & position , float radius , CollisionData * collisionData = NULL ) const;

protected:
	Vector3 normal;
	float distance;
};