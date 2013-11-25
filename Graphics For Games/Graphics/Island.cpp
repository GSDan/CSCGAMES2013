#include "Island.h"

Mesh * Island :: sphere = NULL;
MD5FileData* Island :: hellData = NULL;

float initTime = 2.0f;

Island :: Island (void) {

	ball = new SceneNode (sphere, Vector4(1,1,0.6f,1));
	ball -> SetModelScale (Vector3(50,50,50));
	ball -> SetTransform( Matrix4 :: Translation (Vector3 (0,0,0)));
	ball -> SetBoundingRadius (120.0f);
	AddChild(ball);	

	hellNode = new MD5Node(*hellData);
	hellNode->PlayAnim("../../Meshes/idle2.md5anim");
	hellNode-> SetTransform( Matrix4 :: Translation (Vector3 (1600,450,1800)));
	AddChild(hellNode);
}


void Island::Update(Vector3 sunvec, float msec)
{	
	ball->SetTransform(Matrix4::Translation(sunvec));
    SceneNode::Update(msec);
}