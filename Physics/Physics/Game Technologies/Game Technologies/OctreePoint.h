#ifndef OctreePoint_H
#define OctreePoint_H

#include "../../nclgl/Vector3.h"
#include "PhysicsNode.h"

/*
	Octree code supplied by Brandon Pelfrey, available at https://github.com/brandonpelfrey/SimpleOctree/
	NOT written by Dan Richardson 
*/


// Simple point data type to insert into the tree.
// Have something with more interesting behavior inherit
// from this in order to store other attributes in the tree.
class OctreePoint {
		PhysicsNode* node;
        Vector3 position; 
public:
        OctreePoint() { }
        OctreePoint(const Vector3& position) : position(position) { }
        inline const Vector3& getPosition() const { return position; }
        inline void setPosition(const Vector3& p) { position = p; }

		PhysicsNode* getNode() { return node;}
		void setNode(PhysicsNode* n){ node = n; setPosition(node->GetPosition()); }
};

#endif