#pragma once
#include "PathFinder.h"
#include "DecisionMaker.h"
#include "Dialogbox.h"

class Map;
class Player;

class Enemy : public PathFinder, public DecisionMaker, public Dialogbox
{
public:
	//STATIC VARIABLES, MOSTLY FOR DOG CALL -> TO REDO!
	static  CVector saveCurrentPos;
	static bool isdogAllerted;

	Enemy(Map& map, Player& p, CSprite& eSprite, CVector initPos, int type);
	~Enemy();
	void Draw(CGraphics* g);
	void Update(float time);

	//STATUSES
	void Patroling(float time);
	void Chassing(float gametime);

	//ANIMATION AND UI
	void EnemyInterface();
	void Animation(float time);

	//COMBAT
	void Attack(float time);
	void GettingDamage(float DamageAmount);

	//AI LOGIC AND MOVEMENT TO REDO IF WILL HAVE TIME
	void callForAlert(float gameTime);
	void DogAlert(float gameTime);
	void AISwitchLightOn(float gameTime);
	void movingToPoint(float gameTime);
	void ifwaipointNotEmptyGoto();

	//Getters
	CSprite* GetEnemySprite();
	bool IsEnemyDead();

private:
	//REFERENCES
	Player& player;
	Map& map;

	//SPRITES
	CSprite* enemySprite;

	//WAYPOINTS
	vector<CVector> currentWaypoints;
	vector<CVector> patrolVectorList;

	// CAT CALL FOR HELP STATUSES -> TO REDO!
	bool isInCallForHelp;
	bool dogAlarma;
	bool oneTimeAlert;
	bool catCallForAlert;
	bool humanAlarma;

	//Hp bars
	CSpriteRect* enemyHpBarRect = new CSpriteRect(0, 500, 60, 35, CColor::Red(), CColor::Red(), 0);
	float currentHp, maxHp;

	//combat
	float damage;
	float attackDellayTimer;

	//conditions and timers
	bool IsDead;
	bool isPatroling;
	float positionHoldTimer;
	bool playerChasingLastSpot = false;

	//is enemy static? -> not patroling
	bool isEnemyStaticPos;
	CVector saveStaticPos;

	//init type and speed
	int enemyType;
	float enemySpeed;

	//statuses
	enum enemyStates { IDLE, PATROL, CHASE, ATTACK, WALK, RETURNTOPOS };
	enum enemyType { CAT, DOG, HUMAN };

	//animation
	int lastState;

	//sounds
	CSoundPlayer  lightOn;
	CSoundPlayer attakSound;
	CSoundPlayer deadSound;
};