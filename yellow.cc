#include "yellow.h"
yellow::yellow(){
	head = new actionTrie;
	airHead = new actionTrie;
	meter = new int[4];
	build("Yellow", "Yellow");
}

void yellow::resetAirOptions()
{
	meter[1] = 1;
	meter[2] = 2;
}

void yellow::init()
{
	character::init();
	meter[3] = 0;
}

void yellow::tick()
{
	character::tick();
	if(meter[3] > 0){ 
		meter[3]--;
		if(meter[3] == 0){ 
			meter[3] = -360;
		}
	}
}

bool yellow::step(int &f)
{
	if(meter[3] < 0) meter[3]++;
	return character::step(f);
}

action * yellow::createMove(char * fullName)
{
	char * token;
	char type[2] = {fullName[0], fullName[1]};
	char actionName[151];
	char buffer[101];
	strcpy (buffer, fullName);

	token = strtok(fullName, " \t-@?_%&$!\n");
	sprintf(actionName, "%s/%s", name, token);

	action * m;
	switch(type[0]){
	case '$':
		m = new flashStep(actionName);
		break;
	case '&':
		m = new flashSummon(actionName);
		break;
	default:
		m = character::createMove(buffer);
		break;
	}
	return m;
}

void yellow::drawMeters(int ID)
{
	int color;
	character::drawMeters(ID);
	SDL_Rect c1;
	if(meter[3] >= 0){
		c1.w = meter[3]/3; 
		color = 255;
	} else {
		c1.w = 180 + (meter[3]/2);
		color = 0;
	}
	if(ID == 1){
		c1.x = 110; 
	} else { 
		c1.x = 510 + (180 - c1.w);
	}
	c1.h = 5;
	c1.y = 438;
	glColor4f(1.0f, (float)color, 0.0f, 1.0f);
	glRectf((GLfloat)(c1.x), (GLfloat)(c1.y), (GLfloat)(c1.x + c1.w), (GLfloat)(c1.y + c1.h));
//	SDL_FillRect(screen, &c1, SDL_MapRGB(screen->format, 0, 0, color1));
//	SDL_FillRect(screen, &c2, SDL_MapRGB(screen->format, color2, 0, color2)); 
}

int yellow::takeHit(hStat & s, int b, int &f)
{
	int x = character::takeHit(s, b, f);
	if(x == 1 && meter[3] > 0) meter[3] = 0;
	return x;
}

flashStep::flashStep() {}
flashSummon::flashSummon() {}

flashStep::flashStep(const char * n)
{
	build(n);
	init();
}

flashSummon::flashSummon(const char * n)
{
	build(n);
	init();
}

bool flashStep::setParameter(char * buffer)
{
	char savedBuffer[100];
	strcpy(savedBuffer, buffer);

	char * token = strtok(buffer, "\t: \n-");

	if(!strcmp("FlashCost", token)){
		token = strtok(NULL, "\t: \n-");
		flashMeterCost = atoi(token); 
		return 1;
	} else return airMove::setParameter(savedBuffer);
}

bool flashSummon::setParameter(char * buffer)
{
	char savedBuffer[100];
	strcpy(savedBuffer, buffer);

	char * token = strtok(buffer, "\t: \n-");
	
	if(!strcmp("FlashGain", token)){
		token = strtok(NULL, "\t: \n-");
		flashMeterGain = atoi(token); 
		return 1;
	} else return action::setParameter(savedBuffer);
}

bool flashStep::check(bool pos[5], bool neg[5], int t, int f, int* resource, SDL_Rect& p)
{
	if(!action::check(pos, neg, t, f, resource, p)) return 0;
	if(resource[3] < 1) return 0;
	return 1;
}

bool flashSummon::check(bool pos[5], bool neg[5], int t, int f, int* resource, SDL_Rect& p)
{
	if(!special::check(pos, neg, t, f, resource, p)) return 0;
	if(resource[3] < 0) return 0;
	return 1;
}

void flashSummon::execute(action * last, int *& resource)
{
	if(resource[3] > 0) uFlag = 1;
	else uFlag = 0;
	action::execute(last, resource);
}

void flashStep::execute(action * last, int *& resource)
{
	resource[3] -= flashMeterCost;
	if(resource[3] > 540) resource[3] = 540;
	else if(resource[3] < 0) resource[3] = -360;
	action::execute(last, resource);
}

void flashSummon::step(int *& resource, int f)
{
	if(uFlag){
		if(f == frames - 1) resource[3] = 0;
	} else resource[3] += flashMeterGain / frames + 1;
	if(resource[3] > 540) resource[3] = 540;
	action::step(resource, f);
}
