#include "stdafx.h"
#include "player.h"
#include "Map/Map.h"
#include "Enemy.h"

/*********** CONSTRUCTOR ***********/
Player::Player(Map& map) : PathFinder(map), map(map)
{
	//SPRITES
	mainGoalCheese.LoadImage("Cheese.png");
	mainGoalCheese.SetImage("Cheese.png");
	mainGoalCheese.SetSize(50, 50);

	//player
	playerSprite = new CSprite;
	playerSprite->AddImage("player.png", "Idle", 31, 1, 0, 0, 14, 0);
	playerSprite->AddImage("player.png", "Walk", 31, 1, 16, 0, 23, 0);
	playerSprite->AddImage("player.png", "Attack",  31, 1, 24, 0, 30, 0);
	playerSprite->SetAnimation("Idle");
	damage = 40;

	//mouse
	movementPos.LoadImage("movementPos.png");
	movementPos.SetImage("movementPos.png");
	movementPos.SetSize(35, 35);
	movementPos.SetColorKey(CColor::White());
}

/*********** RESET ON GAME START ***********/
void Player::gameInit()
{
	mainGoalCheese.SetPosition(1800, 1420);
	playerSprite->SetStatus(IDLE);
	playerSprite->SetPosition(380, 100);
	playerSprite->SetSpeed(0);
	playerSprite->SetDirection(90);

	//condition resets
	IsCheeseObtained = false;
	IsDead = false;
	isGameWon = false;

	//stats resets
	currentHp = maxHp = 100;
	currentMp = maxMp = 100;
	playerSpeedOrigin = 100;
	UI::SetHpBar(1);
	UI::SetMpBar(1);

	//buffs resets
	speedBuffTimer = 0;
	isPlayerHasted = false;
	isPlayerHidden = false;
	hideBuffTimer = 0;

	//attack resets
	attackAnimationTimer = 0;
	attackDellayTimer = 0;

	//reset movement conditons
	if (!currentWaypoint.empty())
		currentWaypoint.clear();
	showMovementPosTimer = 0;
}

/*********** UPDATE ***********/
void Player::Update(float time, std::vector<Enemy*>& enemiesRef)
{
	if (IsDead) return;

	AllEnemies = enemiesRef;
	
	//ATTACK
	if (isAttacking) Attack(time);
	if (attackAnimationTimer < time && playerSprite->GetStatus() == ATTACK)  	
		playerSprite->SetStatus(IDLE);

	//MOVEMENT AND ANIMATION
	MoveToWaypoint(); 
	Animation();
	playerSprite->Update(time);

	//GAME WIN CONDTIONS
	if (mainGoalCheese.HitTest(playerSprite) && !IsCheeseObtained)
	{
		obtainCheese.Play("obtainCheese.wav", 1);
		IsCheeseObtained = true;
	}
		
	if (IsCheeseObtained && playerSprite->GetX() > 0 && playerSprite->GetX() < 400 && playerSprite->GetY() > 0 && playerSprite->GetY() < 150)
		isGameWon = true;

	//BUFFS & STATS
	buffResets(time); 
	mpRegen(time); // mp regen
	buffFlags = { isPlayerHidden, isPlayerHasted, IsCheeseObtained };
	buffReaminingTime = { time - hideBuffTimer, time - speedBuffTimer };
}

/*********** DRAW ***********/
void Player::Draw(CGraphics* g, float time)
{
	if (IsDead) return;

	// Draw Path
	if (!currentWaypoint.empty())
	{
		for (int i = 0; i < currentWaypoint.size(); i++)
		{
			CVector segmentStart = (i == 0) ? playerSprite->GetPos() : currentWaypoint[i - 1];
			CVector segmentEnd = currentWaypoint[i];

			CVector directionalV = segmentEnd - segmentStart;
			int numSteps = 10;
			CVector step = directionalV.Normalise() * (directionalV.Length() / numSteps);

			for (int j = 0; j <= numSteps; j++)
			{
				CVector pointAlongLine = segmentStart + step * j;
				g->FillCircle(pointAlongLine, 2, CColor::DarkRed());
			}
		}
	}

	//Draw cursor where to go
	if (showMovementPosTimer > time)
		movementPos.Draw(g);

	//player Draw
	playerSprite->Draw(g);

	//if in test mode Draw Nodes
	for (auto obj : PathFinder::GetTestNodes()) 
		obj->Draw(g);

	//UI - SAVE OFFSET -> DRAW UI -> Resset OFFSET
	CVector SaveOfset = g->GetScrollPos();
	g->SetScrollPos(0, 0);
	UI::DrawUI(g, buffFlags, buffReaminingTime);
	g->SetScrollPos(SaveOfset);

	//Cheese Draw On the map if not yet obtained
	if (!IsCheeseObtained)
		mainGoalCheese.Draw(g);
}

/*********** MOVE TO ***********/
void Player::MoveToWaypoint()
{
	if (!currentWaypoint.empty()) 
	{
		CVector anyChanges = currentWaypoint[0];
		currentWaypoint = PathFinder::PathSmoothing(currentWaypoint, playerSprite->GetPos(), playerSprite);
		CVector destinationToFisrtWP = currentWaypoint.front() -  playerSprite->GetPosition();

		//if there was any changes after PathSmoothing, reset speed to set new direction bellow
		if (anyChanges != currentWaypoint[0])
			playerSprite->SetVelocity(0, 0);
			
		// Start moving toward the first waypoint if player isn't moving
		if (playerSprite->GetSpeed() < 1)
		{
			playerSprite->SetSpeed(playerSpeedOrigin);
			playerSprite->SetStatus(WALK);
			playerSprite->SetDirection(destinationToFisrtWP);
			playerSprite->SetRotation(playerSprite->GetDirection() - 0);
		}

		if (Distance(playerSprite->GetPos(), currentWaypoint.back()) <= 25)
		{
			playerSprite->SetStatus(IDLE);
			currentWaypoint.clear();
			playerSprite->SetVelocity(0, 0);
		}
	}

	//speed buff apply
	if(isPlayerHasted && playerSprite->GetSpeed() > 0)
		playerSprite->SetSpeed(playerSpeedOrigin * 2);
	else if(!isPlayerHasted && playerSprite->GetSpeed() > 0)
		playerSprite->SetSpeed(playerSpeedOrigin);
}

/*********** ATTACK ENEMY ***********/
void Player::Attack(float time)
{
	isAttacking = false;
	attackAnimationTimer = time + 500;
	playerSprite->SetStatus(ATTACK);
	currentWaypoint.clear();
	playerSprite->SetVelocity(0, 0);

	//attack electricalPanel
	if (playerSprite->GetX() > 1840 && playerSprite->GetY() < 150 && map.GetGlobalLightOn())
	{
		lightOff.Play("lightOff.wav", 0);
		isPlayerHidden = true;
		map.SetGlobalLight(false);
	}
		
	//all enemies on front + distance less then 80 getting damage
	for (auto enemy : AllEnemies)
	{
		if (Distance(enemy->enemySprite->GetPos(), playerSprite->GetPos()) > 70) 
			continue;

		//is enemy on fron of player sprite, depends on sprite rotation
		float playerRotation = playerSprite->GetRotation() * (M_PI / 180.0f); // to radians
		CVector playerForward(sin(playerRotation), cos(playerRotation)); // player Forward Vector
		CVector directionToEnemy = enemy->enemySprite->GetPos() - playerSprite->GetPos(); // directional Vector
		
		float dotProduct = Dot(playerForward, directionToEnemy.Normalize());

		bool isPlayerFacingEnemy = dotProduct >= 0.5f; 

		//if facing enemy
		if (isPlayerFacingEnemy)
			enemy->GettingDamage(50);
	}
}

/*********** GETTING DAMAGE ***********/
void Player::GettingDamage(float DamageAmount)
{
	hurtSound.Play("hurtSound.wav", 0);
	currentHp -= DamageAmount;
	if (currentHp <= 0)
	{
		IsDead = true;
		UI::SetHpBar(0);
	}
	else
		UI::SetHpBar(currentHp / maxHp);
}

/*********** BUFFS AND ENERGY RESETS ***********/
void Player::buffResets(float time)
{
	// timers reset - extra condition to speed up check?
	if (isPlayerHasted && speedBuffTimer < time)  isPlayerHasted = false;
	if (isAttacking && attackDellayTimer < time)  isAttacking = false;
	if (isPlayerHidden && hideBuffTimer < time && map.GetGlobalLightOn()) 
		isPlayerHidden = false;
}

void Player::mpRegen(float time)
{
	if (previousTime == 0.0f)
		previousTime = time;
	else
	{
		deltaTime = (time - previousTime) / 1000.f;
		previousTime = time;
	}
	if (currentMp < 100)
	{
		currentMp += 5 * deltaTime;
		if (currentMp > 100) currentMp = 100;
		UI::SetMpBar(currentMp / maxMp);
	}
}

/*********** PLAYER ANIMATION ***********/
void Player::Animation()
{
	if (lastState == playerSprite->GetStatus())
		return;

	playerSprite->SetAnimation("Idle", 12);

	if (playerSprite->GetStatus() == WALK)
		playerSprite->SetAnimation("Walk", 8);

	else if (playerSprite->GetStatus() == ATTACK)
		playerSprite->SetAnimation("Attack", 16);

	lastState = playerSprite->GetStatus();
}

/*********** KEYBOARD ACTIONS ***********/
void Player::OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode, float time)
{
	if (sym == SDLK_q && currentMp >= 10 && !isAttacking && attackDellayTimer < time)
	{
		currentMp -= 10;
		attackDellayTimer = time + 1000;
		isAttacking = true;
		attackSkill.Play("playerAttack.wav", 0);
	}

	if (sym == SDLK_w && currentMp >= 20 && !isPlayerHidden)
	{
		currentMp -= 50;
		hideBuffTimer = time + 3000;
		isPlayerHidden = true;
		hideSkill.Play("vanish.wav", 0);
	}

	if (sym == SDLK_e && currentMp >= 20 && !isPlayerHasted)
	{
		currentMp -= 25;
		speedBuffTimer = time + 2000;
		isPlayerHasted = true;
		speedSkill.Play("speedSkill.wav", 0);
	}
}
 
/*********** MOUSE ACTIONS ***********/
void Player::OnRButtonDown(Uint16 x, Uint16 y, float gameTime)
{
	playerSprite->SetVelocity(0, 0);
	playerSprite->SetStatus(IDLE);
	currentWaypoint = PathFinder::PathGenerator(x,y, playerSprite->GetPos(),true);
	if (!currentWaypoint.empty())
	{
		movementPos.SetPos(currentWaypoint.back());
		showMovementPosTimer = gameTime + 500;
	}
}

/*********** GETTERS ***********/
CSprite* Player::getPlayerSprite()
{
	return playerSprite;
}

bool Player::IsPlayerDead()
{
	return IsDead;
}

bool Player::IsGameWon()
{
	return isGameWon;
}

bool Player::IsPlayerVisible()
{
	return isPlayerHidden;
}