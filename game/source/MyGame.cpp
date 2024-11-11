#include "stdafx.h"
#include "MyGame.h"

/*********** CONSTRUCTOR ***********/
CMyGame::CMyGame(void) : mainMap(), player(mainMap)
{
	//PREFABS
	catSpritePrefab = new CSprite();
	catSpritePrefab->AddImage("catSprite.png", "Idle", 36, 1, 0, 0, 16, 0);
	catSpritePrefab->AddImage("catSprite.png", "Walk", 36, 1, 18, 0, 24, 0);
	catSpritePrefab->AddImage("catSprite.png", "Attack", 36, 1, 28, 0, 35, 0);

	cat2SpritePrefab = new CSprite();
	cat2SpritePrefab->AddImage("catSprite2.png", "Idle", 36, 1, 0, 0, 16, 0);
	cat2SpritePrefab->AddImage("catSprite2.png", "Walk", 36, 1, 21, 0, 26, 0);
	cat2SpritePrefab->AddImage("catSprite2.png", "Attack", 36, 1, 30, 0, 33, 0);

	dogSpritePrefab = new CSprite();
	dogSpritePrefab->AddImage("dogSprite.png", "Attack", 36, 1, 0, 0, 7, 0);
	dogSpritePrefab->AddImage("dogSprite.png", "Walk", 36, 1, 8, 0, 15, 0);
	dogSpritePrefab->AddImage("dogSprite.png", "Idle", 36, 1, 16, 0, 35, 0);

	humanSpritePrefab = new CSprite();
	humanSpritePrefab->AddImage("human.png", "Idle", 2, 4, 0, 3, 0, 3, CColor::White());
	humanSpritePrefab->AddImage("human.png", "Walk", 2, 4, 0, 3, 0, 1, CColor::White());
	humanSpritePrefab->AddImage("human.png", "Attack", 2, 4, 0, 2, 1, 2, CColor::White());

	//Menu BG
	startScreen.LoadImage("menuBar.jpg");
	startScreen.SetImage("menuBar.jpg");
	startScreen.SetSize(1366, 768);
	startScreen.SetPosition(1366 / 2, 768 / 2);

	//GameOver BG
	gameOverBg.LoadImage("gameOverBg.jpg");
	gameOverBg.SetImage("gameOverBg.jpg");
	gameOverBg.SetSize(1366, 768);
	gameOverBg.SetPosition(1366 / 2, 768 / 2);

	//gameWinBg BG
	gameWinBg.LoadImage("gameWinBg.jpg");
	gameWinBg.SetImage("gameWinBg.jpg");
	gameWinBg.SetSize(1366, 768);
	gameWinBg.SetPosition(1366 / 2, 768 / 2);

	//Main Menu Scroling Logo
	mainMenuSelectionLogo.LoadImage("Cheese.png");
	mainMenuSelectionLogo.SetImage("Cheese.png");
	mainMenuSelectionLogo.SetSize(60, 50);
}

/*********** UPDATE ***********/
void CMyGame::OnUpdate()
{
	if (IsMenuMode() || IsPaused() || gameOver)
		return;
	Uint32 t = GetTime();

	//Update And delete Enemies
	for (auto AIplayer : AllEnemies)
	{
		AIplayer->Update(t);
		if (AIplayer->IsEnemyDead())
			AllEnemies.erase(find(AllEnemies.begin(), AllEnemies.end(), AIplayer));
	}

	//update Player and Game Status
	player.Update(t, AllEnemies);

	if (player.IsPlayerDead() || player.IsGameWon())
		OnGameOver();
}

/***********  DRAW ***********/
void CMyGame::OnDraw(CGraphics* g)
{
	//if game over draw win or game over screen for 2 sec
	if (deadScreenTimer != 0 && deadScreenTimer > GetTime())
	{
		if (player.IsGameWon())
			gameWinBg.Draw(g);
		else
			gameOverBg.Draw(g);
	}
	//if menu mode show menu
	else if (IsMenuMode() || IsPaused())
		menuHandler(g);
	else
	{
		//if In Game Draw (map, enemies, p[layer)
		mainMap.Draw(g, player.getPlayerSprite()->GetPosition());
		for (auto AIplayer : AllEnemies)
			AIplayer->Draw(g);
		player.Draw(g, GetTime());
	}
}

/***********  INIT ***********/
void CMyGame::OnInitialize()
{
	PauseGame(false);
	ChangeMode(MODE_MENU);
	gameStarted = IsGameWon = false;
	currentMenuState = MENU;
	startScreenSelection = NEWGAME;
	mainBgMusic.Play("mainBgMusic.wav", -1);
	isMainMusicPlayong = true;
}

/***********  on Game Start ***********/
void CMyGame::OnStartGame()
{
	deadScreenTimer = 0;
	mainMap.SetGlobalLight(true);
	player.gameInit();
	EnemyCreator();
	gameOver = false;
}

/***********  Game OVer ***********/
void CMyGame::OnGameOver()
{
	mainBgMusic.Stop();
	isMainMusicPlayong = false;
	if (player.IsPlayerDead())
		loseSound.Play("gameOver.wav", 0);
	else
		winSound.Play("win.wav", 0);;

	deadScreenTimer = 2500;
	gameOver = true;
	gameStarted = false;
	currentMenuState = MENU;
	ChangeMode(MODE_MENU);
}

/***********  Enemy Creator ***********/
void CMyGame::EnemyCreator()
{
	for (auto& AIplayer : AllEnemies)
		delete AIplayer;

	AllEnemies.clear();
	enum enemyType { CAT, DOG, HUMAN };

	Enemy* cat1 = new Enemy(mainMap, player, *catSpritePrefab->Clone(), CVector(350, 650), CAT);
	Enemy* cat2 = new Enemy(mainMap, player, *cat2SpritePrefab->Clone(), CVector(1840, 330), CAT);
	Enemy* dog = new Enemy(mainMap, player, *dogSpritePrefab->Clone(), CVector(1200, 1200), DOG);
	Enemy* human = new Enemy(mainMap, player, *humanSpritePrefab->Clone(), CVector(1720, 1410), HUMAN);

	AllEnemies.push_back(cat1);
	AllEnemies.push_back(cat2);
	AllEnemies.push_back(dog);
	AllEnemies.push_back(human);
}

/***********  menuHandler ***********/
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
		*g << font("AFontPTSerif.ttf", 28) << color(CColor::Black()) << xy(571, 420) << "Q - attack";
		*g << font(28) << color(CColor::Black()) << xy(571, 370) << "W - hide";
		*g << font(28) << color(CColor::Black()) << xy(571, 330) << "E - SpeedUp";
		*g << font(28) << color(CColor::Black()) << xy(571, 280) << "R. Mouse -  Navigation ";
		*g << font("ARIAL.ttf", 52) << color(startScreenSelection == BACK ? CColor::Yellow() : CColor::LightGray()) << xy(656, 200) << "BACK";
	}

	//MAIN MENU PAGE
	else
	{
		//IF GAME STARTED SWITCH NEW GAME AND CONTINUE POSITION
		if (gameStarted && !gameOver)
		{
			*g << font(42) << color(startScreenSelection == CONTINUE ? CColor::Yellow() : CColor::White()) << xy(626, 420) << "CONTINUE";
			if (startScreenSelection == CONTINUE) mainMenuSelectionLogo.SetPosition(561, 430);
			*g << font(42) << color(startScreenSelection == NEWGAME ? CColor::Yellow() : CColor::White()) << xy(626, 350) << "NEW GAME";
			if (startScreenSelection == NEWGAME) mainMenuSelectionLogo.SetPosition(561, 360);
		}
		else
		{
			if (startScreenSelection == NEWGAME) mainMenuSelectionLogo.SetPosition(561, 430);
			*g << font(42) << color(startScreenSelection == NEWGAME ? CColor::Yellow() : CColor::White()) << xy(626, 420) << "NEW GAME";
			*g << font(42) << color(CColor::DarkGray()) << xy(626, 350) << "CONTINUE";
		}

		*g << font(42) << color(startScreenSelection == CONTROLS ? CColor::Yellow() : CColor::White()) << xy(626, 280) << "CONTROLS";
		if (startScreenSelection == CONTROLS) mainMenuSelectionLogo.SetPosition(561, 290);
		*g << font(42) << color(startScreenSelection == EXIT ? CColor::Yellow() : CColor::White()) << xy(626, 210) << "EXIT";
		if (startScreenSelection == EXIT) mainMenuSelectionLogo.SetPosition(561, 220);

		mainMenuSelectionLogo.Draw(g);
	}
}

/***********  MOUSE EVENTS ***********/
void CMyGame::OnRButtonDown(Uint16 x, Uint16 y)
{
	////*** CALL PLAYER ON KEY DOWN IF NOT IN MENU MODE OR PAUSE
	if (!IsMenuMode() && !IsPaused() && !gameOver)
		player.OnRButtonDown(x, y, GetTime());
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
