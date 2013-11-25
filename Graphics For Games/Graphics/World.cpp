#include "World.h"
Mesh * World :: sun = NULL ;
Mesh * World :: tree = NULL ;

World :: World ( void ) {
	// Optional , uncomment if you want a local origin marker !
	// SetMesh ( cube );
	CreateSun("sphere.obj");
	//CreateTree("Tree.obj");
	SceneNode * island = new SceneNode (); // Red !
	AddChild(island);
	

	sunNode = new SceneNode(sun, Vector4(1,1,1,1));
	
	sunNode -> SetModelScale ( Vector3 (15 ,15 ,15));
	sunNode -> SetTransform ( Matrix4 :: Translation ( Vector3 (0 ,60 ,0)));	island -> AddChild(sunNode);
	/*treeNode = new SceneNode(tree, Vector4(0,0,1,1));
	treeNode -> SetModelScale(Vector3(10,10,10));
	treeNode -> SetTransform(Matrix4::Translation(Vector3(10,10,10)));
	island -> AddChild(treeNode);*/

	island -> SetBoundingRadius(20.0f);
	sunNode -> SetBoundingRadius(10.0f);
	//treeNode -> SetBoundingRadius(14.0f);
}


void World :: Update ( float msec ) 
{
	transform = transform * Matrix4 :: Rotation ( msec / 10.0f, Vector3 (0 ,1 ,0));
	
	sunNode -> SetTransform ( Matrix4 :: Rotation (- msec / 10.0f, Vector3 (1 ,0 ,0)));
	treeNode -> SetTransform ( sunNode -> GetTransform () *Matrix4 :: Rotation ( msec / 10.0f, Vector3 (1 ,0 ,0)));
	SceneNode :: Update ( msec );

}