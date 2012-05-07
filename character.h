/*Header file for character class in project Ground Up Fighting Game
 *
 *Written by Alex Kelly in 2012
 *Under MIT OSI license, see "COPYING" for details
 */

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include "utility.h"
#include <stdio.h>

class character{
public:
	character();		//Load the entire character into memory. This should happen once per player per match.
	virtual ~character();	//Free stuff

	void build(const char*);	//This will *eventually* be the function that parses the character constructor file.
	moveTrie * head;	//Trie for ground moves
	moveTrie * airHead;	//Trie for air moves

	char * name; //The name of the directory from which the character spawns. This is important for loading into memory
	SDL_Surface * draw(int);//Pass sprite information up.
	virtual int takeHit(character *, SDL_Rect&, int);	//Handle taking hits.
	virtual void prepHooks(int[], bool[], bool[]);	//Take input from the game and propagate it to the appropriate moveTrie.
	//BRB prepping my hooks

	move * neutral;
	move * crouch;
	hitstun * reel;
	hitstun * fall;
	hitstun * crouchReel;
	hitstun * crouchBlock;
	hitstun * standBlock;
	hitstun * airBlock;

	move * cMove;
	move * bMove;

	int freeze;
	int health;
	int meter;
	int rounds;
	SDL_Rect * volition;
	bool aerial; 		//Flags whether the character is considered "in the air"
private:

	int state; 

};
