#include "AlienAI.h"

AlienAI::AlienAI(GameEntity& entity){
	enemies = new priority_queue<Target, vector<Target>, EntityCompareDist>();
	nodes = new priority_queue<Node, vector<Node>, NodeCompare>();

	//assign this AI a game entity to control
	this->entity = entity;
	//AI starts as being idle
	currentState = IDLE;
}


void AlienAI::update(){

	Vector3 currentPos = entity.GetPhysicsNode().GetPosition();

	if(currentState == IDLE && !enemies->empty()){
		//if the AI is currently idle and the list of enemies
		//is not empty, calculate a route to the first target
		
		Vector3 enemySpeed = enemies->top().entity->GetPhysicsNode().GetLinearVelocity();
		if(abs(enemySpeed.x) + abs(enemySpeed.y) + abs(enemySpeed.z) <= 0.1){
			currentState = PONDERING;
			Node *root = new Node();
			root->location = entity.GetPhysicsNode().GetPosition();
			root->parent = NULL;
			root->movementCost = 0;
			root->Fcost = calcFCost(*root);
			visitedNodes.push_back(*root);
			planRoute(root);
			currentState = NAVIGATE;
		}
	}// end idle

	if(currentState == NAVIGATE){
		if(entity.GetPhysicsNode().GetPosition() == route.top().location){
			route.pop();
			if(route.empty()){
				//AI is now at the target location!
				currentState == ATTACK;
			}
		}
		//double check we didn't just arrive
		if(currentState == NAVIGATE){			
			//move the AI towards the next element in the route stack
			if(currentPos.x > route.top().location.x){
				entity.GetPhysicsNode().setPosition(Vector3(currentPos.x + 1, currentPos.y, currentPos.z));
			}else if(currentPos.x < route.top().location.x){
				Vector3(currentPos.x - 1, currentPos.y, currentPos.z);
			}

			if(currentPos.y > route.top().location.y){
				entity.GetPhysicsNode().setPosition(Vector3(currentPos.x, currentPos.y + 1, currentPos.z));
			}else if(currentPos.y < route.top().location.y){
				Vector3(currentPos.x, currentPos.y - 1, currentPos.z);
			}

			if(currentPos.z > route.top().location.z){
				entity.GetPhysicsNode().setPosition(Vector3(currentPos.x, currentPos.y, currentPos.z + 1));
			}else if(currentPos.z < route.top().location.z){
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

	enemies->push(newTarget);
	
	//replan route if new node is the new closest enemy and the AI isn't dead AND the
	//new target is stationary
	if(!enemies->empty()){
		if(enemies->top() == newTarget && currentState != DEAD){
			if(enemies->top().entity->GetPhysicsNode().GetLinearVelocity() == Vector3(0,0,0)){
				Node *root = new Node();
				root->location = entity.GetPhysicsNode().GetPosition();
				root->parent = NULL;
				root->movementCost = 0;
				root->Fcost = calcFCost(*root);
				visitedNodes.push_back(*root);

				while(!route.empty()){
					route.pop();
				}

				planRoute(root);
			}
		}
	}
}

int AlienAI::calcFCost(Node& node){
	//cost = movement cost so far + estimated cost from here onwards (hueristic)
	Vector3 target = enemies->top().entity->GetPhysicsNode().GetPosition();
	int Fcost = node.movementCost + abs(node.location.x - target.x) + 
									abs(node.location.y - target.y) +
									abs(node.location.z - target.z);

	return Fcost;
}

void AlienAI::createNodes(Node& root, Vector3 increment){
	Node *node = new Node();
	node->location = root.location + increment;
	node->parent = &root;
	node->movementCost = root.movementCost + abs(increment.x) + abs(increment.y) + abs(increment.z);
	node->Fcost = calcFCost(*node);
	bool visited = false;

	for(int i = 0; i < visitedNodes.size(); i++){
		//see if we've actually already looked here. 
		if(node->location == visitedNodes[i].location){
			visited = true;
			//Check to see if we've found a shorter path:
			if(node->Fcost < visitedNodes[i].Fcost){
				//if we have, swap out the old node's info
				visitedNodes[i].parent = node->parent;
				visitedNodes[i].movementCost = node->movementCost;
				visitedNodes[i].Fcost = node->Fcost;
				break;
			}
		}
	} 
	//if visited is still false we haven't seen this node before so add it for consideration
	if(!visited)
		nodes->push(*node);
}

void AlienAI::planRoute(Node* root){

	bool visited = false;

	for(int i = 0; i < visitedNodes.size(); i++){
		//see if we've actually already looked here. 
		if(root->location == visitedNodes[i].location){
			visited = true;
		}
	} 
	//if not int the visited array, put it in
	
	visitedNodes.push_back(*root);
	Vector3 target = enemies->top().entity->GetPhysicsNode().GetPosition();

	int distance = abs( root->location.x - target.x) + 
					abs(root->location.y - target.y) +
					abs(root->location.z - target.z);

	/*
	increment in stages - measure nodes a larger distance from root according to hueristic.
	For instance, no need to measure on a per unit basis when we have thousands of units
	to traverse. Measure of increments of 15 instead, then lower as we get closer. This will
	make the graph we have to consider much smaller. Can't make increments too large as otherwise
	collisions will occur!
	*/
	int increment = 1;
	if(distance > 150) increment = 20;
	else if (distance > 50) increment = 15;
	else if (distance > 30) increment = 5;
	
	//create nodes to consider
	createNodes(*root, Vector3(0, increment, 0));
	createNodes(*root, Vector3(0, -increment, 0));
	createNodes(*root, Vector3(increment, 0, 0));
	createNodes(*root, Vector3(-increment, 0, 0));
	createNodes(*root, Vector3(0, 0, increment));
	createNodes(*root, Vector3(0, 0, -increment));

	//check to see if next node in queue is actually the target
	if(nodes->top().location == enemies->top().entity->GetPhysicsNode().GetPosition()){
		//it is! We now know the route. Go back up via the node's parents until we get
		// to the root node, which has no parent
		populateRouteStack(&nodes->top());
	}else{
		planRoute(&nodes->top());
	}
	
}

void AlienAI::populateRouteStack(Node* node){
	//recursively populate stack until there is no parent
	if(node->parent != NULL){
		route.push(*node);
		populateRouteStack(node->parent);
	}
}