#pragma once
#include "Map/Map.h"
#include "Player.h"
#include "Enemy.h"


class CMyGame : public CGame
{
	Map mainMap;
	Player player;
 

public:
	CMyGame(void);
	~CMyGame(void);

	void EnemyCreator();

	// Per-Frame Callback Funtions (must be implemented!)
	virtual void OnUpdate();
	virtual void OnDraw(CGraphics* g);

	// Game Life Cycle
	virtual void OnInitialize();
	virtual void OnDisplayMenu();
	virtual void OnStartGame();
	virtual void OnStartLevel(Sint16 nLevel);
	virtual void OnGameOver();
	virtual void OnTerminate();

	// Keyboard Event Handlers
	virtual void OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode);
	virtual void OnKeyUp(SDLKey sym, SDLMod mod, Uint16 unicode);

	// Mouse Events Handlers
	virtual void OnMouseMove(Uint16 x,Uint16 y,Sint16 relx,Sint16 rely,bool bLeft,bool bRight,bool bMiddle);
	virtual void OnLButtonDown(Uint16 x,Uint16 y);
	virtual void OnRButtonDown(Uint16 x,Uint16 y);
	virtual void OnRButtonUp(Uint16 x,Uint16 y);
 
	void menuHandler(CGraphics* g);
	void initSpritesHandler();
	CSprite startScreen;
	CSprite mainMenuSelectionLogo;
	

	bool gameStarted;
	bool IsGameWon;
	bool showControllImg;

	//game Over
	bool gameOver;
	float deadScreenTimer;
	CSprite gameOverBg;
	CSprite gameWinBg;

	int currentMenuState;
	enum gameStates {MENU, PAUSE, INGAME};

	int startScreenSelection;
	enum menuScreenSelection { CONTINUE, NEWGAME, CONTROLS, EXIT, BACK };
	vector<Enemy*> AllEnemies;


	//prefabs  
	CSprite* catSpritePrefab;
	CSprite* dogSpritePrefab;
	CSprite* humanSpritePrefab;

	//sounds
	CSoundPlayer  mainBgMusic;
	bool isMainMusicPlayong; // native Functions dosrn't work at all
	CSoundPlayer  loseSound;
	CSoundPlayer  winSound;
 
};