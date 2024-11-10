#include "stdafx.h"
#include "DecisionMaker.h"
#include "Map/Map.h"


/*********** CONSTRUCTOR ***********/
DecisionMaker::DecisionMaker(Map& m) : map(m)
{
	chassing = false;
}


/*********** IS PLAYER IN SIGHT ***********/
bool DecisionMaker::SpotPlayer(CVector playerPos, CSprite& enemy, int enemyType)
{
    bool canSee = true;
    int angleOfView = 3;
    if (enemyType != CAT)
        angleOfView = 1;

    const float fieldOfViewCosine = cos(M_PI /  angleOfView); // 60 degrees of each side of direction

    CVector enemyDirection = enemy.GetVelocity().Normalize();
    CVector toPlayer = (playerPos - enemy.GetPos()).Normalize();

    float dotProduct = Dot(enemyDirection, toPlayer);

    if (dotProduct < fieldOfViewCosine)
        canSee = false;
    else
    {
        // Check for obstacles between the enemy and player
        for (auto obstacle : map.GetAllObstacles())
        {
            if (Intersection::FindIntersection(playerPos, enemy.GetPos(),
                CVector(obstacle->GetLeft(), obstacle->GetTop()),
                CVector(obstacle->GetRight(), obstacle->GetBottom())) ||
                Intersection::FindIntersection(playerPos, enemy.GetPos(),
                    CVector(obstacle->GetLeft(), obstacle->GetBottom()),
                    CVector(obstacle->GetRight(), obstacle->GetTop())))
            {
                canSee = false;
                break;
            }
        }
    }

    return canSee;

}

void DecisionMaker::ChasePlayer(CVector playerPos, CVector enemyPos)
{

}

void DecisionMaker::ReturnToBaseState()
{
}