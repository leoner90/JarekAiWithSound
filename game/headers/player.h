#pragma once
#include "PathFinder.h"
#include "UI.h"

class Map;
class Enemy;
class Player  : public PathFinder, public UI
{
public:
	
	Player(Map& map);
	~Player();
	void gameInit();
	void Draw(CGraphics* g, float time);
	void Update(float time, std::vector<Enemy*>& enemiesRef);
	
	void OnRButtonDown(Uint16 x, Uint16 y, float gameTime);
	void MoveToWaypoint();
	void Animation();
	void Attack(float time);
	void GettingDamage(float DamageAmount);
	void OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode, float time);
	void buffResets(float time);
	void mpRegen(float time);
	CSprite* playerSprite;
	bool IsCheeseObtained;
 
	bool IsDead;
	bool isGameWon;

	CSprite mainGoalCheese;
	float playerSpeedOrigin;
	bool isPlayerHasted;
	float speedBuffTimer;

	bool isPlayerHidden;
	float hideBuffTimer;


	bool isAttacking;

private:
	int newState;
	std::vector <CVector> currentWaypoint;
	int lastState;
	enum playerStates {IDLE, WALK, ATTACK};


	float currentHp, maxHp;
	float currentMp, maxMp;
	float damage;
	float attackDellayTimer;


	std::vector<Enemy*> AllEnemies;

	//delta Time
	float previousTime = 0.0f;
	float deltaTime;
	float attackAnimationTimer;
	Map& map;

	CVector saveLastPlayerVelocityVector;
	//bufs flags
	std::vector<bool> buffFlags;
	std::vector<float> buffReaminingTime;

	CSprite movementPos;
	float showMovementPosTimer;

	CSoundPlayer  lightOff;
	CSoundPlayer  obtainCheese;
	CSoundPlayer  hideSkill;
	CSoundPlayer  speedSkill;
	CSoundPlayer  attackSkill;
	CSoundPlayer  hurtSound;
	
 
};
 