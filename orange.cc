#include "interface.h"
orange::orange()
{
	head = new actionTrie;
	airHead = new actionTrie;
	build("Orange", "Orange");
}

void orange::init(action *& cMove)
{
	character::init(cMove);
	matrixComplexity = 0;
	matrix = NULL;
}

void orange::step(action *& cMove, int &f, int &freeze)
{
	for(int i = 0; i < matrixComplexity; i++){
		if(matrix[i]->dead) cull(i);
	}
	character::step(cMove, f, freeze);
}

void orange::cull(int q)
{
	for(int i = q; i < matrixComplexity - 1; i++)
		matrix[i] = matrix[i+1];
	matrixComplexity--;
}

instance * orange::spawn(action * source)
{
	instance * n = source->spawn();
	if(!n) return NULL;
	else {
		instance ** temp;
		if(matrixComplexity < 7) temp = new instance*[matrixComplexity+1];
		else{
			temp = new instance*[7];
			matrix[6]->dead = true;
		}
		int i;
		for(i = matrixComplexity; i > 0; i--){
			if(i != 7) temp[i] = matrix[i - 1];
		}
		temp[0] = n;
		if(matrix) delete [] matrix;
		matrix = temp;
		if(matrixComplexity < 7) matrixComplexity++;
		return matrix[0];
	}
}

int orange::takeHit(action *& cMove, hStat & s, int b, int &f, int &c, int &h, int &p)
{
	int freeze = character::takeHit(cMove, s, b, f, c, h, p);
	if(p == 1){ 
		for(int i = 0; i < matrixComplexity; i++){
			if(matrix[i]->hitComplexity <= 1 && matrix[i]->hitbox[i].w < 1)
				matrix[i]->dead = true;
		}
	}
	return freeze;
}
