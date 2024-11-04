#include "stdafx.h"
#include "Map/Map.h"
#include "Enemy.h"
#include "player.h"

//static For DOG CALL
CVector Enemy::saveCurrentPos = {0, 0};
bool Enemy::isdogAllerted = false;

Enemy::Enemy(Map& map, Player& p, CSprite& eSprite, CVector initPos, int type)
	: PathFinder(map), DecisionMaker(map), player(p), map(map)
{
	enemySprite = &eSprite;
	enemyType = type;
	enemySprite->SetPosition(initPos);
	enemySprite->SetDirection(90);
	enemySprite->SetSpeed(0);
	enemySprite->SetAnimation("Idle", 6);
	enemySprite->SetStatus(PATROL);

	IsDead = false;
	isPatroling = false;
	isEnemyStaticPos = false;
	positionHoldTimer = attackDellayTimer = 0;
	damage = 20;
	currentHp = maxHp = 100;
	enemySpeed = 40;

 	dogAlarma = false;
	isdogAllerted = false;

	oneTimeAlert = false;
	catCallForAlert = false;

	humanAlarma = false;
 

	bool isInCallForHelp = false;
	if (type == HUMAN || type == DOG)
	{
		enemySprite->SetStatus(IDLE);
		isEnemyStaticPos = true;
		saveStaticPos = initPos;
		enemySpeed = 100;
		damage = 11;
	}
}

void Enemy::Update(float time)
{
	if (IsDead) return;

	Dialogbox::Update(time);
	EnemyInterface();
	Animation();
	enemySprite->Update(time);

	bool canSee = DecisionMaker::SpotPlayer(player.playerSprite->GetPos(), *enemySprite, enemyType);
	if (player.isPlayerHidden) canSee = false;

	//DOG CALL TO LAST PLAYER POS
	if (isdogAllerted && enemyType == DOG)
	{
		if (canSee)
			isdogAllerted = false;
		else
			DogAlert(time);
		return;
	}

	//HUMAN CALL TO SWITCH LIGHT POSITION
	if ((!map.globalLight && enemyType == HUMAN))
		AISwitchLightOn(time);
		
	//CAT CALL FOR HELP IF LOW HP
	if (currentHp < 80 && !oneTimeAlert && (enemyType != DOG && enemyType != HUMAN))
		catCallForAlert = true;

	if (catCallForAlert)
	{
		if (canSee && oneTimeAlert)
			catCallForAlert = false;
		else
			callForAlert(time);
		return;
	}

	Attack(time);//ATTACK
	Chassing(time); //CHASE

	//HUMAN AND DOG RETURN TO STATIC POS IF NO OTHER ACTION REQRED(ATTACK /CHASSING)
	if (enemySprite->GetStatus() == RETURNTOPOS && !canSee)
		movingToPoint(time);
	Patroling(time);		
}

void Enemy::Chassing(float gametime)
{
	bool canSee = DecisionMaker::SpotPlayer(player.playerSprite->GetPos(), *enemySprite, enemyType);
 
	if (player.isPlayerHidden) 
		canSee = false;
 
	if (!currentWaypoints.empty() && enemySprite->GetStatus() == CHASE)
	{
		CVector destinationToFisrtWP = currentWaypoints.front() - enemySprite->GetPosition(); // directional vector
		// Start moving toward the first waypoint if player isn't moving
		if (enemySprite->GetSpeed() < 1 || canSee)
		{
			enemySprite->SetDirection(destinationToFisrtWP);
			enemySprite->SetRotation(enemySprite->GetDirection());
			enemySprite->SetSpeed(enemySpeed * 2);
			if(Distance(enemySprite->GetPos(), player.playerSprite->GetPos()) > 100)
				ShowDialogBox("I CAN SEE YOU!", gametime);
		}
	 
		// Stop on the arriving to dest
		if ((Dot(enemySprite->GetVelocity(), destinationToFisrtWP) < 0))
		{
			currentWaypoints.erase(currentWaypoints.begin());
			enemySprite->SetVelocity(0, 0);
			if (currentWaypoints.empty() && !canSee)
				playerChasingLastSpot = false;
		}
	}

	float distance = Distance(enemySprite->GetPos(), player.playerSprite->GetPos());
	if (canSee && distance < 300 && distance > 50)
	{
		enemySprite->SetStatus(CHASE);
		currentWaypoints.clear();
		currentWaypoints.push_back(player.playerSprite->GetPos());
		playerChasingLastSpot = true;
	}


	else if (!isEnemyStaticPos && enemySprite->GetStatus() == CHASE && !playerChasingLastSpot)
	{
		//cout << "WHERE PLAYER HAS GONE, WELL WILL GO BACK TO PATROL" << endl;
		if (Distance(enemySprite->GetPos(), player.playerSprite->GetPos()) > 100)
			ShowDialogBox("WHERE IS HE?!", gametime);
		isPatroling = false; // to reset patroling waypoints and start from new pos
		enemySprite->SetStatus(PATROL);
		positionHoldTimer = 0;
	}
	else if (isEnemyStaticPos && enemySprite->GetStatus() == CHASE)
	{
		currentWaypoints = PathFinder::Move(saveStaticPos.GetX(), saveStaticPos.GetY(), enemySprite->GetPos(), false);
		enemySprite->SetStatus(RETURNTOPOS);
	}
	
}

void Enemy::Attack(float time)
{
	bool canSee = DecisionMaker::SpotPlayer(player.playerSprite->GetPos(), *enemySprite, enemyType);
	if (player.isPlayerHidden) canSee = false;

	if (enemySprite->GetStatus() == CHASE && Distance(enemySprite->GetPos(), player.playerSprite->GetPos()) < 50)
	{
		enemySprite->SetStatus(ATTACK);
		ShowDialogBox("ANOTHER TOY!", time);
		enemySprite->SetVelocity(0, 0);
	}
		
	else if (enemySprite->GetStatus() == ATTACK && (Distance(enemySprite->GetPos(), player.playerSprite->GetPos()) > 50 || !canSee))
		enemySprite->SetStatus(CHASE);
	
	if (enemySprite->GetStatus() == ATTACK && attackDellayTimer < time)
	{
 
		attackDellayTimer = time + 1500;
		if (enemyType == CAT) attakSound.Play("cat.wav",0);
		else if(enemyType == DOG) attakSound.Play("dogBark.wav",0);
		else if (enemyType == HUMAN) attakSound.Play("human.wav",0);
		player.GettingDamage(damage);
	}	
}

void Enemy::GettingDamage(float DamageAmount)
{
	cout << "getting Daamage";
	currentHp -= DamageAmount;
	if (currentHp <= 0)
	{
		IsDead = true;
		deadSound.Play("EnemyDeath.wav", 0);
	}
}


void Enemy::Patroling(float time)
{
	if (enemySprite->GetStatus() == PATROL)
	{
		if (!isEnemyStaticPos && !isPatroling)
		{
			cout << "Generate new Patrol Point" << endl;
			ShowDialogBox("PATROLING!", time);
			isPatroling = true;
			patrolVectorList.clear();
			currentWaypoints.clear();
			patrolVectorList = PathFinder::GenerateAiPatrolPoints(enemySprite->GetPos());
			currentWaypoints = patrolVectorList;
			positionHoldTimer = 0;
		}

		if (!currentWaypoints.empty() && positionHoldTimer == 0)
		{
			CVector destinationToFisrtWP = currentWaypoints.front() - enemySprite->GetPosition();
			if (enemySprite->GetSpeed() < 1)
			{
				enemySprite->SetSpeed(enemySpeed);
				enemySprite->SetDirection(destinationToFisrtWP);
				enemySprite->SetRotation(enemySprite->GetDirection() - 0);
			}

 
			if (Dot(enemySprite->GetVelocity(), destinationToFisrtWP) < 0)
			{
				enemySprite->SetVelocity(0, 0);
				positionHoldTimer = 1500 + time;
			}
		}

		if (positionHoldTimer != 0 && positionHoldTimer < time)
		{
			currentWaypoints.erase(currentWaypoints.begin());
			enemySprite->SetVelocity(0, 0);
			positionHoldTimer = 0;
	
			if (currentWaypoints.empty())
				currentWaypoints = patrolVectorList;
		}
	}
}

void Enemy::Draw(CGraphics* g)
{
	if (IsDead) 
		return;
	enemySprite->Draw(g);
	enemyHpBarRect->Draw(g);
	Dialogbox::Draw(g, CVector(enemySprite->GetX(), enemySprite->GetTop() + 25));
}

void Enemy::ifwaipointNotEmptyGoto()
{
	CVector anyChanges = currentWaypoints[0];
	currentWaypoints = PathFinder::PathSmoothing(currentWaypoints, *enemySprite);
	CVector destinationToFisrtWP = currentWaypoints.front() - enemySprite->GetPosition();

	//if there was any changes after PathSmoothing, reset speed to set new direction bellow
	if (anyChanges != currentWaypoints[0])
		enemySprite->SetVelocity(0, 0);

	// Start moving toward the first waypoint if player isn't moving
	if (enemySprite->GetSpeed() < 1)
	{
		enemySprite->SetSpeed(120);
		enemySprite->SetDirection(destinationToFisrtWP);
		enemySprite->SetRotation(enemySprite->GetDirection() - 0);
	}
}

Enemy::~Enemy()
{
	patrolVectorList.clear();
	currentWaypoints.clear();
	delete enemyHpBarRect;
	delete enemySprite;
}

void Enemy::EnemyInterface()
{
	float baseHpBarWidth = 70;
	float hpBarSize = baseHpBarWidth * (currentHp / maxHp);
	if (hpBarSize < 0) hpBarSize = 0;
	enemyHpBarRect->SetSize(hpBarSize, 12);
	enemyHpBarRect->SetPosition(enemySprite->GetX(), enemySprite->GetTop() + 10);
}

void Enemy::Animation()
{
	if (lastState == enemySprite->GetStatus())
		return;
	enemySprite->SetAnimation("Idle", 17);
	
	if (enemySprite->GetStatus() == PATROL || enemySprite->GetStatus() == CHASE || enemySprite->GetStatus() ==  RETURNTOPOS) {
		enemySprite->SetAnimation("Walk", 8);

	}
	lastState = enemySprite->GetStatus();
}

void Enemy::callForAlert(float gameTime)
{
	if (!isInCallForHelp)
	{
		ShowDialogBox("HELP, I NEED IT!", gameTime);
		currentWaypoints.clear();
		saveCurrentPos = enemySprite->GetPos();
		currentWaypoints = PathFinder::Move(1200, 1200, enemySprite->GetPos(), false);
		isInCallForHelp = true;
	}
 
	if (!currentWaypoints.empty())
	{
		ifwaipointNotEmptyGoto();
		// Stop on the arriving to dest
		if ((enemySprite->GetPosition() - currentWaypoints.back()).Length() < 30)
		{
			currentWaypoints.clear();
			enemySprite->SetVelocity(0, 0);

			//if cat runs for a dog and comes back where mouse was , reset back to patrol
			if (oneTimeAlert)
			{
				isPatroling = false;  
				enemySprite->SetStatus(PATROL);
				positionHoldTimer = 0;
				currentWaypoints.clear();
				enemySprite->SetVelocity(0, 0);
				
				catCallForAlert = false;
			}
			//if cat rans for a dog, alerts the dog , and sets movement back to where mouse was
			if (!oneTimeAlert)
			{
				//only call for dog when it's not allerted, so it's returned to spawn point
				if (!isdogAllerted)
				{
					isdogAllerted = true;
					currentWaypoints = PathFinder::Move(saveCurrentPos.GetX(), saveCurrentPos.GetY(), enemySprite->GetPos(), false);
				}
				
				oneTimeAlert = true;
				
			}
		}
	}
}

void Enemy::DogAlert(float gameTime)
{
	if (!dogAlarma)
	{
		ShowDialogBox("SHOW THE WAY!", gameTime);
		currentWaypoints.clear();
		currentWaypoints = PathFinder::Move(saveCurrentPos.GetX(), saveCurrentPos.GetY(), enemySprite->GetPos(), false);
		dogAlarma = true;
	}

	if (!currentWaypoints.empty())
	{
		ifwaipointNotEmptyGoto();
		// Stop on the arriving to dest
		if ((enemySprite->GetPosition() - currentWaypoints.back()).Length() < 30)
		{
			positionHoldTimer = 2000 + gameTime;
			ShowDialogBox("STUPID CAT!", gameTime);
			currentWaypoints.clear();
			currentWaypoints = PathFinder::Move(1200, 1200, enemySprite->GetPos(), false);

			isdogAllerted = false;
			enemySprite->SetStatus(RETURNTOPOS);
			enemySprite->SetVelocity(0, 0);
			
		}
	}
}

void Enemy::AISwitchLightOn(float gameTime)
{
	if (!humanAlarma)
	{
		currentWaypoints.clear();
		currentWaypoints = PathFinder::Move(1850, 120, enemySprite->GetPos(), false);
		humanAlarma = true;
		enemySprite->SetStatus(IDLE);
	}

	if (!currentWaypoints.empty())
	{
		ifwaipointNotEmptyGoto();
		// Stop on the arriving to dest
		if ((enemySprite->GetPosition() - currentWaypoints.back()).Length() < 30)
		{
			positionHoldTimer = 2000 + gameTime;
			map.globalLight = true;
			lightOn.Play("lightOn.wav", 0);
			humanAlarma = false;
			currentWaypoints.clear();
			currentWaypoints = PathFinder::Move(1720, 1410, enemySprite->GetPos(), false);
			enemySprite->SetStatus(RETURNTOPOS);
			enemySprite->SetVelocity(0, 0);

		}
	}
}

void Enemy::movingToPoint(float gameTime)
{
	if (positionHoldTimer > gameTime) return;
	ifwaipointNotEmptyGoto();
 
	// Stop on the arriving to dest
	if ((enemySprite->GetPosition() - currentWaypoints.back()).Length() < 50)
	{
		enemySprite->SetStatus(IDLE);
		currentWaypoints.clear();
		enemySprite->SetVelocity(0, 0);
		isdogAllerted = false;
		dogAlarma = false;
	}
}