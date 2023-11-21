#include "stdafx.h"
#include "MyGame.h"
#include <sstream>

CMyGame::CMyGame(void)
{
	score=0;
}

//New Random(x) Position For Given Sprite
void CMyGame::UpdateFruitPos(CSprite& sprite)
{
	sprite.SetPosition(float(rand() % 768 + 16 ), 600);
}

// Sprite Creator
void CMyGame::spriteCreator(CSprite& sprite, char * spritepath)
{
	sprite.LoadImage(spritepath, CColor::Black());
	sprite.SetImage(spritepath);
}

CMyGame::~CMyGame(void)
{
	// TODO: add destruction code here
}
 
void CMyGame::OnUpdate()
{
	if (IsMenuMode() || IsGameOverMode()) return;

	int t = GetTime();
 
	// BANANA DELAY LOGIC  bananaDelay = GetTime() + 3000
	if (bananaDelayTime - t <= 0 ) banana.SetMotion(0, -250);
	else banana.SetMotion(0, 0);
	

	// PEAR LOGIC
	//cos goes from - 1 to 1  sin = Y ,  COS = X
	angle += 0.03;
	double X = pear.GetX() + radius * cos(angle);
	double Y = pear.GetY() + radius * sin(angle);
	pear.SetPos(X, Y);
	
	
	// HIT TESTS
	if (cherry.HitTest(&player))
	{	
		score++;
		player.SetSize(player.GetSize().X() + 10, player.GetSize().Y() + 10);
		UpdateFruitPos(cherry); 
		catchSound.Play("sounds/twang.wav");
		catchSound.Volume(0.15);
	}

	if (banana.HitTest(&player) && bananaDelayTime - t <= 0)
	{
		if (score >= 10) score -= 10;
		else score = 0;
		UpdateFruitPos(banana);
		player.SetSize(32,32);
		scaleDownSize.Play("sounds/scaleDown.wav");
		scaleDownSize.Volume(0.2);
		bananaDelayTime = t + bananaDelay;
	}

	if (pear.HitTest(&player))
	{
		gameOver.Play("sounds/reset.wav");
		score = 0;
	}

	if (apple.HitTest(&player))
	{
		mainbBgMusic.Stop();
		gameOver.Play("sounds/gameOver.wav");
		gameOver.Volume(0.2);
		GameOver();
	}

	// If sprites reached bottom of the screen
	if(cherry.GetY() < 0) UpdateFruitPos(cherry);
	if (apple.GetY() < 0) UpdateFruitPos(apple);
	if (banana.GetY() < 0)
	{
		bananaDelayTime = t + bananaDelay;
		UpdateFruitPos(banana);
	}
		
	// ----- updating sprites -----------
	player.Update(t);
	cherry.Update(t);
	pear.Update(t);
	apple.Update(t);
	banana.Update(t);
}

void CMyGame::OnDraw(CGraphics* g)
{   
	//Draw Sprites
	player.Draw(g); 
	cherry.Draw(g);
	pear.Draw(g);
	apple.Draw(g);
	if (bananaDelayTime - GetTime() <= 0)   banana.Draw(g);
	
 


	//Draw Game Rulles & Score
	cherryRules.Draw(g);
	bananaRules.Draw(g);
	pearRules.Draw(g);
	appleRules.Draw(g);
	*g << font(12)  << color(CColor::Green()) << xy(630, 6) << " - Score + 1, Size++";
	*g << font(12) << color(CColor::Green()) << xy(630, 24) << " - Score -10 , Size = Normal";
	*g << font(12) << color(CColor::Green()) << xy(630, 42) << " - Score = 0";
	*g << font(12) << color(CColor::Green()) << xy(630, 60) << " - Game Over";
	*g << font(28) << color(CColor::Red()) << xy(10, 570) << score;
	*g << font(12) << color(CColor::Red()) << xy(400, 300) << 300 + 10 * sin(1.6);
	*g << font(12) << color(CColor::Red()) << xy(400, 205) << "x";
	*g << font(12) << color(CColor::Red()) << xy(400, 395) << "x";
	*g << font(12) << color(CColor::Red()) << xy(300, 300) << "x";
	*g << font(12) << color(CColor::Red()) << xy(500, 300) << "x";
 
	// print game over at at the centre of the screen
	if ( IsGameOverMode())
		*g << font(46) << color(CColor::Green()) << vcenter << center << " GAME OVER "
		<< font(22) << color(CColor::Red()) << endl << "Your Score is: " << score;
}

/////////////////////////////////////////////////////
// Game Life Cycle

// one time initialisation
void CMyGame::OnInitialize()
{
	//Sprites For Rules
	spriteCreator(cherryRules, "Cherry.bmp");
	spriteCreator(bananaRules, "Banana.bmp");
	spriteCreator(pearRules, "Pear.bmp");
	spriteCreator(appleRules, "Apple.bmp");


	spriteCreator(cherry, "Cherry.bmp"); //cherry
	spriteCreator(pear, "Pear.bmp"); 	//pear
	spriteCreator(apple, "Apple.bmp");	//apple
	spriteCreator(banana, "Banana.bmp");	//banana
	spriteCreator(player, "greenmonster.bmp");	//player
}

void CMyGame::OnDisplayMenu()
{
	StartGame();
}

// called when Game Mode entered
void CMyGame::OnStartGame()
{
	mainbBgMusic.Play("sounds/mainBgMusic.wav",-1);
	mainbBgMusic.Volume(0.05);
	score = angle = bananaDelayTime = 0;
	player.SetSize(32, 32);
	
	//player
	player.SetPosition(600, 400);
	player.SetOmega(200);

	//cherry
	UpdateFruitPos(cherry);
	cherry.SetMotion(0, -100);

	//Banana
	UpdateFruitPos(banana);
	banana.SetMotion(0, -100);
	banana.SetOmega(50);

	//apple
	UpdateFruitPos(apple);
	apple.SetMotion(0, -175);

	//pear 
	pear.SetPos(408, 178 );
	
	
	//Sprite Position For Rules
	cherryRules.SetPosition(float(620), 10);
	cherryRules.SetSize(16, 16);

	bananaRules.SetPosition(float(620), 28);
	bananaRules.SetSize(16, 16);

	pearRules.SetPosition(float(620), 46);
	pearRules.SetSize(16, 16);

	appleRules.SetPosition(float(620), 64);
	appleRules.SetSize(16, 16);

	HideMouse();
}

// called when Game is Over
void CMyGame::OnGameOver()
{
	ShowMouse();
}

// one time termination code
void CMyGame::OnTerminate()
{
}

/////////////////////////////////////////////////////
// Keyboard Event Handlers

void CMyGame::OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode)
{
	if (sym == SDLK_F4 && (mod & (KMOD_LALT | KMOD_RALT)))
		StopGame();
	if (sym == SDLK_F2)
		NewGame();
}

void CMyGame::OnKeyUp(SDLKey sym, SDLMod mod, Uint16 unicode)
{
	
}


/////////////////////////////////////////////////////
// Mouse Events Handlers

void CMyGame::OnMouseMove(Uint16 x,Uint16 y,Sint16 relx,Sint16 rely,bool bLeft,bool bRight,bool bMiddle)
{
	// synchronise horizontal paddle position with the mouse position 
	if (IsGameMode()) player.SetPosition(x,y);
}

void CMyGame::OnLButtonDown(Uint16 x,Uint16 y)
{
}

void CMyGame::OnLButtonUp(Uint16 x,Uint16 y)
{
}

void CMyGame::OnRButtonDown(Uint16 x,Uint16 y)
{
}

void CMyGame::OnRButtonUp(Uint16 x,Uint16 y)
{
}

void CMyGame::OnMButtonDown(Uint16 x,Uint16 y)
{
}

void CMyGame::OnMButtonUp(Uint16 x,Uint16 y)
{
}
