#include "Island.h"
Mesh * Island :: sphere = NULL;
float initTime = 2.0f;

Island :: Island (void) {

	ball = new SceneNode (sphere, Vector4(1,1,0,1));
	ball -> SetModelScale (Vector3(70,70,70));
	ball -> SetTransform( Matrix4 :: Translation (Vector3 (1000,-1000,1000)));

	ball -> SetBoundingRadius (120.0f);
	AddChild(ball);	
	
}


void Island::Update(float msec)
{
    initTime  += msec * 0.001;
   
       
    float x=   1800 + cos(initTime* 1.288/10) * 2400 ;
    float y = 900 + sin (initTime* 1.288/10) * 2400;
    transform =  Matrix4::Translation(Vector3(0,x,y));
    
    SceneNode::Update(msec);
}