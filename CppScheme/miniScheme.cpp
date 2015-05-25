//#define DEBUG_TOKEN
#define _CRTDBG_MAP_ALLOC
#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif  // _DEBUG
#include <stdlib.h>
#include <crtdbg.h>
#include <iostream>
#include <windows.h>
#include "Parser.h"
#include <fstream>
#include "build_in.h"
#include <xtree>
using namespace CppScheme;
using std::cin;
using std::cerr;
using std::endl;
using std::cout;


void printCurrentEnv(EnvTreeList& env){
	auto cur = env.head;

	while (cur) {
		cout << "********************** current_enviroment *****************" << endl;
		for (auto iter = cur->ptr_to_data->begin(); iter != cur->ptr_to_data->end(); ++iter) {
			cout << iter->first << "   " << endl;
		}
		cur = cur->next;
	}
}

void init(EnvTree* env);
void release(EnvTree*);

void mainloop(EnvTreeList env, std::istream& input){
	init(GlobalVariable);
	std::cout << "Scheme " << current_line_number << ">> ";
	cur_char = input.get();
	ExpAST* expr = parseExpAst(input, env);

	if (expr){
		if (Object* ret = expr->eval(env)){
			if (DoubleValue* ptr = dynamic_cast<DoubleValue*>(ret)){
				cout << ptr->value << endl;
			}
			else if (Procedure* ptr = dynamic_cast<Procedure*>(ret)){
				if (VariableExp* var_ptr = dynamic_cast<VariableExp*>(expr)){
					cout << "#<Procedure: " << var_ptr->var_name << ">" << endl;
				}
				else{
					cout << "#<Procedure>" << endl;
				}
			}
			else if (BoolValue* ptr = dynamic_cast<BoolValue*>(ret)){
				if (ptr->value){
					cout << "#t" << endl;
				}
				else{
					cout << "#f" << endl;
				}
			}
			delete ret;
		}
		delete expr;
	}
}

//initialize the builtin functions
void init(EnvTree* env){
	(*env)["+"] = new Add();
	(*env)["*"] = new Mul();
	(*env)["/"] = new Div();
	(*env)["-"] = new Sub();
	(*env)["cons"] = new Cons();
	(*env)["car"] = new Car();
	(*env)["cdr"] = new Cdr();
	(*env)["list"] = new List();
	(*env)["<"] = new Less();
	(*env)[">"] = new Greater();
	(*env)["="] = new Equal();
	(*env)["remainder"] = new Remainder();
	(*env)["and"] = new And();
	(*env)["or"] = new Or();
	(*env)["not"] = new Not();
	(*env)["even?"] = new IsEven();
	(*env)["square"] = new Square();
	(*env)["display"] = new Display();

}


void release(EnvTree* env){
	for (auto iter = env->begin(); iter != env->end(); ++iter) {
		delete iter->second;
	}
}


int main(){

	GlobalVariable = new EnvTree();
	Env = Env.push_front(GlobalVariable);


	std::ifstream file_in("E:\\BaiduDisk\\Code\\Compiler\\PGWT\\CppScheme\\Data\\input.txt");

	mainloop(Env, std::cin);




	Env.clearLocal();

	file_in.close();


	cout << "ending construct" << endl;
	_CrtDumpMemoryLeaks();
	return 0;
}