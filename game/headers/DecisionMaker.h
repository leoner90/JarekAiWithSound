#pragma once
#include "Intersection.h"

class Map;

class DecisionMaker : public Intersection
{
public:
	DecisionMaker(Map& m);
	~DecisionMaker();
	bool SpotPlayer(CVector playerPos, CVector enemyPos);
	void ChasePlayer(CVector playerPos, CVector enemyPos);
	void ReturnToBaseState();
private:
	Map& map;
	bool chassing;
};