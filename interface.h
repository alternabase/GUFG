/*Interface header for GUFG
 *The interface class/struct is intended as an abstraction between the player and the engine
 *This is where things pertaining to inputs should go, as well as sprsibly information like the
 *round timer.
 *
 *Written by Alex Kelly in 2012
 *Licensed under MIT OSI: Additional information in the file "COPYING"
 */

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <stdio.h>
#include "player.h"
class interface
{
public:
	interface();        //Constructor!
	~interface();
	/*Important interface functions that will remain interface functions*/

	void readInput();   //Reads input from players.
	void resolve();     //This will eventually resolve actions (Such as a move hitting a player.) With only one player, this is not currently sprsible.
	void draw();        //This will eventually draw everything. Right now the drawing is done in the main loop.
	void runTimer();    //Runs the timer. Currently just a decrementer wrapper.
	void spriteInit();  //Initializes the sprite for drawing
	void roundInit();
	void matchInit();
	void cSelectMenu();
	void checkWin();
	void dragBG(int);

	player * p[2];
	SDL_Surface *background, *screen;
	SDL_Rect bg;
	int grav; 			//Gravitational constant. 
	bool fullscreen;		//For use with later.
	bool sAxis1[4], sAxis2[4];	//Initial input buffering.
	bool posEdge1[5], posEdge2[5];	
	bool negEdge1[5], negEdge2[5];
	void readStick(SDL_Event&);
	void readKey(SDL_Event&);
	int combo1;
	int combo2;
	int numRounds;
	bool gameover;

//testing variables;
	bool q;
private:
	int timer;
	int screenHeight, screenWidth, floor, wall;
};
