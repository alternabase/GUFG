/*Header file for "Player" class in project Ground Up Fighting Game
 *
 *Written by Alex Kelly in 2012
 *Under MIT OSI license, see the file "COPYING" for details
 */

#include "charlist.h"
class player{
public:
	player();
	player(int);
	~player();

	const char * inputName[10];   //Input names. This is really just for housekeeping.
	SDL_Event input[10];    //Inputs. These are the SDL_Events tied to the 10 buttons in the actual game
	character * pick;
	int deltaX, deltaY; 
	int posX, posY;
	int rounds;		//How many rounds has this player won this match?
	int facing;
	int padding[400];   //More magic. Do not touch
	void writeConfig();
	bool readConfig();
	void characterSelect(int);
	void land();
	void pushInput(bool*);
	void getMove(bool*, bool*, SDL_Rect &, bool);
	void draw(int, int);
	void drawBoxen(int, int);
	void drawHitParticle(int, int);
	void drawMeters(int);
	void readEvent(SDL_Event &, bool *&, bool *&, bool *&);
	bool spriteCheck();
	void roundInit();
	int takeHit(int, hStat&);
	void connect(int, hStat&);
	bool CHState();
	bool elasticX:1;
	bool elasticY:1;
	bool slide:1;
	bool stick:1;
	int blockType;
	int hover;
	int throwInvuln;

	SDL_Rect spr, *hitbox, *hitreg, collision, *momentum, lock;
	int hitComplexity, regComplexity, momentumComplexity;
	bool rCorner, lCorner;

	int particleLife;
	int particleType;

	/*Helper functions for "resolve" tick*/
	void updateRects();
	void enforceGravity(int, int);
	void checkBlocking();
	void checkFacing(player*);
	int dragBG(int, int);
	void checkCorners(int, int, int);
	void combineDelta();
	void addVector(SDL_Rect&);
	void clipVectors(int);
	void pullVolition();
	void invertVectors(int);
	void setPosition(int, int);
	bool secondInstance;
	void getThrown(action*, int, int);

private:
	int ID;
	int inputBuffer[30];
	void init();
	void removeVector(int);
};
