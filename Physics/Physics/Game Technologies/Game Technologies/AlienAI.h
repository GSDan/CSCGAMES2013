#include "GameEntity.h"
#include <vector>
#include <queue>
#include <stack>
#include <set>
#include <math.h>




class Node {
	public:
		Node(){ distanceToParent = NULL; directionToParent = NULL;};		
		//lets us backtrack
		Vector3 location;
		int distanceToParent;
		char directionToParent;
		int movementCost;
		int Fcost; //cost = movement cost so far + estimated cost from here onwards (hueristic)
};

class NodeCompare
{
public:
	bool operator() (const Node &lhs, const Node &rhs) const
	{
		return lhs.Fcost > rhs.Fcost;
	}
};

class AlienAI{

enum BehaviourState {
	IDLE,
	NAVIGATE,
	ATTACK,
	PONDERING,
	DEAD
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
	

	void createNodes(int distanceToParent, char direction, Node root);
	void planRoute();
	void populateRouteQueue(int distanceToParent, char direction, Vector3 position);
	queue <Vector3> route;
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

