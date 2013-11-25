#include "Island.h"
Mesh * Island :: sphere = NULL;

Island :: Island (void) {

	SceneNode * sunNode = new SceneNode (sphere, Vector4(1,1,0,1));
	sunNode -> SetModelScale (Vector3(10,15,5));
	sunNode -> SetTransform( Matrix4 :: Translation (Vector3 (0,100,0)));

	AddChild(sunNode);

	sunNode -> SetBoundingRadius (15.0f);
	
}

void Island :: Update ( float msec ) 
{
	transform = transform * Matrix4 :: Rotation ( msec / 10.0f, Vector3 (0 ,1 ,0));
	SceneNode :: Update ( msec );
}