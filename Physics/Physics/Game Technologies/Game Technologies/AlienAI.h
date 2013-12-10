#include "GameEntity.h"
#include <vector>
#include <queue>
#include <stack>
#include <set>
#include <math.h>

class AlienAI{

enum BehaviourState {
	IDLE,
	NAVIGATE,
	ATTACK,
	PONDERING,
	DEAD
};

class Node {
	public:
		Node(){ location = Vector3(0,0,0); parent = NULL; movementCost = 0;};
		Vector3 location;
		Node* parent;
		int movementCost;
		int Fcost; //cost = movement cost so far + estimated cost from here onwards (hueristic)
};

struct NodeCompare{
	bool operator()(Node& lhs, Node& rhs){
		return lhs.Fcost > rhs.Fcost;
		//compare the movement cost so far of each object + their hueristic (lower = better)
	}
};


public:
	AlienAI(void){};
	AlienAI(GameEntity& entity);
	//~AlienAI();

	void update();
	void addTarget(GameEntity& target);
	void kill(){ currentState = DEAD; }
	Vector3 getPosition() { return entity.GetPhysicsNode().GetPosition(); }
	GameEntity* getCurrentTarget(){ return enemies.top().entity;}
	BehaviourState getBehaviourState(){ return currentState;};

protected:

	class Target {
	public:
		Target(){
			entity = new GameEntity();
			predator = new AlienAI();
		}
		GameEntity* entity;
		AlienAI* predator;

		inline bool operator==(const Target &rhs){
		
			if(entity->GetPhysicsNode().GetPosition() == rhs.entity->GetPhysicsNode().GetPosition() && 
				entity->GetPhysicsNode().GetLinearVelocity() == rhs.entity->GetPhysicsNode().GetLinearVelocity() &&
				entity->GetRenderNode().GetMesh() == rhs.entity->GetRenderNode().GetMesh() ){
				//if both sides are in the same place at the same time, with the same mesh 
				//and same velocity, it's probably pretty safe to assume they're the same object
				return true;
			}else{
				return false;
			}	
		}
	};

	struct EntityCompareDist{
		//compare entities by their distance from the AI (lower = better)
		bool operator()(Target& lhs, Target& rhs){
			Vector3 lhsValue = lhs.entity->GetPhysicsNode().DistanceTo(lhs.predator->getPosition());
			Vector3 rhsValue = rhs.entity->GetPhysicsNode().DistanceTo(rhs.predator->getPosition());
			return lhsValue < rhsValue;
		}
	};

	//a priority queue sorted by distance from current location
	priority_queue<Target, vector<Target>, EntityCompareDist> enemies;
	//priority queue of potential A* graph nodes
	priority_queue<Node, vector<Node>, NodeCompare> nodes;
	vector<Node> visitedNodes;
	BehaviourState currentState;
	GameEntity entity;
	set<Vector3> queuedNodeSet;
	//set<Node>::iterator setIt;

	void createNodes(Node& root, Vector3 increment);
	void planRoute(Node* root);
	void populateRouteStack(Node* node);
	stack <Node> route;
	void sortTargets();
	void attack();
	int calcFCost(Node& node);

	bool checkIfSetContains(Vector3 &vec)
	{
		set<Vector3>::iterator it;
		for (it=queuedNodeSet.begin(); it!=queuedNodeSet.end(); ++it)
		 {
			 if((*it).x == vec.x && (*it).y == vec.y && (*it).z == vec.z)
			 {
				 return true;
			 }
		 }

		return false;
	}
	
	
};

