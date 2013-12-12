#include "MyGame.h"
#include <queue>
/*
Creates a really simple scene for our game - A cube robot standing on
a floor. As the module progresses you'll see how to get the robot moving
around in a physically accurate manner, and how to stop it falling
through the floor as gravity is added to the scene. 

You can completely change all of this if you want, it's your game!

*/
MyGame::MyGame(Vector3& OGGravity)	{
	gameCamera = new Camera(-30.0f,0.0f,Vector3(0,450,850));

	Renderer::GetRenderer().SetCamera(gameCamera);

	CubeRobot::CreateCube();

	gravity = OGGravity;

	/*
	We're going to manage the meshes we need in our game in the game class!

	You can do this with textures, too if you want - but you might want to make
	some sort of 'get / load texture' part of the Renderer or OGLRenderer, so as
	to encapsulate the API-specific parts of texture loading behind a class so
	we don't care whether the renderer is OpenGL / Direct3D / using SOIL or 
	something else...
	*/
	cube	= new OBJMesh("../../Meshes/centeredcube.obj");
	quad	= Mesh::GenerateQuad();
	sphere	= new OBJMesh("../../Meshes/sphereNew.obj");
	rubble  = new OBJMesh("../../Meshes/sphereNew.obj");
	UFO		= new OBJMesh("../../Meshes/centeredUFO.obj");
	quad-> SetTexture ( SOIL_load_OGL_texture ("../../Textures/grass.jpg",SOIL_LOAD_AUTO , SOIL_CREATE_NEW_ID , 0));
	UFO-> SetTexture ( SOIL_load_OGL_texture ("../../Textures/Top1.jpg",SOIL_LOAD_AUTO , SOIL_CREATE_NEW_ID , 0));
	sphere-> SetTexture ( SOIL_load_OGL_texture ("../../Textures/earth_sphere.jpg",SOIL_LOAD_AUTO , SOIL_CREATE_NEW_ID , 0));
	rubble-> SetTexture ( SOIL_load_OGL_texture ("../../Textures/destroyed.jpg",SOIL_LOAD_AUTO , SOIL_CREATE_NEW_ID , 0));
	/*
	A more 'robust' system would check the entities vector for duplicates so as
	to not cause problems...why not give it a go?
	*/
	allEntities.push_back(BuildQuadEntity(1000.0f));
	//allEntities.push_back(BuildRobotEntity());
	
	//create UFO
	/*
	GameEntity* ufo = BuildUFOEntity(80);
	ufo->GetRenderNode().SetModelScale(Vector3(50,50,50));
	ufo->GetPhysicsNode().setPosition(Vector3(0, 150, 0));		
	ufo->GetPhysicsNode().SetMass(1);
	ufo->GetPhysicsNode().setHealth(10);
	ufo->GetPhysicsNode().setGravity(Vector3(0,0,0));
	ufo->GetPhysicsNode().calcCubeInvInertia(10);
	ufo->GetPhysicsNode().setAngularVelocity(Vector3(0,10,0));
	ufo->GetPhysicsNode().setAngularDamping(Vector3(1,1,1)); //no damping on rotations
	ufo->GetPhysicsNode().SetCollisionType(COLLISION_SPHERE);
	allEntities.push_back(ufo); */
	

}

MyGame::~MyGame(void)	{
	/*
	We're done with our assets now, so we can delete them
	*/
	delete cube;
	delete quad;
	delete sphere;

	CubeRobot::DeleteCube();

	//GameClass destructor will destroy your entities for you...
}

/*
Here's the base 'skeleton' of your game update loop! You will presumably
want your games to have some sort of internal logic to them, and this
logic will be added to this function.
*/
void MyGame::UpdateGame(float msec, int& size, int& score, int& ents) {
	

	if(gameCamera) {
		gameCamera->UpdateCamera(msec);
	}

	vector<GameEntity*> toDestroy;

	for(vector<GameEntity*>::iterator i = allEntities.begin(); i != allEntities.end(); ++i) {
                (*i)->Update(msec);
				if((*i)->GetPhysicsNode().getHealth() <= 0) toDestroy.push_back((*i));
    }

	for(vector<GameEntity*>::iterator i = toDestroy.begin(); i != toDestroy.end(); ++i){
		explode(*(*i), score);
	}

	if(Window::GetKeyboard()->KeyTriggered(KEYBOARD_E)){

		//set restful object
		GameEntity* cube = BuildSphereEntity(size);
		//cube->GetRenderNode().SetColour(Vector4(1,0,0,1));//red
		//start the projectile at the camera position and set object physics properties
		cube->GetPhysicsNode().setPosition(gameCamera->GetPosition()+ Vector3(0, -5, 0));
		cube->GetPhysicsNode().SetMass(1);
		cube->GetPhysicsNode().setHealth(5);
		cube->GetPhysicsNode().calcCubeInvInertia(10);
		cube->GetPhysicsNode().SetCollisionType(COLLISION_SPHERE);
		cube->setSize(size);
		cube->GetPhysicsNode().setGravity(Vector3(0,0,0));
		//add projectile to allEntities list
		allEntities.push_back(cube);
	}

	if(Window::GetKeyboard()->KeyTriggered(KEYBOARD_Q)){

		//shoot a projectile
		GameEntity* cube = BuildSphereEntity(size);
		//cube->GetRenderNode().SetColour(Vector4(1,0,0,1));//red
		//start the projectile at the camera position and set object physics properties
		Vector3 force = gameCamera->GetForwardVector()*-2;
		cube->GetPhysicsNode().setPosition(gameCamera->GetPosition()+ Vector3(0, -5, 0));
		cube->GetPhysicsNode().setLinearVelocity(force);
		cube->GetPhysicsNode().SetMass(1);
		cube->GetPhysicsNode().setHealth(5);
		cube->GetPhysicsNode().calcCubeInvInertia(10);
		cube->GetPhysicsNode().setGravity(gravity);
		cube->GetPhysicsNode().SetCollisionType(COLLISION_SPHERE);
		cube->setSize(size);
		//add projectile to allEntities list
		allEntities.push_back(cube);
	}

	if(Window::GetKeyboard()->KeyTriggered(KEYBOARD_P)&& size<128){
		size*=2;
	}

	if(Window::GetKeyboard()->KeyTriggered(KEYBOARD_O)&& size>2){
		size/=2;
	}

	ents = allEntities.size();

	/*
	Here's how we can use OGLRenderer's inbuilt debug-drawing functions! 
	I meant to talk about these in the graphics module - Oops!

	We can draw squares, lines, crosses and circles, of varying size and
	colour - in either perspective or orthographic mode.

	Orthographic debug drawing uses a 'virtual canvas' of 720 * 480 - 
	that is 0,0 is the top left, and 720,480 is the bottom right. A function
	inside OGLRenderer is provided to convert clip space coordinates into
	this canvas space coordinates. How you determine clip space is up to you -
	maybe your renderer has getters for the view and projection matrix?

	Or maybe your Camera class could be extended to contain a projection matrix?
	Then your game would be able to determine clip space coordinates for its
	active Camera without having to involve the Renderer at all?

	Perspective debug drawing relies on the view and projection matrices inside
	the renderer being correct at the point where 'SwapBuffers' is called. As
	long as these are valid, your perspective drawing will appear in the world.

	This gets a bit more tricky with advanced rendering techniques like deferred
	rendering, as there's no guarantee of the state of the depth buffer, or that
	the perspective matrix isn't othographic. Therefore, you might want to draw
	your debug lines before the inbuilt position before SwapBuffers - there are
	two OGLRenderer functions DrawDebugPerspective and DrawDebugOrtho that can
	be called at the appropriate place in the pipeline. Both take in a viewProj
	matrix as an optional parameter.

	Debug rendering uses its own debug shader, and so should be unnaffected by
	and shader changes made 'outside' of debug drawing

	*/
	//Lets draw a box around the cube robot!
	Renderer::GetRenderer().DrawDebugBox(DEBUGDRAW_PERSPECTIVE, Vector3(0,51,0), Vector3(100,100,100), Vector3(1,0,0));

	////We'll assume he's aiming at something...so let's draw a line from the cube robot to the target
	////The 1 on the y axis is simply to prevent z-fighting!
	Renderer::GetRenderer().DrawDebugLine(DEBUGDRAW_PERSPECTIVE, Vector3(0,1,0),Vector3(200,1,200), Vector3(0,0,1), Vector3(1,0,0));

	////Maybe he's looking for treasure? X marks the spot!
	Renderer::GetRenderer().DrawDebugCross(DEBUGDRAW_PERSPECTIVE, Vector3(200,1,200),Vector3(50,50,50), Vector3(0,0,0));

	////CubeRobot is looking at his treasure map upside down!, the treasure's really here...
	Renderer::GetRenderer().DrawDebugCircle(DEBUGDRAW_PERSPECTIVE, Vector3(-200,1,-200),50.0f, Vector3(0,1,0));

}

/*
Makes an entity that looks like a CubeRobot! You'll probably want to modify
this so that you can have different sized robots, with different masses and
so on!
*/
GameEntity* MyGame::BuildRobotEntity() {
	GameEntity*g = new GameEntity(new CubeRobot(), new PhysicsNode());

	g->ConnectToSystems();
	return g;
}

/*
Makes a cube. Every game has a crate in it somewhere!
*/
GameEntity* MyGame::BuildCubeEntity(float size) {
	GameEntity*g = new GameEntity(new SceneNode(cube), new PhysicsNode());
	g->ConnectToSystems();

	SceneNode &test = g->GetRenderNode();

	test.SetModelScale(Vector3(size,size,size));
	test.SetBoundingRadius(size);

	return g;
}

/*
Makes a UFO
*/
GameEntity* MyGame::BuildUFOEntity(float size) {
	GameEntity*g = new GameEntity(new SceneNode(UFO), new PhysicsNode());
	g->GetPhysicsNode().setSize(size);
	g->ConnectToSystems();
	return g;
}



/*
Makes a sphere.
*/
GameEntity* MyGame::BuildSphereEntity(float radius) {
	SceneNode* s = new SceneNode(sphere);

	s->SetModelScale(Vector3(radius,radius,radius));
	s->SetBoundingRadius(radius);

	GameEntity*g = new GameEntity(s, new PhysicsNode());
	g->ConnectToSystems();
	return g;
}

//create rubble
GameEntity* MyGame::BuildRubbleEntity(float radius) {
	SceneNode* s = new SceneNode(rubble);

	s->SetModelScale(Vector3(radius,radius,radius));
	s->SetBoundingRadius(radius);

	GameEntity*g = new GameEntity(s, new PhysicsNode());
	g->ConnectToSystems();
	return g;
}

/*
Makes a flat quad, initially oriented such that we can use it as a simple
floor. 
*/
GameEntity* MyGame::BuildQuadEntity(float size) {
	 SceneNode* s = new SceneNode(quad);

     s->SetModelScale(Vector3(size,size,size));
     s->SetBoundingRadius(size);

     PhysicsNode*p = new PhysicsNode(Quaternion::AxisAngleToQuaterion(Vector3(1,0,0), 90.0f), Vector3());

     GameEntity*g = new GameEntity(s, p);
     g->GetPhysicsNode().SetCollisionType(COLLISION_PLANE);
     g->GetPhysicsNode().SetMass(0);
     g->GetPhysicsNode().setHealth(1000);
     float invIn[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1};
     g->GetPhysicsNode().setInertia(Matrix4(invIn));
     g->ConnectToSystems();
     return g;
}

void MyGame::explode(GameEntity& entity, int& score){
	int newSize = entity.GetPhysicsNode().getSize()/2;
	Vector3 basePos = entity.GetPhysicsNode().GetPosition();
	Vector3 baseVel = entity.GetPhysicsNode().GetLinearVelocity();

	Vector3 v0 = Vector3(0.5f,0,0);
	Vector3 v1 = Vector3(0,0.5f,0);
	Vector3 v2 = Vector3(0,0,0.5f);
	Vector3 v3 = Vector3(-0.5f,0,0);
	Vector3 v4 = Vector3(0,0,-0.5f);
	//prevent recursive destruction!
	if(newSize > 2){
		//create four smaller entities
		for(int i = 0; i < 5; i++){

			GameEntity* rubble = BuildRubbleEntity(newSize);
			rubble->GetPhysicsNode().setPosition(basePos + Vector3(i * 7, i *8, i*7));
			
			switch (i){
			case 0:
				rubble->GetPhysicsNode().setLinearVelocity(baseVel + v0 );
				break;
			case 1:
				rubble->GetPhysicsNode().setLinearVelocity(baseVel + v1 );
				break;
			case 2:
				rubble->GetPhysicsNode().setLinearVelocity(baseVel + v2 );
				break;
			case 3:
				rubble->GetPhysicsNode().setLinearVelocity(baseVel + v3 );
				break;
			case 4:
				rubble->GetPhysicsNode().setLinearVelocity(baseVel + v4 );
				break;
			}
			rubble->GetPhysicsNode().SetMass(1);
			rubble->GetPhysicsNode().setHealth(10);
			rubble->GetPhysicsNode().calcCubeInvInertia(10);
			rubble->GetPhysicsNode().setGravity(gravity);
			if(newSize > 2 && allEntities.size() < 500)
				rubble->GetPhysicsNode().SetCollisionType(COLLISION_SPHERE);
			else
				rubble->GetPhysicsNode().SetCollisionType(PLANE_ONLY_SPHERE);
			rubble->setSize(newSize);
			allEntities.push_back(rubble);
		}
		//add size to score
		score += newSize * 4;
		//remove the physics and render nodes of this entity
		entity.DisconnectFromSystems();
		//finally, remove this entity from this class
		for(vector<GameEntity*>::iterator i = allEntities.begin(); i != allEntities.end(); ++i) {
			if((*i) == &entity) {
				allEntities.erase(i);
				break;
			}
		}
	}
	
}