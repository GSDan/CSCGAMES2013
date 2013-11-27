/******************************************************************************
Class:MD5Node
Implements:SceneNode
Author:Rich Davison
Description: This year, we're going to more tightly integrate MD5 mesh
rendering to the concept of a scene graph. 

This simple subclass of the SceneNode contains the information required to
render and animate a skeletal mesh.

We need the data from the file we load (sourceData), a data structure to store
the current state of this node's skeleton (currentSkeleton) and a pointer to
the animation being used to modify this skeleton (currentAnim). 

Why don't we just keep this all in the MD5Mesh class? Doing that would mean
we'd either have to have only one particular instance of a mesh, or load the
entire mesh data multiple times! So instead, we keep the 'unique' per-instance
data in one class, and the mesh data used by these classes in another class.

For example, this lets us have 1000s of hell knights on screen, all potentially
with their own current animation, and current animation frame!

-_-_-_-_-_-_-_,------,   
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""   

*//////////////////////////////////////////////////////////////////////////////


#pragma once
#include "scenenode.h"
#include "MD5FileData.h"
#include "MD5Mesh.h"

class MD5Node : public SceneNode	{
public:
	MD5Node(const MD5FileData &ofType);
	~MD5Node(void);

	/*
	Updates the skeleton according to the animation frames held in the current
	applied MD5Anim.
	*/
	virtual void	Update(float msec);
	virtual void	Draw(const OGLRenderer &r);

	/*
	Searches the map of animations for an MD5Anim with the passed in name, and
	starts applying it to the current MD5Mesh
	*/
	void	PlayAnim(std::string name);	

protected:
	const MD5FileData&	sourceData;
	MD5Skeleton			currentSkeleton;
	MD5Anim*			currentAnim;

	float				frameTime;
	unsigned int		currentAnimFrame;	//Current frame of animation
};

