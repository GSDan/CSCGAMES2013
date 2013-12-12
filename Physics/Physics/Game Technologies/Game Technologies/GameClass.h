/******************************************************************************
Class:GameClass
Implements:
Author:Rich Davison	<richard.davison4@newcastle.ac.uk> and YOU!
Description: A very simple 'base class' with which to base your game projects
on. This will simply update all of the 'sub systems' of the program
(ie the Renderer, the PhysicsSystem, and later on the SoundSystem).

This class also holds a vector of GameEntities - these are the classes that
represent all of the objects in your game.

There's also a Camera in here, so that the 'game' component of the program has
control over what the Renderer 'sub system' looks at. 

-_-_-_-_-_-_-_,------,   
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""   

*//////////////////////////////////////////////////////////////////////////////

#pragma once
#include "Renderer.h"
#include "PhysicsSystem.h"
#include "GameEntity.h"
#include "AlienAI.h"

#include <vector>

#define RENDER_HZ	60
#define PHYSICS_HZ	120

#define PHYSICS_TIMESTEP (1000.0f / (float)PHYSICS_HZ)

class GameClass	{
public:
	GameClass();
	~GameClass(void);

	virtual void UpdateCore(float msec, Vector3& gravity, int& size, int& score, int& ents, int& fps);

	virtual void UpdateGame(float msec, int& size, int& score, int& ents) = 0;

	static GameClass& GetGameClass() { return *instance;}

protected:
	float renderCounter;
	float physicsCounter;

	vector<GameEntity*> allEntities;

	Camera* gameCamera;
	AlienAI* AI;
	static GameClass* instance;
};

