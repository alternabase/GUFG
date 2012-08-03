#include "action.h"
#include <math.h>

void hitstun::init(int n)
{
	currentFrame = 0;
	counter = n;
	cFlag = 0;
}

void hitstun::step(int *& resource)
{
	if(counter <= 0) currentFrame++;
	else counter--;
}

action * hitstun::blockSuccess(int st){
	init(st);
	return this;
}

int hitstun::takeHit(hStat & s, int b){
	if(s.blockMask.i & blockState.i){
		if(b == 1){ 
			init(s.stun - 1  - s.stun/5);
			s.push = (s.push*4)/5;
			return -1;
		} else {
			init(s.stun - std::max(0, 1 - s.stun/15));
			return 0;
		}
	}
	else return 1;
}

hitstun::hitstun(const char * n)
{
	build(n);
	currentFrame = 0;
}

int hitstun::arbitraryPoll(int q)
{
	if(q == 1) return counter;
	else return 0;
}
