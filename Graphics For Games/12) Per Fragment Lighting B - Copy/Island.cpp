#include "Island.h"
Mesh * Island :: sphere = NULL;

Island :: Island (void) {

		ball = new SceneNode (sphere, Vector4(1,1,0,1));
	ball -> SetModelScale (Vector3(70,70,70));
	ball -> SetTransform( Matrix4 :: Translation (Vector3 (4000,100,40)));

	ball -> SetBoundingRadius (150.0f);
	AddChild(ball);	
	
}



void Island :: Update ( float msec ) 
{
	transform = transform * Matrix4 :: Rotation ( msec / 1000.0f, Vector3 (1 ,0 ,1));

	cout<<ball->GetTransform().GetPositionVector()<<endl;
	

	
	//cout<<transform<<endl;
	SceneNode :: Update ( msec );
}