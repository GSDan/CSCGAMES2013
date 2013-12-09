#include "AlienAI.h"

AlienAI::AlienAI(GameEntity& entity){
	//assign this AI a game entity to control
	this->entity = entity;
	//AI starts as being idle
	currentState = IDLE;
}


void AlienAI::addTarget(GameEntity& target){

	//push this new target to the existing queue
	//queue will automatically sort according to
	//object's distance from this AI
	Target newTarget = Target();
	newTarget.entity = &target;
	newTarget.predator = this;

	enemies.push(newTarget);
	
	//replan route if new node is the new closest enemy
	if(enemies.top() == newTarget)
		planRoute(entity.GetPhysicsNode().GetPosition());
}