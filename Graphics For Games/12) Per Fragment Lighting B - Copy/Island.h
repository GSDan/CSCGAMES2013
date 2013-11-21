# pragma once
#include "../../nclgl/SceneNode.h"
#include "..\..\nclgl\OBJMesh.h"
#include"../../nclgl/MD5Mesh.h"
#include"../../nclgl/MD5Node.h"
#include "../../nclgl/Light.h"


class Island : public SceneNode {
protected :
	static Mesh * sphere ;
	static MD5FileData*	hellData;
	

public :
	Island ( void );
	~ Island ( void ){};
	virtual void Update ( Vector3 vec, float msec);
	static void CreateSphere () {
		OBJMesh *m = new OBJMesh ();
		m-> LoadOBJMesh(MESHDIR"sphere.obj");
		sphere = m;
	}
	static void CreateHell(){
		hellData		= new MD5FileData("../../Meshes/hellknight.md5mesh");
		hellData->AddAnim("../../Meshes/idle2.md5anim");		
	}
	
	SceneNode* ball; 
	MD5Node*   hellNode;
	
	static void DeleteSphere (){ delete sphere ;}
	static void DeleteHell (){ delete hellData ;}


};