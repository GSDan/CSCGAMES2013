# pragma once
#include "../../nclgl/SceneNode.h"
#include "..\..\nclgl\OBJMesh.h"
#include "../../nclgl/Light.h"

class Island : public SceneNode {
public :
	Island ( void );
	~ Island ( void ){};
	virtual void Update ( Vector3 vec, float msec);
	static void CreateSphere () {
		OBJMesh *m = new OBJMesh ();
		m-> LoadOBJMesh(MESHDIR"sphere.obj");
		sphere = m;
	}
	SceneNode* ball; 
	static void DeleteSphere (){ delete sphere ;}
protected :
	static Mesh * sphere ;
	

};