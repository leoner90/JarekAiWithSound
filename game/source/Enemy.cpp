#include "stdafx.h"
#include "Map/Map.h"
#include "Enemy.h"
#include "player.h"

//static For DOG CALL
CVector Enemy::saveCurrentPos = { 0, 0 };
bool Enemy::isdogAllerted = false;

/*********** CONSTRUCTOR ***********/
Enemy::Enemy(Map& map, Player& p, CSprite& eSprite, CVector initPos, int type)
	: PathFinder(map), DecisionMaker(map), player(p), map(map)
{
	enemySprite = &eSprite;
	enemyType = type;
	enemySprite->SetPosition(initPos);
	enemySprite->SetDirection(90);
	enemySprite->SetSpeed(0);
	enemySprite->ClearColorKey();
	enemySprite->SetAnimation("Idle", 8);
	enemySprite->SetStatus(PATROL);

	//resets
	IsDead = false;
	isPatroling = false;
	isEnemyStaticPos = false;
	positionHoldTimer = attackDellayTimer = 0;
	dogAlarma = false;
	isdogAllerted = false;
	oneTimeAlert = false;
	catCallForAlert = false;
	humanAlarma = false;

	//stats
	damage = 10;
	currentHp = maxHp = 100;
	enemySpeed = 40;

	bool isInCallForHelp = false;
	if (type == HUMAN || type == DOG)
	{
		enemySprite->SetStatus(IDLE);
		isEnemyStaticPos = true;
		saveStaticPos = initPos;
		enemySpeed = 100;
		damage = 40;
		currentHp = maxHp = 300;
	}
}

/*********** DESTRUCTOR ***********/
Enemy::~Enemy()
{
	patrolVectorList.clear();
	currentWaypoints.clear();
	delete enemyHpBarRect;
	delete enemySprite;
}

/*********** UPDATE ***********/
void Enemy::Update(float time)
{
	if (IsDead) return;

	Dialogbox::Update(time);
	EnemyInterface();
	Animation(time);
	enemySprite->Update(time);

	//can see the player to REDO :)
	bool canSee = DecisionMaker::SpotPlayer(player.getPlayerSprite()->GetPos(), *enemySprite, enemyType);
	if (player.IsPlayerVisible()) canSee = false;

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
	if ((!map.GetGlobalLightOn() && enemyType == HUMAN))
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

	//basicly rotates cat to player if it's allready seen , to fix a bag that during attack player can just run behind the cat and cat starts patroling
	if (Distance(enemySprite->GetPos(), player.getPlayerSprite()->GetPos()) < 100 && !player.IsPlayerVisible())
		enemySprite->SetDirection(player.getPlayerSprite()->GetPos() - enemySprite->GetPos());


	Attack(time);//ATTACK
	Chassing(time); //CHASE

	//HUMAN AND DOG RETURN TO STATIC POS IF NO OTHER ACTION REQRED(ATTACK /CHASSING)
	if (enemySprite->GetStatus() == RETURNTOPOS && !canSee)
		movingToPoint(time);
	Patroling(time);
}

/*********** DRAW ***********/
void Enemy::Draw(CGraphics* g)
{
	if (IsDead) return;
	enemySprite->Draw(g);
	enemyHpBarRect->Draw(g);
	Dialogbox::Draw(g, CVector(enemySprite->GetX(), enemySprite->GetTop() + 25));
}

/*********** CHASING ***********/
void Enemy::Chassing(float gametime)
{
	bool canSee = DecisionMaker::SpotPlayer(player.getPlayerSprite()->GetPos(), *enemySprite, enemyType);

	if (player.IsPlayerVisible())
		canSee = false;

	if (!currentWaypoints.empty() && enemySprite->GetStatus() == CHASE)
	{
		CVector destinationToFisrtWP = currentWaypoints.front() - enemySprite->GetPosition(); // directional vector
		// Start moving toward the first waypoint 
		if (enemySprite->GetSpeed() < 1 || canSee)
		{
			enemySprite->SetDirection(destinationToFisrtWP);
			enemySprite->SetRotation(enemySprite->GetDirection());
			enemySprite->SetSpeed(enemySpeed * 2);
			if (Distance(enemySprite->GetPos(), player.getPlayerSprite()->GetPos()) > 100)
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
	//START CHASING
	float distance = Distance(enemySprite->GetPos(), player.getPlayerSprite()->GetPos());
	if (canSee && distance < 300 && distance > 70)
	{
		enemySprite->SetStatus(CHASE);
		currentWaypoints.clear();
		currentWaypoints.push_back(player.getPlayerSprite()->GetPos());
		playerChasingLastSpot = true;
	}

	//RETURN TO PATROL
	else if (!isEnemyStaticPos && enemySprite->GetStatus() == CHASE && !playerChasingLastSpot)
	{
		if (Distance(enemySprite->GetPos(), player.getPlayerSprite()->GetPos()) > 100)
			ShowDialogBox("WHERE IS HE?!", gametime);
		isPatroling = false; // to reset patroling waypoints and start from new pos
		enemySprite->SetStatus(PATROL);
		positionHoldTimer = 0;
	}
	// IF STATIC NPC (NO PATROL) RETURN TO STATIC POS
	else if (isEnemyStaticPos && enemySprite->GetStatus() == CHASE && player.IsPlayerVisible())
	{
		currentWaypoints.clear();
		enemySprite->SetVelocity(0, 0);
		currentWaypoints = PathFinder::PathGenerator(saveStaticPos.GetX(), saveStaticPos.GetY(), enemySprite->GetPos(), false);
		enemySprite->SetStatus(RETURNTOPOS);
	}

}

/*********** ATTACK ***********/
void Enemy::Attack(float time)
{
	bool canSee = DecisionMaker::SpotPlayer(player.getPlayerSprite()->GetPos(), *enemySprite, enemyType);
	if (player.IsPlayerVisible()) canSee = false;
	float distance = Distance(enemySprite->GetPos(), player.getPlayerSprite()->GetPos());

	//attack if close
	if (enemySprite->GetStatus() == CHASE && distance < 70 && canSee)
	{
		enemySprite->SetStatus(ATTACK);
		ShowDialogBox("ANOTHER TOY!", time);
		enemySprite->SetVelocity(0, 0);
		currentWaypoints.clear();
	}

	//BACK TO CHASE
	if (enemySprite->GetStatus() == ATTACK && ((Distance(enemySprite->GetPos(), player.getPlayerSprite()->GetPos()) > 70 || player.IsPlayerVisible())))
	{
		currentWaypoints.clear();
		enemySprite->SetStatus(CHASE);
		currentWaypoints.push_back(player.getPlayerSprite()->GetPos());
	}

	//DAMAGE PLAYER
	if (enemySprite->GetStatus() == ATTACK && attackDellayTimer < time && !player.IsPlayerVisible())
	{
		attackDellayTimer = time + 1500;
		if (enemyType == CAT) attakSound.Play("cat.wav", 0);
		else if (enemyType == DOG) attakSound.Play("dogBark.wav", 0);
		else if (enemyType == HUMAN) attakSound.Play("human.wav", 0);
		player.GettingDamage(damage);
	}
}

/*********** PATROLING ***********/
void Enemy::Patroling(float time)
{
	if (enemySprite->GetStatus() == PATROL || (enemySprite->GetStatus() == IDLE && enemyType != DOG && enemyType != HUMAN))
	{
		//IF NO PATROL POIONT -> SET NEW ONES
		if (!isEnemyStaticPos && !isPatroling)
		{
			ShowDialogBox("PATROLING!", time);
			isPatroling = true;
			enemySprite->SetVelocity(0, 0);
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
				enemySprite->SetStatus(IDLE);
			}
		}

		if (positionHoldTimer != 0 && positionHoldTimer < time)
		{
			currentWaypoints.erase(currentWaypoints.begin());
			enemySprite->SetVelocity(0, 0);
			positionHoldTimer = 0;
			enemySprite->SetStatus(PATROL);
			if (currentWaypoints.empty())
				currentWaypoints = patrolVectorList;
		}
	}
}

/*********** GETTING DAMAGE ***********/
void Enemy::GettingDamage(float DamageAmount)
{
	currentHp -= DamageAmount;
	if (currentHp <= 0)
	{
		IsDead = true;
		deadSound.Play("EnemyDeath.wav", 0);
	}
}

/*********** MOVING FORWARD ***********/
void Enemy::ifwaipointNotEmptyGoto()
{
	CVector anyChanges = currentWaypoints[0];
	currentWaypoints = PathFinder::PathSmoothing(currentWaypoints, enemySprite->GetPos(), enemySprite);
	CVector destinationToFisrtWP = currentWaypoints.front() - enemySprite->GetPosition();

	//if there was any changes after PathSmoothing, reset speed to set new direction bellow
	if (anyChanges != currentWaypoints[0])
		enemySprite->SetVelocity(0, 0);

	// Start moving toward the first waypoint if player isn't moving
	if (enemySprite->GetSpeed() < 1)
	{
		enemySprite->SetSpeed(120);
		enemySprite->SetDirection(destinationToFisrtWP);
		enemySprite->SetRotation(enemySprite->GetDirection());
	}
}

/*********** UI ***********/
void Enemy::EnemyInterface()
{
	float baseHpBarWidth = 70;
	float hpBarSize = baseHpBarWidth * (currentHp / maxHp);
	if (hpBarSize < 0) hpBarSize = 0;
	enemyHpBarRect->SetSize(hpBarSize, 12);
	enemyHpBarRect->SetPosition(enemySprite->GetX(), enemySprite->GetTop() + 10);
}

/*********** ANIMATION ***********/
void Enemy::Animation(float time)
{
	if (lastState == enemySprite->GetStatus() || (enemySprite->GetStatus() == RETURNTOPOS && positionHoldTimer > time))
		return;

	enemySprite->SetAnimation("Idle", 8);

	if (enemySprite->GetStatus() == CHASE || enemySprite->GetStatus() == PATROL || enemySprite->GetStatus() == RETURNTOPOS || enemySprite->GetStatus() == WALK)
	{
		int walkFrames = 12;
		if (enemyType == HUMAN)
			walkFrames = 8;
		enemySprite->SetAnimation("Walk", 12);
	}
	else if (enemySprite->GetStatus() == ATTACK)
	{
		int attackFrames = 3;
		if (enemyType == DOG)
			attackFrames = 6;
		else if (enemyType == HUMAN)
			attackFrames = 2;
		enemySprite->SetAnimation("Attack", attackFrames);
	}
	lastState = enemySprite->GetStatus();
}

/*********** CAT CALL FOR ALERT ***********/
void Enemy::callForAlert(float gameTime)
{
	if (!isInCallForHelp)
	{
		ShowDialogBox("HELP, I NEED IT!", gameTime);
		enemySprite->SetStatus(WALK);
		currentWaypoints.clear();
		enemySprite->SetVelocity(0, 0);
		saveCurrentPos = enemySprite->GetPos();
		currentWaypoints = PathFinder::PathGenerator(1200, 1200, enemySprite->GetPos(), false);
		isInCallForHelp = true;
	}

	if (!currentWaypoints.empty())
	{
		ifwaipointNotEmptyGoto();

		// Stop on the arriving to dest
		if (Distance(enemySprite->GetPos(), currentWaypoints.back()) <= 25)
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
					currentWaypoints = PathFinder::PathGenerator(saveCurrentPos.GetX(), saveCurrentPos.GetY(), enemySprite->GetPos(), false);
				}

				oneTimeAlert = true;
			}
		}
	}
}

/*********** DOG ALLERTED ***********/
void Enemy::DogAlert(float gameTime)
{
	if (!dogAlarma)
	{
		ShowDialogBox("SHOW THE WAY!", gameTime);
		enemySprite->SetVelocity(0, 0);
		currentWaypoints.clear();
		enemySprite->SetStatus(WALK);
		currentWaypoints = PathFinder::PathGenerator(saveCurrentPos.GetX(), saveCurrentPos.GetY(), enemySprite->GetPos(), false);
		dogAlarma = true;
	}

	if (!currentWaypoints.empty())
	{
		ifwaipointNotEmptyGoto();

		// Stop on the arriving to dest -  waits and  return back to static
		cout << Distance(enemySprite->GetPos(), currentWaypoints.back()) << endl;
		if (Distance(enemySprite->GetPos(), currentWaypoints.back()) <= 25)
		{
			positionHoldTimer = 2000 + gameTime;
			ShowDialogBox("STUPID CAT!", gameTime);
			currentWaypoints.clear();
			currentWaypoints = PathFinder::PathGenerator(1200, 1200, enemySprite->GetPos(), false);

			isdogAllerted = false;
			enemySprite->SetStatus(RETURNTOPOS);
			enemySprite->SetAnimation("Idle", 8);
			enemySprite->SetVelocity(0, 0);
		}
	}
}

/*********** AI LOGIC IF LIGHT IS OFF ***********/
void Enemy::AISwitchLightOn(float gameTime)
{
	if (!humanAlarma)
	{
		currentWaypoints.clear();
		currentWaypoints = PathFinder::PathGenerator(1850, 120, enemySprite->GetPos(), false);
		humanAlarma = true;
		enemySprite->SetStatus(WALK);
	}

	if (!currentWaypoints.empty())
	{
		ifwaipointNotEmptyGoto();
		// Stop on the arriving to dest
		if (Distance(enemySprite->GetPos(), currentWaypoints.back()) <= 25)
		{
			positionHoldTimer = 2000 + gameTime;
			map.SetGlobalLight(true);
			lightOn.Play("lightOn.wav", 0);
			humanAlarma = false;
			currentWaypoints.clear();
			currentWaypoints = PathFinder::PathGenerator(1720, 1410, enemySprite->GetPos(), false);
			enemySprite->SetStatus(RETURNTOPOS);
			enemySprite->SetAnimation("Idle", 8);
			enemySprite->SetVelocity(0, 0);

		}
	}
}

/*********** RETURNING TO POSITION ***********/
void Enemy::movingToPoint(float gameTime)
{
	if (positionHoldTimer > gameTime) return;
	if (!currentWaypoints.empty())
	{
		ifwaipointNotEmptyGoto();

		// Stop on the arriving to dest
		if (Distance(enemySprite->GetPos(), currentWaypoints.back()) <= 25)
		{
			currentWaypoints.clear();
			enemySprite->SetStatus(IDLE);

			enemySprite->SetVelocity(0, 0);
			isdogAllerted = false;
			dogAlarma = false;
		}
	}
}

//GETTERS
CSprite* Enemy::GetEnemySprite()
{
	return enemySprite;
}

bool Enemy::IsEnemyDead()
{
	return IsDead;
}