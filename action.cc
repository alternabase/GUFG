#include <stdio.h>
#include <iostream>
#include <fstream>
#include "action.h"
#include <assert.h>
using namespace std;

action::action() : frames(0), hits(0), name(NULL)
{
	name = NULL;
}

action::action(const char * n) : frames(0), hits(0)
{
	build(n);
}

action::~action()
{
	if(!this) return;
/*	for(int i = 0; i < frames; i++){
		if(sprite[i] != NULL) SDL_FreeSurface(sprite[i]);
		if(fSprite[i] != NULL) SDL_FreeSurface(fSprite[i]);
	}*/
	if(collision) delete [] collision;
	if(hitbox) delete [] hitbox;
	if(hitreg) delete [] hitreg;
	if(hitComplexity) delete [] hitComplexity;
	if(regComplexity) delete [] regComplexity;
	if(deltaComplexity) delete [] deltaComplexity;
	if(delta) delete [] delta;
	if(state) delete [] state;
	if(gain) delete [] gain;
	if(totalStartup) delete [] totalStartup;
	if(name) delete [] name;
	if(stats) delete [] stats;
	if(next) delete next;
	if(onConnect) delete [] onConnect;
}

void action::zero()
{
	attemptStart = 0;
	attemptEnd = 0;
	stop = 0;
	throwinvuln = 0;
	crouch = 0;
	armorStart = 0; armorLength = 0;
	armorHits = 0;
	guardStart = 0; guardLength = 0;
	blockState.i = 0;
	isProjectile = 0;
	stats = NULL;
	cost = 0;
	dies = 0;
	fch = 0;
	tempNext = NULL;
	tempAttempt = NULL;
}

void action::build(const char * n)
{
	zero();
	ifstream read;
	char fname[40];
	char buffer[100];
	char savedBuffer[100];
	buffer[0] = '\0';

	sprintf(fname, "%s.mv", n);
	read.open(fname);
	assert(!read.fail());
	next = NULL;
	attempt = NULL;

	do {
		read.getline(buffer, 100);
		strcpy(savedBuffer, buffer);
	} while (setParameter(buffer));

	parseProperties(savedBuffer);

	collision = new SDL_Rect[frames];
	hitbox = new SDL_Rect*[frames];
	hitComplexity = new int[frames];
	hitreg = new SDL_Rect*[frames];
	regComplexity = new int[frames];
	delta = new SDL_Rect*[frames];
	deltaComplexity = new int[frames];

	int currHit = 0;

	for(int i = 0; i < frames; i++){
		while(read.get() != '$'); read.ignore(2);
		read >> collision[i].x >> collision[i].y >> collision[i].w >> collision[i].h;
		while(read.get() != '$'); read.ignore(2);
		read.get(buffer, 100, '\n');
		regComplexity[i] = aux::defineRectArray(buffer, hitreg[i]);
		while(read.get() != '$'); read.ignore(2);
		read.get(buffer, 100, '\n');
		deltaComplexity[i] = aux::defineRectArray(buffer, delta[i]);
		if(hits > 0 && currHit < hits){
			if(i > totalStartup[currHit] && i <= totalStartup[currHit]+active[currHit]){
				while(read.get() != '$'); read.ignore(2);
				read.get(buffer, 100, '\n');
				hitComplexity[i] = aux::defineRectArray(buffer, hitbox[i]);
				if(i == totalStartup[currHit]+active[currHit]) currHit++;
			} else {
				hitComplexity[i] = 1;
				hitbox[i] = new SDL_Rect[1];
				hitbox[i][0].x = 0; hitbox[i][0].y = 0; hitbox[i][0].w = 0; hitbox[i][0].h = 0;
			}
		} else {
			hitComplexity[i] = 1;
			hitbox[i] = new SDL_Rect[1];
			hitbox[i][0].x = 0; hitbox[i][0].y = 0; hitbox[i][0].w = 0; hitbox[i][0].h = 0;
		}
	}
	read.close();

	for(int i = 0; i < 5; i++){
		button[i] = 0;
	}
	int r = strlen(n);
	for(int i = 0; i < r; i++){
		switch(n[i]){
		case 'A':
			button[0] = 1;
			break;
		case 'B':
			button[1] = 1;
			break;
		case 'C':
			button[2] = 1;
			break;
		case 'D':
			button[3] = 1;
			break;
		case 'E':
			button[4] = 1;
			break;
		default:
			break;
		}
	}

	SDL_Surface *temp;
	width = new int[frames];
	height = new int[frames];
	sprite = new GLuint[frames];
	for(int i = 0; i < frames; i++){
		sprintf(fname, "%s#%i.png", n, i);
		temp = aux::load_image(fname);
		if(!temp){
			width[i] = 0;
			height[i] = 0;
			sprite[i] = 0;
		} else {
			width[i] = temp->w;
			height[i] = temp->h;
			sprite[i] = aux::surface_to_texture(temp);
		}
	}
}

bool action::setParameter(char * buffer)
{
	char* token = strtok(buffer, "\t: \n");

	if(!strcmp("Name", token)){
		token = strtok(NULL, "\t:\n");
		name = new char[strlen(token)+1];
		sprintf(name, "%s", token);
//		printf(": %s\n", name);
		return 1;
	} else if (!strcmp("Buffer", token)) {
		token = strtok(NULL, "\t: \n");
		tolerance = atoi(token);
		token = strtok(NULL, "\t: \n");
		activation = atoi(token);
//		printf("Buffer: %i : %i\n", tolerance, activation);
		return 1;
	} else if (!strcmp("Hits", token)) {
		token = strtok(NULL, "\t: \n");
		hits = atoi(token);
		if(hits > 0){
			stats = new hStat[hits];
			CHStats = new hStat[hits];
			onConnect = new action*[hits];
			tempOnConnect = new char*[hits];
			for (int i = 0; i < hits; i++){
				onConnect[i] = NULL;
				tempOnConnect[i] = NULL;
				stats[i].hitState.i = 0;
			}
		} else {
			stats = NULL;
			onConnect = NULL;
		}
		state = new cancelField[hits+1];
		gain = new int[hits+1];
		for(int i = 0; i < hits+1; i++)
			gain[i] = 0;
//		printf("Hits: %i\n", hits);
		return 1;
	} else if (!strcmp("Next", token)) {
		token = strtok(NULL, "\t: \n");
		tempNext = new char[strlen(token)+1];
		strcpy(tempNext, token);
		return 1;
	} else if (!strcmp("Attempt", token)) {
		token = strtok(NULL, "\t: \n-");
		attemptStart = atoi(token); 

		token = strtok(NULL, "\t: \n-");
		attemptEnd = atoi(token); 

		token = strtok(NULL, "\t: \n");
		tempAttempt = new char[strlen(token)+1];
		strcpy(tempAttempt, token);
		return 1;
	} else if (!strcmp("Connect", token)) {
		token = strtok(NULL, "\t: \n");
		int x = atoi(token);
		token = strtok(NULL, "\t: \n");
		tempOnConnect[x] = new char[strlen(token)+1];
		strcpy(tempOnConnect[x], token);
		return 1;
	} else if (!strcmp("Blocks", token)) {
		token = strtok(NULL, "\t: \n");
		blockState.i = atoi(token);
//		printf("Blocks: %i\n", blockState.i);
		return 1;
	} else if (!strcmp("Check", token)) {
		token = strtok(NULL, "\t: \n");
		allowed.i = atoi(token);
//		printf("Check: %i\n", allowed.i);
		return 1;
	} else if (!strcmp("Cost", token)) {
		token = strtok(NULL, "\t: \n");
		cost = atoi(token);
//		printf("Cost: %i\n", cost);
		return 1;
	} else if (!strcmp("Frames", token)) {
		token = strtok(NULL, "\t: \n");
		frames = atoi(token);
//		printf("Frames: %i\n", frames);
		int startup, countFrames = -1;
		if(hits > 0) {
			totalStartup = new int[hits];
			active = new int[hits];
		} else {
			totalStartup = NULL;
			active = NULL;
		}

		for(int i = 0; i < hits; i++){
			token = strtok(NULL, "\t: \n");
			startup = atoi(token);
			countFrames += startup;
			totalStartup[i] = countFrames;
			token = strtok(NULL, "\t: \n");
			active[i] = atoi(token);
			countFrames += active[i];
		}
		return 1;
	} else if (!strcmp("State", token)) {
//		printf("State");
		for(int i = 0; i < hits+1; i++){
			token = strtok(NULL, "\t: \n");
			state[i].i = atoi(token);
//			printf(": %i ", state[i].i);
		}
//		printf("\n");
		return 1;
	} else if (!strcmp("HitAllows", token)) {
//		printf("HitAllows");
		for(int i = 0; i < hits; i++){
			token = strtok(NULL, "\t: \n");
			stats[i].hitState.i = atoi(token);
//			printf(": %i ", hitState[i].i);
		}
//		printf("\n");
		return 1;
	} else if (!strcmp("Damage", token)) {
//		printf("Damage");
		for(int i = 0; i < hits; i++){
			token = strtok(NULL, "\t: \n");
			stats[i].damage = atoi(token);
//			printf(": %i ", stats[i].damage);
		}
//		printf("\n");
		return 1;
	} else if (!strcmp("Push", token)) {
//		printf("Push");
		for(int i = 0; i < hits; i++){
			token = strtok(NULL, "\t: \n");
			stats[i].push = atoi(token);
//			printf(": %i ", stats[i].push);
		}
//		printf("\n");
		return 1;
	} else if (!strcmp("Lift", token)) {
//		printf("Lift");
		for(int i = 0; i < hits; i++){
			token = strtok(NULL, "\t: \n");
			stats[i].lift = atoi(token);
//			printf(": %i ", stats[i].lift);
		}
//		printf("\n");
		return 1;
	} else if (!strcmp("Float", token)) {
//		printf("Float");
		for(int i = 0; i < hits; i++){
			token = strtok(NULL, "\t: \n");
			stats[i].hover = atoi(token);
//			printf(": %i ", stats[i].hover);
		}
//		printf("\n");
		return 1;
	} else if (!strcmp("Blowback", token)) {
//		printf("Blowback");
		for(int i = 0; i < hits; i++){
			token = strtok(NULL, "\t: \n");
			stats[i].blowback = atoi(token);
//			printf(": %i ", stats[i].blowback);
		}
//		printf("\n");
		return 1;
	} else if (!strcmp("Stun", token)) {
//		printf("Stun");
		for(int i = 0; i < hits; i++){
			token = strtok(NULL, "\t: \n");
			stats[i].stun = atoi(token);
			CHStats[i].stun = stats[i].stun / 2 + stats[i].stun;
//			printf(": %i ", stats[i].stun);
		}
//		printf("\n");
		return 1;
	} else if (!strcmp("Untech", token)) {
//		printf("Untech");
		for(int i = 0; i < hits; i++){
			token = strtok(NULL, "\t: \n");
			stats[i].untech = atoi(token);
			CHStats[i].untech = 10;
//			printf(": %i ", stats[i].untech);
		}
//		printf("\n");
		return 1;
	} else if (!strcmp("Blockable", token)) {
//		printf("Blockable");
		for(int i = 0; i < hits; i++){
			token = strtok(NULL, "\t: \n");
			stats[i].blockMask.i = atoi(token);
//			printf(": %i ", stats[i].blockMask.i);
		}
//		printf("\n");
		return 1;
	} else if (!strcmp("Gain", token)) {
//		printf("Gain");
		for(int i = 0; i < hits+1; i++){
			token = strtok(NULL, "\t: \n");
			gain[i] = atoi(token);
//			printf(": %i ", gain[i]);
		}
//		printf("\n");
		return 1;
	} else if (!strcmp("Autoguard", token)) {
		token = strtok(NULL, "\t: \n-");
		guardStart = atoi(token); 

		token = strtok(NULL, "\t: \n-");
		guardLength = atoi(token); 
		guardLength = guardLength - guardStart;
		return 1;
	} else if (!strcmp("Armor", token)) {
		token = strtok(NULL, "\t: \n-");
		armorStart = atoi(token); 

		token = strtok(NULL, "\t: \n-");
		armorLength = atoi(token); 
		armorLength = armorLength - armorStart;
		return 1;
	} else if (!strcmp("MaxArmor", token)) {
		token = strtok(NULL, "\t: \n-");
		armorHits = atoi(token); 
		return 1;
	} else return 0;
}

void action::parseProperties(char * buffer)
{
	char * token = strtok(buffer, " \t\n:");
	token = strtok(NULL, "\n");
	/*Debug*/
//	printf("%s properties: %s\n", name, buffer);
	int ch = 0;
//	printf("%s: ", name);
	for(unsigned int i = 0; i < strlen(token); i++){
//		printf("%c ", token[i]);
		switch(token[i]){
		case '^': 
			stats[ch].launch = 1;
			break;
		case 'g':
			stats[ch].ghostHit = 1;
			break;
		case '>':
			stats[ch].wallBounce = 1;
			break;
		case 'v':
			stats[ch].floorBounce = 1;
			break;
		case '_':
			stats[ch].slide = 1;
			break;
		case 's':
			stop = 1;
			break;
		case 'S': 
			stop = 2;
			break;
		case 'c':
			crouch = 1;
			break;
		case ':':
			ch++;
			break;
		case 't':
			throwinvuln = 1;
			break;
		case 'T':
			throwinvuln = 2;
			break;
		case '=':
			stats[ch].stick = 1;
			break;
		case 'p':
			stats[ch].hitsProjectile = 1;
			break;
		case 'd':
			dies = 1;
			break;
		case 'C':
			fch = 1;
			break;
		default:
			break;
		}
	}
//	printf("\n");
}

bool action::window(int f)
{
	if(!attempt) return 0;
	if(f < attemptStart) return 0;
	if(f > attemptEnd) return 0;
	return 1;
}

bool action::check(bool pos[5], bool neg[5], int t, int f, int resource[], SDL_Rect &p)
{
	for(int i = 0; i < 5; i++){
		if(button[i] == 1){
			if(!pos[i]) return 0;
		}
	}
	if(t > tolerance) return 0;
	if(f > activation) return 0;
	if(cost > resource[0]) return 0;
	return check(p);
}

bool action::check(SDL_Rect &p)
{
	return 1;
}

void action::pollRects(SDL_Rect &c, SDL_Rect* &r, int &rc, SDL_Rect* &b, int &hc, int f, int cFlag)
{
	if(f >= frames) return;
	if(rc > 0 && r) delete [] r;
	if(hc > 0 && b) delete [] b;
	rc = regComplexity[f];
	hc = hitComplexity[f];
	r = new SDL_Rect[rc];
	b = new SDL_Rect[hc];

	c.x = collision[f].x; c.w = collision[f].w;
	c.y = collision[f].y; c.h = collision[f].h;

	SDL_Rect * tempreg = hitreg[f];
	for(int i = 0; i < rc; i++){
		r[i].x = tempreg[i].x; r[i].w = tempreg[i].w;
		r[i].y = tempreg[i].y; r[i].h = tempreg[i].h;
	}
	SDL_Rect * temphit = hitbox[f];
	for(int i = 0; i < hc; i++){
		if(cFlag > calcCurrentHit(f)) {
			b[i].x = 0; b[i].w = 0;
			b[i].y = 0; b[i].h = 0;
		} else {
			b[i].x = temphit[i].x; b[i].w = temphit[i].w;
			b[i].y = temphit[i].y; b[i].h = temphit[i].h;
		}
	}
}

void action::pollStats(hStat & s, int f, bool CH)
{
	int c = calcCurrentHit(f);
	s.damage = stats[c].damage + CHStats[c].damage * CH;
	s.stun = stats[c].stun + CHStats[c].stun * CH;
	s.push = stats[c].push + CHStats[c].push * CH;
	s.lift = stats[c].lift + CHStats[c].lift * CH;
	s.untech = stats[c].untech + CHStats[c].untech * CH;
	if(CH){
		s.launch = CHStats[c].launch;
		s.hover = CHStats[c].hover;
		s.wallBounce = CHStats[c].wallBounce;
		s.floorBounce = CHStats[c].floorBounce;
		s.slide = CHStats[c].slide;
		s.stick = CHStats[c].stick;
		s.ghostHit = CHStats[c].ghostHit;
		s.blowback = CHStats[c].blowback;
	} else {
		s.launch = stats[c].launch;
		s.hover = stats[c].hover;
		s.wallBounce = stats[c].wallBounce;
		s.floorBounce = stats[c].floorBounce;
		s.slide = stats[c].slide;
		s.stick = stats[c].stick;
		s.ghostHit = stats[c].ghostHit;
		s.blowback = stats[c].blowback;
	}
	s.blockMask.i = stats[c].blockMask.i;
}

bool action::cancel(action * x, int& c, int &h)
{
	cancelField r;
	r.i = x->state[c].i;
	if(h > 0 && h == c){ 
		r.i = r.i + x->stats[h - 1].hitState.i;
	}
	if(x == NULL) return 1;
	else{
		if(allowed.i & r.i){
//			if(r.i > 1) printf("%i allows %i\n", r.i, allowed.i);
			if(x == this){
				if(c == 0) return 0;
				else if(allowed.i & 4) return 1;
				else return 0;
			}
			else return 1;
		}
	}
	return 0;
}

void action::step(int *& resource, int &f)
{
	if(f == 0){
		if(resource[0] + gain[0] < 300) resource[0] += gain[0];
		else resource[0] = 300;
	}
	f++;
}

int action::calcCurrentHit(int frame)
{
	int b = 0;
	for(int i = 0; i < hits; i++){
		if(frame > totalStartup[i]) b = i;
	} 
	return b;
}

action * action::connect(int *& resource, int &c, int f)
{
	c = calcCurrentHit(f)+1;
	if(resource[0] + gain[c] < 300) resource[0] += gain[c];
	else resource[0] = 300;
	if(onConnect[c-1] != NULL){
		return onConnect[c-1];
	}
	else return NULL;
}

action * action::blockSuccess(int st)
{
	return this;
}

void action::execute(action * last, int *& resource)
{
	resource[0] -= cost;
}

void action::feed(action * c, int code, int i)
{
	switch(code){
	case 0:
		next = c;
		if(tempNext) delete [] tempNext;
		break;
	case 2:
		onConnect[i] = c;
		if(tempOnConnect[i]) delete [] tempOnConnect[i];
//		printf("%s-%i: %s\n", name, i, onConnect[i]->name);
		break;
	case 3:
		attempt = c;
		if(tempAttempt) delete [] tempAttempt;
		break;
	}
}

char * action::request(int code, int i)
{
	switch(code){
	case 0: 
		return tempNext;
	case 2: 
		return tempOnConnect[i];
	case 3:
		return tempAttempt;
	default:
		return NULL;
	}
}

int action::takeHit(hStat & s, int b, int &f, int &c, int &h)
{
	if(s.blockMask.i & blockState.i && f > guardStart && f < guardStart + guardLength)
		return 0;
	else if (f > armorStart && f < armorStart + armorLength){
		s.stun = 0;
		return 1;
	} else {
		f = 0;
		c = 0;
		h = 0;
		return 1;
	}
}

bool action::CHState(int f)
{
	if(hits < 1) return false;
	else if(f < totalStartup[hits-1]) return true;
	else return fch;
}

