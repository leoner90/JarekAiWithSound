#pragma once
#include "Intersection.h"

class Map;

class DecisionMaker : public Intersection
{
public:
	DecisionMaker(Map& m);
	~DecisionMaker();
	bool SpotPlayer(CVector playerPos, CSprite& enemy, int enemyType);
	void ChasePlayer(CVector playerPos, CVector enemyPos);
	void ReturnToBaseState();
private:
	Map& map;
	bool chassing;
	enum enemyType {CAT, DOG, HUMAN}; 
};