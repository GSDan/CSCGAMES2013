#include "GameClass.h"

GameClass* GameClass::instance = NULL;

GameClass::GameClass()	{
	renderCounter	= 0.0f;
	physicsCounter	= 0.0f;

	instance		= this;
}

GameClass::~GameClass(void)	{
	for(vector<GameEntity*>::iterator i = allEntities.begin(); i != allEntities.end(); ++i) {
		delete (*i);
	}
	delete gameCamera;
}

void GameClass::UpdateCore(float msec, Vector3& gravity, int& size, int& score, int& ents, int& fps) {
	renderCounter	-= msec;
	physicsCounter	+= msec;

	if(renderCounter <= 0.0f) {	//Update our rendering logic
		Renderer::GetRenderer().UpdateScene(1000.0f / (float)RENDER_HZ);
		Renderer::GetRenderer().RenderScene(size, score, ents, fps);
		renderCounter += (1000.0f / (float)RENDER_HZ);
	}

	while(physicsCounter >= 0.0f) {
		physicsCounter -= PHYSICS_TIMESTEP;
		PhysicsSystem::GetPhysicsSystem().Update(PHYSICS_TIMESTEP, gravity);
	}

	Renderer::GetRenderer().UpdateScene(msec);
	Renderer::GetRenderer().RenderScene(size, score, ents, fps);
}