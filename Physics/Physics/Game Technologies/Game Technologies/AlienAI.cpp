#include "AlienAI.h"

AlienAI::AlienAI(GameEntity& entity){
	//assign this AI a game entity to control
	this->entity = entity;
	//AI starts as being idle
	currentState = IDLE;

	
	
}


void AlienAI::update(){

	Vector3 currentPos = entity.GetPhysicsNode().GetPosition();

	if(currentState == IDLE && !enemies.empty()){
		//if the AI is currently idle and the list of enemies
		//is not empty, calculate a route to the first target
		
		Vector3 enemySpeed = enemies.top().entity->GetPhysicsNode().GetLinearVelocity();
		if(abs(enemySpeed.x) + abs(enemySpeed.y) + abs(enemySpeed.z) <= 0.1){
			currentState = PONDERING;
			Node root = Node();
			root.location = currentPos;
			root.directionToParent = NULL;
			root.distanceToParent = 0;
			root.movementCost = 0;
			root.Fcost = calcFCost(root);			
			//visitedNodes.push_back(root);
			nodes.push(root);
			planRoute();
			currentState = NAVIGATE;
		}
	}// end idle

	if(currentState == NAVIGATE){
		if( entity.GetPhysicsNode().GetPosition() == route.front()){
			// we're at the node
			route.pop();
			//onto the next location! Unless...
			if(route.empty()){
				//AI is now at the target location!
				currentState = ATTACK;
			}
		}
		//double check we didn't just arrive
		if(currentState == NAVIGATE){			
			//move the AI towards the next element in the route stack
			if(currentPos.x > route.front().x){
				entity.GetPhysicsNode().setPosition(Vector3(currentPos.x + 1, currentPos.y, currentPos.z));
			}else if(currentPos.x < route.front().x){
				Vector3(currentPos.x - 1, currentPos.y, currentPos.z);
			}

			if(currentPos.y > route.front().y){
				entity.GetPhysicsNode().setPosition(Vector3(currentPos.x, currentPos.y + 1, currentPos.z));
			}else if(currentPos.y < route.front().y){
				Vector3(currentPos.x, currentPos.y - 1, currentPos.z);
			}

			if(currentPos.z > route.front().z){
				entity.GetPhysicsNode().setPosition(Vector3(currentPos.x, currentPos.y, currentPos.z + 1));
			}else if(currentPos.z < route.front().z){
				Vector3(currentPos.x, currentPos.y, currentPos.z - 1);
			}

		}
	}//end navigate


	//TODO ATTACK
}


void AlienAI::addTarget(GameEntity& target){

	//push this new target to the existing queue
	//queue will automatically sort according to
	//object's distance from this AI
	Target newTarget = Target();
	newTarget.entity = &target;
	newTarget.predator = this;

	enemies.push(newTarget);
	
	//replan route if new node is the new closest enemy and the AI isn't dead AND the
	//new target is stationary
	if(!enemies.empty()){
		if(enemies.top() == newTarget && currentState != DEAD){
			if(enemies.top().entity->GetPhysicsNode().GetLinearVelocity() == Vector3(0,0,0)){

				Node root = Node();
				root.location = entity.GetPhysicsNode().GetPosition();
				root.directionToParent = NULL;
				root.distanceToParent = 0;
				root.movementCost = 0;
				root.Fcost = calcFCost(root);
				//visitedNodes.push_back(root);
				nodes.push(root);

				//reset the current path
				//(C++ doesn't have a stack clear function...)
				while(!route.empty()){
					route.pop();
				}

				//recursively create a tree to search using A*
				planRoute();
			}
		}
	}
}

int AlienAI::calcFCost(Node& node){
	//cost = movement cost so far + estimated cost from here onwards (hueristic)
	Vector3 target = enemies.top().entity->GetPhysicsNode().GetPosition();
	float euclidean = sqrt(pow(abs(node.location.x - target.x), 2) + 
							pow(abs(node.location.y - target.y), 2) +
							pow(abs(node.location.z - target.z), 2));
	return node.movementCost + euclidean;
}

//creates a node using the current top node in the queue and given params
void AlienAI::createNodes(int distanceToParent, char direction, Node root){
	Node node = Node();
	
	if(direction == 'U')
		node.location = root.location + Vector3(0,-distanceToParent,0);
	else if(direction == 'D')
		node.location = root.location + Vector3(0,distanceToParent,0);
	else if(direction == 'L')
		node.location = root.location + Vector3(-distanceToParent,0,0);
	else if(direction == 'R')
		node.location = root.location + Vector3(distanceToParent,0,0);
	else if(direction == 'F')
		node.location = root.location + Vector3(0,0,-distanceToParent);
	else if(direction == 'B')
		node.location = root.location + Vector3(0,0,distanceToParent);

	node.directionToParent = direction;
	node.distanceToParent = distanceToParent;
	node.movementCost = root.movementCost + distanceToParent;
	node.Fcost = calcFCost(node);

	bool visited = false;	
	
	//check not already in queue set
	if(checkIfSetContains(node.location) != true ){

		for(int i = 0; i < visitedNodes.size(); i++){
			//see if we've actually already looked here. 
			if(node.location == visitedNodes[i].location){
				visited = true;
				//Check to see if we've found a shorter path:
				if(node.Fcost < visitedNodes[i].Fcost){
					//if we have, swap out the old node's info
					visitedNodes[i].directionToParent = direction;
					visitedNodes[i].movementCost = node.movementCost;
					visitedNodes[i].Fcost = node.Fcost;
					break;
				}
			}
		} 	 
	
		//if visited is still false we haven't seen this node before so add it for consideration
		if(!visited){
			queuedNodeSet.insert(node.location);
			nodes.push(node);
		}
	}
}

void AlienAI::planRoute(){

	//recursive A* pathfinding function
	Node root = nodes.top();

	nodes.pop();
	visitedNodes.push_back(root);

	if(root.location == enemies.top().entity->GetPhysicsNode().GetPosition()){
		//We've found the final node! Finalise the route the AI should take
		populateRouteQueue(root.distanceToParent, root.directionToParent, root.location);	
	}else{
		
		Vector3 target = enemies.top().entity->GetPhysicsNode().GetPosition();

		int distance = abs( root.location.x - target.x) + 
						abs(root.location.y - target.y) +
						abs(root.location.z - target.z);

		/*
		increment in stages - measure nodes a larger distance from root according to hueristic.
		For instance, no need to measure on a per unit basis when we have thousands of units
		to traverse. Measure of increments of 15 instead, then lower as we get closer. This will
		make the graph we have to consider much smaller. Can't make increments too large as otherwise
		collisions will occur!
		*/
		int increment = 1;
		if(distance > 150) increment = 40;
		else if (distance > 50) increment = 15;
		else if (distance > 30) increment = 5;

		//create nodes to consider
		createNodes(increment, 'U', root);
		createNodes(increment, 'D', root);
		createNodes(increment, 'L', root);
		createNodes(increment, 'R', root);
		createNodes(increment, 'F', root);
		createNodes(increment, 'B', root);

		//check to see if the top node has changed, and final node has been found
		if(nodes.top().location == enemies.top().entity->GetPhysicsNode().GetPosition()){
			//it is! We now know the route. Go back up via the node's parents until we get
			// to the root node, which has no parent
			populateRouteQueue(increment, nodes.top().directionToParent, nodes.top().location);
		}else{	
			planRoute();
		}
		
	}
	
}

//recursively populate queue until there is no parent (root). Parameters are the child's details.
void AlienAI::populateRouteQueue(int distanceToParent, char directionToParent, Vector3 position){
	
	//calculate new position based on the child's and directions to parent
	Vector3 newPos = position;

	if(directionToParent == 'U')
		newPos += Vector3(0,-distanceToParent,0);
	else if(directionToParent == 'D')
		newPos += Vector3(0,distanceToParent,0);
	else if(directionToParent == 'L')
		newPos += Vector3(-distanceToParent,0,0);
	else if(directionToParent == 'R')
		newPos += Vector3(distanceToParent,0,0);
	else if(directionToParent == 'F')
		newPos += Vector3(0,0,-distanceToParent);
	else if(directionToParent == 'B')
		newPos += Vector3(0,0,distanceToParent);

	if(newPos != entity.GetPhysicsNode().GetPosition()){
		//if that position isn't the AI's position, search visited nodes for the 
		//calculated position and run this function with new details			
		route.push(newPos);
		bool found = false;

		for(int i = 0; i < visitedNodes.size(); i++){
			//see if we've actually already looked here. 
			if(newPos == visitedNodes[i].location){
				populateRouteQueue(visitedNodes[i].distanceToParent,
								   visitedNodes[i].directionToParent,
								   newPos);	
				found = true;
				break;
			}
			//something bad has happened if this forloop has finished successfully...
			if(!found)
				cout << "Node not found!" << endl;
		} 	
	}

	
	//return up through all dream levels
	return;
}