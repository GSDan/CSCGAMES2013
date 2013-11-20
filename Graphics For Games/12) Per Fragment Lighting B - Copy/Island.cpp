#include "Island.h"

Mesh * Island :: sphere = NULL;
float initTime = 2.0f;

Island :: Island (void) {

	ball = new SceneNode (sphere, Vector4(1,1,0.6f,1));
	ball -> SetModelScale (Vector3(50,50,50));
	ball -> SetTransform( Matrix4 :: Translation (Vector3 (0,0,0)));

	ball -> SetBoundingRadius (120.0f);
	AddChild(ball);	
	
}


void Island::Update(Vector3 vec, float msec)
{
        
    transform =  Matrix4::Translation(vec);
    SceneNode::Update(msec);
}