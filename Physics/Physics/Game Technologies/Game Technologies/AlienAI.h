#include "GameEntity.h"
#include <vector>
#include <queue>

class AlienAI{

enum BehaviourState {
	IDLE,
	NAVIGATE,
	ATTACK,
	PONDERING,
	DEAD
};

public:
	AlienAI(GameEntity& entity);
	~AlienAI();

	void planRoute(Vector3 thisNode);
	void addTarget(GameEntity& target);
	void kill(){ currentState = DEAD; }
	Vector3 getPosition() { return entity.GetPhysicsNode().GetPosition(); }
	GameEntity* getCurrentTarget(){ return enemies.top().entity;}
	BehaviourState getBehaviourState(){ return currentState;};

protected:

	class Target {
	public:
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
		//
		bool operator()(Target& lhs, Target& rhs){
			Vector3 lhsValue = lhs.entity->GetPhysicsNode().DistanceTo(lhs.predator->getPosition());
			Vector3 rhsValue = rhs.entity->GetPhysicsNode().DistanceTo(rhs.predator->getPosition());
			return lhsValue < rhsValue;
		}
	};
	
	//a priority queue sorted by distance from current location
	priority_queue<Target, vector<Target>, EntityCompareDist> enemies;
	priority_queue<Vector3> nodes;
	BehaviourState currentState;
	GameEntity entity;
	
	void sortTargets();
	void attack();
	

};

