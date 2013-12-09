#include "GameEntity.h"
#include <vector>

class AlienAI{

public:
	AlienAI(GameEntity& entity){ this->entity = entity; }
	~AlienAI();
	void updateAI();
	void attack();
private:
	GameEntity entity;
};