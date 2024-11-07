#pragma once
#include "PathFinder.h"
#include "UI.h"

//Forward Declaration
class Map;
class Enemy;

class Player  : public PathFinder, public UI
{
public:
	//Main Functions
	Player(Map& map);
	void gameInit();
	void Draw(CGraphics* g, float time);
	void Update(float time, std::vector<Enemy*>& enemiesRef);

	//Mouse And Keyboard Actions
	void OnRButtonDown(Uint16 x, Uint16 y, float gameTime);
	void OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode, float time);

	//Move To A point
	void MoveToWaypoint();

	//Animation
	void Animation();

	//Combat
	void Attack(float time);
	void GettingDamage(float DamageAmount);

	//Buffs and Mp regen
	void buffResets(float time);
	void mpRegen(float time);

	//Getters
	CSprite* getPlayerSprite();
	bool IsPlayerDead();
	bool IsGameWon();
	bool IsPlayerVisible();

private:
	//Current Path waypoint
	std::vector <CVector> currentWaypoint;

	//SPRITES
	CSprite mainGoalCheese;
	CSprite* playerSprite;

	//Movement Sprite and Times
	CSprite movementPos;
	float showMovementPosTimer;

	//Animation
	int lastState;
	
	//STATES
	enum playerStates {IDLE, WALK, ATTACK};

	//STATS 
	float currentHp, maxHp;
	float currentMp, maxMp;
	float playerSpeedOrigin;

	//COMBAT
	float damage;
	float attackDellayTimer;
	bool isAttacking;

	//delta Time
	float previousTime = 0.0f;
	float deltaTime;
	float attackAnimationTimer;

	//Map reference
	Map& map;

	//Enemies Reference 
	std::vector<Enemy*> AllEnemies;

	//bufs and flags
	float hideBuffTimer;
	std::vector<bool> buffFlags;
	std::vector<float> buffReaminingTime;
	bool isPlayerHasted;
	float speedBuffTimer;

	//Game and Player Conditions
	bool IsCheeseObtained;
	bool IsDead;
	bool isGameWon;
	bool isPlayerHidden;

	//SOUNDS
	CSoundPlayer  lightOff;
	CSoundPlayer  obtainCheese;
	CSoundPlayer  hideSkill;
	CSoundPlayer  speedSkill;
	CSoundPlayer  attackSkill;
	CSoundPlayer  hurtSound;
};
 