# pragma once
#include "../../nclgl/SceneNode.h"
#include "..\..\nclgl\OBJMesh.h"

class World : public SceneNode {
public :
	World ( void );
	~ World ( void ){};
	virtual void Update ( float msec );
	static void CreateSun (string filename) {
		OBJMesh *m = new OBJMesh ();
		m-> LoadOBJMesh(MESHDIR+filename);
		sun = m;
	}
	static void DeleteSun (){ delete sun ;}

	static void CreateTree (string filename) {
		OBJMesh *m = new OBJMesh ();
		m-> LoadOBJMesh(MESHDIR+filename);
		tree = m;
	}
	static void DeleteTree (){ delete tree ;}
protected :
	static Mesh * sun ;
	static Mesh * tree ;
	SceneNode * sunNode ;
	SceneNode * treeNode ;
	SceneNode * character ;
};