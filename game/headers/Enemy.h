#pragma once
#include "PathFinder.h"
#include "DecisionMaker.h"
#include "Dialogbox.h"

class Map;
class Player;

class Enemy : public PathFinder, public DecisionMaker, public Dialogbox
{
public:

	static  CVector saveCurrentPos;
	static bool isdogAllerted;
 

	Enemy(Map& map, Player& p, CSprite& eSprite,  CVector initPos, int type);
	void Draw(CGraphics* g);
	void Update(float time);
	void Patroling(float time);
	void Chassing(float gametime);
	void Attack(float time);
	void GettingDamage(float DamageAmount);
	void EnemyInterface();
	void Animation(float time);
	void callForAlert(float gameTime);
	void DogAlert(float gameTime);
	void AISwitchLightOn(float gameTime);
	bool dogAlarma;

	bool oneTimeAlert;
	bool catCallForAlert;


	bool humanAlarma;

	void movingToPoint(float gameTime);
	void ifwaipointNotEmptyGoto();
	bool isInCallForHelp;
	~Enemy();

	CSprite* enemySprite;
	bool IsDead;
	bool isPatroling;
	bool isEnemyStaticPos;
	int enemyType;

private:
	//Hp bars
	CSpriteRect* enemyHpBarRect = new CSpriteRect(0, 500, 60, 35, CColor::Red(), CColor::Red(), 0);
	Player& player;
	vector<CVector> currentWaypoints;
	vector<CVector> patrolVectorList;
	float positionHoldTimer;
	enum enemyStates { IDLE, PATROL, CHASE, ATTACK, WALK, RETURNTOPOS};
	int lastState;
	enum enemyType {CAT, DOG, HUMAN};
	
	bool playerChasingLastSpot = false;

	float currentHp, maxHp;
	float damage;
	float attackDellayTimer;

	float enemySpeed;
	CVector saveStaticPos;
	Map& map;

	
	CVector mainSpriteSize;


	CSoundPlayer  lightOn;
	CSoundPlayer attakSound;
	CSoundPlayer deadSound;
};