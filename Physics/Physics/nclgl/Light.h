#pragma once

#include "Vector4.h"
#include "Vector3.h"

class Light{
public:
	Light(Vector3 position, Vector4 colour, float radius) {
		this->position = position;
		this->colour = colour;
		this->radius = radius;
	}
	
	~Light(void){};
	
	Vector3 GetPosition() const {return position;}
	float GetRadius() const {return radius;}
	Vector4 GetColour() const {return colour;}

	void SetRadius(float val) {radius = val;}
	void SetColour(Vector4 val) {colour = val;}
	void SetPosition(Vector3 val) {position = val;}

protected:
	
	Vector3 position;
	Vector4 colour;
	float radius;
};