#include "stdafx.h"
#include "MyGame.h"

CMyGame::CMyGame(void) : mainMap(), player(mainMap)
{
	//PREFABS
	catSpritePrefab = new CSprite();
	catSpritePrefab->AddImage("catSprite.png", "Walk", 36, 1, 0, 0, 6, 0, CColor::Red());
	catSpritePrefab->AddImage("catSprite.png", "Attack", 36, 1, 7, 0, 15, 0, CColor::Red());
	catSpritePrefab->AddImage("catSprite.png", "Idle", 36, 1, 16, 0, 35, 0, CColor::Red());
 
	dogSpritePrefab = new CSprite();
	dogSpritePrefab->AddImage("dogSprite.png", "Attack", 35, 1, 0, 0, 7, 0, CColor::Red());
	dogSpritePrefab->AddImage("dogSprite.png", "Walk", 35, 1, 8, 0, 15, 0, CColor::Red());
	dogSpritePrefab->AddImage("dogSprite.png", "Idle", 35, 1, 16, 0, 34, 0, CColor::Red());


	humanSpritePrefab = new CSprite();
	humanSpritePrefab->AddImage("human.png", "Walk", 2, 3, 0, 2, 0, 0, CColor::Blue());
	humanSpritePrefab->AddImage("human.png", "Idle", 2, 3, 0, 2, 0, 0, CColor::Blue());
	
}

CMyGame::~CMyGame(void)
{
}


/*********** UPDATE ***********/
void CMyGame::OnUpdate()
{
	if (IsMenuMode() || IsPaused() || gameOver) return;
	Uint32 t = GetTime();
	for (auto AIplayer : AllEnemies) 
	{
		AIplayer->Update(t);
		if (AIplayer->IsDead) 
			AllEnemies.erase(find(AllEnemies.begin(), AllEnemies.end(), AIplayer));
	}
	player.Update(t, AllEnemies);

	if (player.IsDead || player.isGameWon)
		OnGameOver();
		
}

/***********  DRAW ***********/
void CMyGame::OnDraw(CGraphics* g)
{
	if (deadScreenTimer != 0 && deadScreenTimer > GetTime())
	{
		if(player.isGameWon)
			gameWinBg.Draw(g);
		else
			gameOverBg.Draw(g);
	}
	else if (IsMenuMode() || IsPaused())
		menuHandler(g);
	
		
	else 
	{
		mainMap.Draw(g, player.playerSprite->GetPosition());
		for (auto AIplayer : AllEnemies)   AIplayer->Draw(g);
		player.Draw(g);
	}
}


/***********  INIT ***********/
void CMyGame::OnInitialize()
{
	initSpritesHandler();
	PauseGame(false);
	ChangeMode(MODE_MENU);  
	gameStarted = IsGameWon = false;
	currentMenuState = MENU;
	startScreenSelection = NEWGAME;
	mainMap.globalLight = true;
	mainBgMusic.Play("mainBgMusic.wav", -1);
	isMainMusicPlayong = true;
}

/***********  TO CLEAN ***********/
void CMyGame::OnDisplayMenu()
{
	//StartGame();
	//ChangeMode(MODE_MENU);
}

void CMyGame::OnStartGame()
{
	deadScreenTimer = 0;
	player.gameInit();
	EnemyCreator();
	gameOver = false;
}

void CMyGame::OnStartLevel(Sint16 nLevel)
{

}

void CMyGame::OnGameOver()
{
	mainBgMusic.Stop();
	isMainMusicPlayong = false;
	if (player.IsDead)
		loseSound.Play("gameOver.wav", 0);
	else
		winSound.Play("win.wav", 0);;

	deadScreenTimer =  2500;
	gameOver = true;
	gameStarted = false;
	currentMenuState = MENU;
	ChangeMode(MODE_MENU);
}

void CMyGame::OnTerminate()
{
}

/***********  KEYBOARD EVENTS ***********/


void CMyGame::OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode)
{
 
	if (!IsMenuMode() && !IsPaused() && !gameOver)
		player.OnKeyDown(sym, mod, unicode, GetTime());


	//*** MENU NAVIGATION DOWN
	if ((IsMenuMode() || IsPaused()) && ((sym == SDLK_s) || (sym == SDLK_DOWN)) && !showControllImg)
	{
		startScreenSelection++;
		if (startScreenSelection > 3) startScreenSelection = gameStarted ? 0 : 1; //Change Sequence if game started
	}

	//*** MENU NAVIGATION UP
	if ((IsMenuMode() || IsPaused()) && ((sym == SDLK_w) || (sym == SDLK_UP)) && !showControllImg)
	{
		startScreenSelection--;
		int range = gameStarted ? 0 : 1;
		if (startScreenSelection < range) startScreenSelection = 3;
	}

	//*** MAIN MENU LOGIC
	if ((IsMenuMode() || IsPaused()) && (sym == 13) && deadScreenTimer < GetTime()) //enter
	{
		//###  NEW GAME
		if (startScreenSelection == NEWGAME)
		{
			currentMenuState = INGAME;
			PauseGame(false);
			StartGame();
			gameStarted = true;
			gameOver = false;
		}

		//###  CONTINUE THE GAME IF IT'S STARTED
		if (startScreenSelection == CONTINUE && gameStarted) {
			currentMenuState = INGAME;
			PauseGame(false);
		}

		//###  CONTROLS PAGE LOGIC
		if (startScreenSelection == CONTROLS) {
			showControllImg = true;
			startScreenSelection = BACK;
		}

		else if (startScreenSelection == BACK && showControllImg)
		{
			showControllImg = false;
			startScreenSelection = CONTROLS;
		}

		//# EXIT
		if (startScreenSelection == EXIT) StopGame();
	}

	//*** ESC MENU SHOW HIDE, WHEN ESC PRESSED
	if (sym == SDLK_ESCAPE && !IsGameOver())
	{
		if (showControllImg)
		{
			showControllImg = false;
			startScreenSelection = CONTROLS;
		}
		
		if (gameStarted)
		{
			if (currentMenuState == MENU && !showControllImg)
			{
				PauseGame(false);
				currentMenuState = INGAME;
			}
			else
			{
				PauseGame(true);
				currentMenuState = MENU;
			}
		}
	}
}


void CMyGame::OnKeyUp(SDLKey sym, SDLMod mod, Uint16 unicode)
{
}



/***********  MOUSE EVENTS ***********/

void CMyGame::OnMouseMove(Uint16 x, Uint16 y, Sint16 relx, Sint16 rely, bool bLeft, bool bRight, bool bMiddle)
{
}

void CMyGame::OnLButtonDown(Uint16 x, Uint16 y)
{
	
}

 
void CMyGame::OnRButtonDown(Uint16 x, Uint16 y)
{
	////*** CALL PLAYER ON KEY DOWN IF NOT IN MENU MODE OR PAUSE
	if (!IsMenuMode() && !IsPaused() && !gameOver)
		player.OnRButtonDown(x, y);
}

void CMyGame::OnRButtonUp(Uint16 x, Uint16 y)
{
}

 
void CMyGame::initSpritesHandler()
{
	//Menu BG
	startScreen.LoadImage("menuBar.jpg");
	startScreen.SetImage("menuBar.jpg");
	startScreen.SetSize(1024, 768);
	startScreen.SetPosition(1024 / 2, 768 / 2);


	//GameOver BG
	gameOverBg.LoadImage("gameOverBg.jpg");
	gameOverBg.SetImage("gameOverBg.jpg");
	gameOverBg.SetSize(1024, 768);
	gameOverBg.SetPosition(1024 / 2, 768 / 2);

	//gameWinBg BG
	gameWinBg.LoadImage("gameWinBg.jpg");
	gameWinBg.SetImage("gameWinBg.jpg");
	gameWinBg.SetSize(1024, 768);
	gameWinBg.SetPosition(1024 / 2, 768 / 2);

	//Main Menu Scroling Logo
	mainMenuSelectionLogo.LoadImage("Cheese.png");
	mainMenuSelectionLogo.SetImage("Cheese.png");
	mainMenuSelectionLogo.SetSize(60, 50);;
 
}

void CMyGame::menuHandler(CGraphics* g)
{
	//music reset - yeah the best place to do it!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! ( for game reset mostly)
	if (!isMainMusicPlayong)
	{
		mainBgMusic.Play("mainBgMusic.wav", -1);
		isMainMusicPlayong = true;
	}
		
	startScreen.Draw(g);
	//CONTROLS PAGE
	if (showControllImg)
	{
		*g << font("AFontPTSerif.ttf",28) << color(CColor::Black()) << xy(400, 420) << "Q - attack";
		*g << font(28) << color(CColor::Black()) << xy(400, 370) << "W - hide";
		*g << font(28) << color(CColor::Black()) << xy(400, 330) << "E - SpeedUp";
		*g << font(28) << color(CColor::Black()) << xy(400, 280) << "R. Mouse -  Navigation ";
		*g << font("ARIAL.ttf", 52) << color(startScreenSelection == BACK ? CColor::Yellow() : CColor::LightGray()) << xy(485, 200) << "BACK";
	}

	//MAIN MENU PAGE
	else
	{
		//IF GAME STARTED SWITCH NEW GAME AND CONTINUE POSITION
		if (gameStarted && !gameOver)
		{
			*g << font(42) << color(startScreenSelection == CONTINUE ? CColor::Yellow() : CColor::White()) << xy(455, 420) << "CONTINUE";
			if (startScreenSelection == CONTINUE) mainMenuSelectionLogo.SetPosition(390, 430);
			*g << font(42) << color(startScreenSelection == NEWGAME ? CColor::Yellow() : CColor::White()) << xy(455, 350) << "NEW GAME";
			if (startScreenSelection == NEWGAME) mainMenuSelectionLogo.SetPosition(390, 360);
		}
		else
		{
			if (startScreenSelection == NEWGAME) mainMenuSelectionLogo.SetPosition(390, 430);
			*g << font(42) << color(startScreenSelection == NEWGAME ? CColor::Yellow() : CColor::White()) << xy(455, 420) << "NEW GAME";
			*g << font(42) << color(CColor::DarkGray()) << xy(455, 350) << "CONTINUE";
		}

		*g << font(42) << color(startScreenSelection == CONTROLS ? CColor::Yellow() : CColor::White()) << xy(455, 280) << "CONTROLS";
		if (startScreenSelection == CONTROLS) mainMenuSelectionLogo.SetPosition(390, 290);
		*g << font(42) << color(startScreenSelection == EXIT ? CColor::Yellow() : CColor::White()) << xy(485, 210) << "EXIT";
		if (startScreenSelection == EXIT) mainMenuSelectionLogo.SetPosition(390, 220);

		mainMenuSelectionLogo.Draw(g);
	}
}


void CMyGame::EnemyCreator()
{
	for (auto& AIplayer : AllEnemies)
		delete AIplayer;
		
	AllEnemies.clear();
	enum enemyType { CAT, DOG, HUMAN };
 
	Enemy* cat1 = new Enemy(mainMap, player, *catSpritePrefab->Clone(),CVector(350, 650), CAT);
	Enemy* cat2 = new Enemy(mainMap, player, *catSpritePrefab->Clone(), CVector(1840, 330), CAT);
	Enemy* dog = new Enemy(mainMap, player, *dogSpritePrefab->Clone(), CVector(1200, 1200), DOG);
	Enemy* human = new Enemy(mainMap, player, *humanSpritePrefab->Clone(), CVector(1720, 1410), HUMAN);

	AllEnemies.push_back(cat1);
	AllEnemies.push_back(cat2);
	AllEnemies.push_back(dog);
	AllEnemies.push_back(human);
}