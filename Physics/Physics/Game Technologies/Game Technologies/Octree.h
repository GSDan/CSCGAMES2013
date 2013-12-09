#include "PhysicsNode.h"

class Octree {
public:
	Octree(vector<PhysicsNode> alloc, float size){

		//allocate boundaries
		boundaryXR = size;
		boundaryXL = -size;
		boundaryYT = size;
		boundaryYB = -size;
		boundaryZF = size;
		boundaryZB = -size;

	};

	void createChildren();

protected:
	vector<PhysicsNode> elements;
	float boundaryXL; //left
	float boundaryXR; //right
	float boundaryYT; //top
	float boundaryYB; //bottom
	float boundaryZF; //front
	float boundaryZB; //back

};