#include "name.h"
name::name(){
	head = new actionTrie;
	airHead = new actionTrie;
	meter = new int[3];
	build("Name", "Name");
}
