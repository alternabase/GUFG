#include "yellow.h"
yellow::yellow(){
	name = "Yellow";
	delete head;
	delete neutral;
//	delete crouch;
	delete standBlock;
	delete crouchBlock;
	delete airBlock;
	delete reel;
	delete fall;
//	delete crouchReel;
	head = new moveTrie;
	build(name);
	
	cMove = neutral;
}
