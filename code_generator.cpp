#include<iostream>
#include<fstream>
#include <iomanip>
#include <string>
#include <string.h>
#include "table.h"

using namespace std;

string operators[8] = { "*", "+", "-", "=" , "/", ">", "<", "%" };
string separator[10] = { "'", "(", ")", "{", "}", "[", "]", ",", ":", ";" };
string keywords[7] = {"int", "bool", "if", "else", "while", "input", "output" };

ifstream fin;		//read file
char ch;	// read file and get every character
string tmp_word = "";

int memory_loaction = 5000;
int current_memory_loaction = memory_loaction;
SymbolTable st[100];
string existId[100] = {""};
InstructionTable itable[100];
string last_assigned_id = "";
int id_counter = 0;
int st_counter = 0;
int it_counter = 0;
int jump_index = 0;
int instruction_counter = 0;

void I();
void A(string);
void lexer(string);

int find_index(string word) {
	for (int i = 0; i < st_counter; i++) {
		if (st[i].id == word) {
			return	i;
		}
	}
	return -1;
}

bool isNumber(string word) {
	if (isdigit(word[0]))
		return true;
	else
		return false;
}

bool isBoolean(string word) {
	if (word == "true" || word == "false") {
		return true;
	}
	else 
		return false;
}
bool isKeywords(string word) {
	for (int i = 0; i < 7; i++) {
		if (keywords[i] == word)
			return true;
	}
	return false;
}

bool isIdentifier(string word) {
	for (int i = 0; i <= id_counter; i++) {
		if (existId[i] == word) {
			return true;
		}
	}
	return false;
}

string get_next_token() {
	string token = "";
	do {
		ch = fin.get();
		if (ch != ' ' && ch != ';' && ch != ',' && ch != '\n' && ch != '@')
			token += ch;
	} while (ch != ' ' && ch != '@' && ch != '\n');

	return token;
}

void gen_instr(string op, int oprnd) {
	if (it_counter != 0) {
		itable[it_counter].step = itable[it_counter - 1].step + 1;
	}
	itable[it_counter].Op = op;
	itable[it_counter].Oprnd = oprnd;
	it_counter++;
	instruction_counter++;
}

int get_address(string word) {
	for (int i = 0; i < st_counter; i++) {
		if (st[i].id == word) {
			return st[i].memoryLocation;
		}
	}

	return 0;
}

void F(string token) {
	if (isIdentifier(token)) {
		gen_instr("PUSHM", get_address(token));
		
	}
	else if (isNumber(token)) {
		gen_instr("PUSHI", stoi(token));
	}
	else {
		cout << "id expected" << endl;
	}
}

void T(string token) {
	F(token);
}

void Ep() {
	string tokenEp = "";
	if (ch != fin.eof()) {
		tokenEp = get_next_token();
	}
	if (tokenEp == "+") {
		tokenEp = get_next_token();
		T(tokenEp);
		gen_instr("ADD", -1);
	}
	else if (tokenEp == "-") {
		tokenEp = get_next_token();
		T(tokenEp);
		gen_instr("SUB", -1);
	}
	else if (tokenEp == "*") {
		tokenEp = get_next_token();
		T(tokenEp);
		gen_instr("MUL", -1);
	}
	else if (tokenEp == "/") {
		tokenEp = get_next_token();
		T(tokenEp);
		gen_instr("DIV", -1);
	}
}

void E() {
	string tokenE = get_next_token();
	if (isIdentifier(tokenE)) {
		T(tokenE);
		Ep();
	}
	else if (isBoolean(tokenE)) {
		if(tokenE == "true")
			gen_instr("PUSHI", 1);
		else
			gen_instr("PUSHI", 0);
	}
	else if (isNumber(tokenE)){
		int number = stoi(tokenE);
		gen_instr("PUSHI", number);
	}
	else {
		cout << " Syntax error, receive unknow id:" << tokenE << endl;
		system("pause");
		exit(0);
	}


}

void C() {
	string tokenC = get_next_token();
	last_assigned_id = tokenC;
		F(tokenC);
		string firstToken = tokenC;
		tokenC = get_next_token();
		if (tokenC == ">") {
			tokenC = get_next_token();
			F(tokenC);
			gen_instr("GRT", -1);
			jump_index = it_counter;
			gen_instr("JUMPZ", -1);
			instruction_counter = 0;
		}
		else if (tokenC == "<") {
			tokenC = get_next_token();
			F(tokenC);
			gen_instr("LES", -1);
			jump_index = it_counter;
			gen_instr("JUMPZ", -1);
			instruction_counter = 0;
		}
		else if (tokenC == "==") {
			tokenC = get_next_token();
			F(tokenC);
			gen_instr("EQU", -1);
			jump_index = it_counter;
			gen_instr("JUMPZ", -1);
			instruction_counter = 0;
		}
		else if (tokenC == "!=") {
			tokenC = get_next_token();
			F(tokenC);
			gen_instr("NEQ", -1);
			jump_index = it_counter;
			gen_instr("JUMPZ", -1);
			instruction_counter = 0;
		}
		else if (tokenC == ">=") {
			tokenC = get_next_token();
			F(tokenC);
			gen_instr("GEQ", -1);
			jump_index = it_counter;
			gen_instr("JUMPZ", -1);
			instruction_counter = 0;
		}
		else if (tokenC == "<=") {
			tokenC = get_next_token();
			F(tokenC);
			gen_instr("LEQ", -1);
			jump_index = it_counter;
			gen_instr("JUMPZ", -1);
			instruction_counter = 0;
		}
	
}

void I() {
	string tokenI = get_next_token();
	if (tokenI == "(")
		C();
	tokenI = get_next_token();
	if (tokenI == ")") {
		tokenI = get_next_token();
		if (tokenI == "{") {
			tokenI = get_next_token();
			do{
				lexer(tokenI);
				tokenI = get_next_token();
			} while (tokenI != "}");
			itable[jump_index].Oprnd = itable[jump_index].step + instruction_counter + 1;
		}
	}
}

void ELSE() {
	string tokenE = get_next_token();
	if (tokenE == "{") {
		tokenE = get_next_token();
		do {
			lexer(tokenE);
			tokenE = get_next_token();
		} while (tokenE != "}");
		itable[jump_index].Oprnd = itable[jump_index].step + instruction_counter + 1;
	}
}

void A(string word) {
	string save = word;
	last_assigned_id = word;
	string tokenA = get_next_token();
	
	if (tokenA == "=") { // assign operation
		E();
		gen_instr("POPM", get_address(save));

	}
	else {
		cout << " = expected but receive " << tokenA << endl;
		system("pause");
		exit(0);
	}
}

void W() {
	string tokenW = get_next_token();
	if (tokenW == "(") {
		int jump_back_index = it_counter;
		C();
		tokenW = get_next_token();
		if (tokenW == ")") {
			tokenW = get_next_token();
			if (tokenW == "{") {
				tokenW = get_next_token();
				do {
					lexer(tokenW);
					tokenW = get_next_token();
				} while (tokenW != "}");
				gen_instr("JUMP", jump_back_index);
				itable[jump_index].Oprnd = itable[jump_index].step + instruction_counter + 1;
			}
		}	
	}
}

void INPUT() {
	string tokenI = get_next_token();
	gen_instr("STDIN", -1);
	if (tokenI == "(") {
		tokenI = get_next_token();
		gen_instr("POPM", get_address(tokenI));
		tokenI = get_next_token(); // get ')'
		if (tokenI != ")") {
			cout << "Error, Expect )" << endl;
			exit(0);
		}
	}
}

void OUTPUT() {
	string tokenO = get_next_token();
	if (tokenO == "(") {
		tokenO = get_next_token();
		F(tokenO);
		tokenO = get_next_token();
		gen_instr("STDOUT", -1);
		if (tokenO != ")") {
			cout << "Error, Expect )" << endl;
			exit(0);
		}
		
	}
}

void lexer(string word) {
	if (isKeywords(word)) {
		if(word == "int" || word == "bool")
			st[st_counter].type = word;
		else if(word == "if"){
			I();
		}
		else if (word == "else") {
			ELSE();
		}
		else if (word == "while") {
			gen_instr("LABEL", -1);
			W();
		}
		else if (word == "input") {
			INPUT();
		}
		else if (word == "output") {
			OUTPUT();
		}
	}
	else if (!isIdentifier(word)) {
		existId[id_counter] = word;
		id_counter++;
		st[st_counter].id = word;
		st[st_counter].memoryLocation = current_memory_loaction;
		st_counter++;
		current_memory_loaction++;
		if (st[st_counter].type == "") {
			st[st_counter].type = st[st_counter - 1].type;
		}
	}
	else if (isIdentifier(word)) {
		A(word);
	}
	tmp_word = "";
}

int main() {
	st[0].memoryLocation = memory_loaction;

	string filename;
	cout << "Please enter file name where you want to read: ";
	cin >> filename;
	fin.open(filename);
	if (!fin.is_open()) {
		cout << "error while opening the file\n";
		exit(0);
	}
	while (!fin.eof()) {
		tmp_word = get_next_token();
		if (ch == '@') {
			break;
		}
		lexer(tmp_word);
		
	}
	
	cout << endl << endl;
	cout << "Instruction Table" << endl;
	cout << "------------------------------------------------------------------------------------------" << endl;
	cout << "Step				|	Op				|	Oprnd" << endl;
	cout << "------------------------------------------------------------------------------------------" << endl;
	for (int i = 0; i < it_counter; i++) {
		cout << itable[i].step << "				|	" << itable[i].Op << "				|	" << itable[i].Oprnd << endl;
		cout << "------------------------------------------------------------------------------------------" << endl;
	}

	cout << endl << endl;
	cout << "Symbol Table" << endl;
	cout << "-------------------------------------------------------------------------------------------------------------" << endl;
	cout << "Identifier			|	MemoryLocation			|	Type"			 << endl;
	cout << "-------------------------------------------------------------------------------------------------------------" << endl;
	for (int i = 0; i < st_counter; i++) {
		cout << st[i].id << "				|	" << st[i].memoryLocation << "				|	" << st[i].type << endl;
		cout << "-------------------------------------------------------------------------------------------------------------" << endl;
	}

	system("pause");
	return 0;
}