#include "stdafx.h"
#include "DecisionMaker.h"
#include "Map/Map.h"
//player
//enemy

DecisionMaker::DecisionMaker(Map& m) : map(m)
{
	chassing = false;
}

DecisionMaker::~DecisionMaker()
{
}


bool DecisionMaker::SpotPlayer(CVector playerPos, CVector enemyPos)
{
	bool canSee = true;
	for (auto obstacle : map.checkObects)
	{
		if (Intersection::FindIntersection(playerPos, enemyPos,
			CVector(obstacle->GetLeft(), obstacle->GetTop()),
			CVector(obstacle->GetRight(), obstacle->GetBottom())) ||
			Intersection::FindIntersection(playerPos, enemyPos,
				CVector(obstacle->GetLeft(), obstacle->GetBottom()),
				CVector(obstacle->GetRight(), obstacle->GetTop())))
		{
			canSee = false;
			break;
		}

	}
	return canSee;
}

void DecisionMaker::ChasePlayer(CVector playerPos, CVector enemyPos)
{
	cout << chassing << endl;
}

void DecisionMaker::ReturnToBaseState()
{
}