#pragma once
#include <string>

using namespace std;

struct SymbolTable {
	string id;
	int memoryLocation;
	string type;
};

struct InstructionTable {
	int step = 1;
	string Op;
	int Oprnd;
};