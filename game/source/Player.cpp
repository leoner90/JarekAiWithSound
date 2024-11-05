#include "stdafx.h"
#include "player.h"
#include "Map/Map.h"
#include "Enemy.h"

Player::Player(Map& map) : PathFinder(map), map(map)
{
	mainGoalCheese.LoadImage("Cheese.png");
	mainGoalCheese.SetImage("Cheese.png");
	mainGoalCheese.SetSize(50, 50);

	//player
	playerSprite = new CSprite;
	playerSprite->AddImage("player.png", "Idle", 31, 1, 0, 0, 14, 0);
	playerSprite->AddImage("player.png", "Walk", 31, 1, 16, 0, 23, 0);
	playerSprite->AddImage("player.png", "Attack",  31, 1, 24, 0, 30, 0);
	playerSprite->SetAnimation("Idle");
	damage = 50;

	//mouse
	movementPos.LoadImage("movementPos.png");
	movementPos.SetImage("movementPos.png");
	movementPos.SetSize(35, 35);
	movementPos.SetColorKey(CColor::White());
}

Player::~Player()
{
}

void Player::gameInit()
{
	mainGoalCheese.SetPosition(1800, 1420);
	playerSprite->SetStatus(IDLE);
	playerSprite->SetPosition(350, 250);
	playerSprite->SetSpeed(0);
	playerSprite->SetDirection(90);

	//buss and stats reset
	IsCheeseObtained = false;
	IsDead = false;
	currentHp = maxHp = 100;
	currentMp = maxMp = 100;

	playerSpeedOrigin = 100;
	speedBuffTimer = 0;
	isPlayerHasted = false;

	isPlayerHidden = false;
	hideBuffTimer = 0;

	isGameWon = false;

	UI::SetHpBar(1);
	UI::SetMpBar(1);
	attackDellayTimer = 0;
	if (!currentWaypoint.empty())
		currentWaypoint.clear();

	saveLastPlayerVelocityVector = { 0,100 };


	showMovementPosTimer = 0;
 
}

void Player::Update(float time, std::vector<Enemy*>& enemiesRef)
{
	if (IsDead) return;
	if (playerSprite->GetSpeed() != 0)
		saveLastPlayerVelocityVector = playerSprite->GetVelocity();
	AllEnemies = enemiesRef;
	mpRegen(time);
	if (isAttacking)
		Attack(time);
	buffResets(time);
	MoveToWaypoint();

	Animation();
	playerSprite->Update(time);

	if (mainGoalCheese.HitTest(playerSprite) && !IsCheeseObtained)
	{
		obtainCheese.Play("obtainCheese.wav", 1);
		IsCheeseObtained = true;
	}
		
	if (IsCheeseObtained && playerSprite->GetX() > 0 && playerSprite->GetX() < 400 && playerSprite->GetY() > 0 && playerSprite->GetY() < 150)
		isGameWon = true;


	buffFlags = { isPlayerHidden, isPlayerHasted, IsCheeseObtained };
	buffReaminingTime = { time - hideBuffTimer, time - speedBuffTimer };
}

void Player::Draw(CGraphics* g, float time)
{
	if (IsDead) return;

	//cursor where to go
	if (showMovementPosTimer > time)
		movementPos.Draw(g);

	playerSprite->Draw(g);


	for (auto obj : testNodes) 
		obj->Draw(g);

	if (!currentWaypoint.empty())
			g->DrawLine(CVector(playerSprite->GetRight(), playerSprite->GetBottom()), currentWaypoint[0], 4, CColor::Red());

	CVector SaveOfset = g->GetScrollPos();
	if(!IsCheeseObtained)
		mainGoalCheese.Draw(g);

	

	g->SetScrollPos(0, 0);
	UI::DrawUI(g, buffFlags, buffReaminingTime);
	g->SetScrollPos(SaveOfset);
}

void Player::MoveToWaypoint()
{
	if (!currentWaypoint.empty()) 
	{
		CVector anyChanges = currentWaypoint[0];
		currentWaypoint = PathFinder::PathSmoothing(currentWaypoint, *playerSprite);
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

	
		// Stop on the arriving to dest
		if ((playerSprite->GetPosition() - currentWaypoint.back()).Length() < 30)
		{
			playerSprite->SetStatus(IDLE);
			currentWaypoint.clear();
			playerSprite->SetVelocity(0, 0);
		}

		
	
		/*
		CVector v = currentWaypoint.front() - playerSprite->GetPosition();
		if (Dot(playerSprite->GetVelocity(), v) < 0)
		{
			currentWaypoint.erase(currentWaypoint.begin());
			playerSprite->SetStatus(IDLE);
		 
			playerSprite->SetVelocity(0, 0);
		}
		*/
	}

	//baffs
	if(isPlayerHasted && playerSprite->GetSpeed() > 0)
		playerSprite->SetSpeed(playerSpeedOrigin * 2);
	else if(!isPlayerHasted && playerSprite->GetSpeed() > 0)
		playerSprite->SetSpeed(playerSpeedOrigin);
}


void Player::Animation()
{
	if (lastState == playerSprite->GetStatus())
		return;

	playerSprite->SetAnimation("Idle", 12);
	if (playerSprite->GetStatus() == WALK) {
		playerSprite->SetAnimation("Walk", 8);
	}
	lastState = playerSprite->GetStatus();

}

void Player::Attack(float time)
{
	isAttacking = false;

	//attack electricalPanel
	if (playerSprite->GetX() > 1840 && playerSprite->GetY() < 150 && map.globalLight)
	{
		lightOff.Play("lightOff.wav", 0);
		isPlayerHidden = true;
		map.globalLight = false;
	}
		
 
	//all enemies on front + distance less then 80 getting damage
	for (auto enemy : AllEnemies)
	{
		if (Distance(enemy->enemySprite->GetPos(), playerSprite->GetPos()) > 60) 
			continue;

		//is enemy on fron of player sprite, depends on sprite rotation
		float playerRotation = playerSprite->GetRotation() * (M_PI / 180.0f); // to radians
		CVector playerForward(sin(playerRotation), cos(playerRotation)); // player Forward Vector
		CVector directionToEnemy = enemy->enemySprite->GetPos() - playerSprite->GetPos(); // directional Vector
		
		float dotProduct = Dot(playerForward, directionToEnemy.Normalize());

		cout << dotProduct;
		bool isPlayerFacingEnemy = dotProduct >= 0.5f; 

		//if facing each other and distance < 50
		if (isPlayerFacingEnemy)
			enemy->GettingDamage(50);
	}
}

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

void Player::OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode, float time)
{
	;
	;
	;
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
		hideBuffTimer = time + 2000;
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

void Player::buffResets(float time)
{
	// timers reset - extra condition to speed up check?
	if (isPlayerHasted && speedBuffTimer < time)  isPlayerHasted = false;
	if (isAttacking && attackDellayTimer < time)  isAttacking = false;
	if (isPlayerHidden && hideBuffTimer < time && map.globalLight )  isPlayerHidden = false;
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

 
 
void Player::OnRButtonDown(Uint16 x, Uint16 y, float gameTime)
{
	playerSprite->SetVelocity(0, 0);
	playerSprite->SetStatus(IDLE);
	currentWaypoint = PathFinder::Move(x,y, playerSprite->GetPos(),true);
	if (!currentWaypoint.empty())
	{
		movementPos.SetPos(currentWaypoint.back());
		showMovementPosTimer = gameTime + 500;
	}
		
}