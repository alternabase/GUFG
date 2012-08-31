#include "black.h"
black::black(){
	head = new actionTrie;
	airHead = new actionTrie;
	meter = new int[3];
	build("black", "black");
}
