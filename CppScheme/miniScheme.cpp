//#define DEBUG_TOKEN
/*
#define _CRTDBG_MAP_ALLOC

#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW

#endif	// DBG_NEW
#endif  // _DEBUG
#include <stdlib.h>
#include <crtdbg.h>*/
#include <iostream>
#include <windows.h>
#include "Parser.h"
#include <fstream>
#include "build_in.h"
#include <xtree>
#include "gc.h"

using namespace CppScheme;

using std::cin;
using std::cerr;
using std::endl;
using std::cout;

void init(EnvTree* env);
void release(EnvTree*);

void mainloop(EnvTreeList env, std::istream& input){
	while (true){
		std::cout << "Scheme " << current_line_number << ">> ";
		cur_char = input.get();
		ExpAST* expr = parseExpAst(input, env);

		if (expr){
			if (expr->_exp_type == ExpAST_TYPE::EXIT_TYPE){
				delete expr;
				break;
			}
			Object* ret = expr->eval(env);
			if (ret) {
				switch (ret->obtype)
				{
				case ObjectType::BOOLOBJ:
				{
					BoolValue* ptr = (BoolValue*)ret;
					if (ptr->value){
						std::cout << "#t" << std::endl;
					}
					else{
						std::wcout << "#f" << std::endl;
					}
					break;
				}
				case ObjectType::BuiltInOBJ:
				case ObjectType::PROCEDURE:
				{
					if (expr->_exp_type == ExpAST_TYPE::VARIABLE_TYPE){
						VariableExp* var_ptr = (VariableExp*)expr;
						cout << "#<Procedure: " << var_ptr->var_name << ">" << endl;
					}
					else{
						cout << "#<Procedure>" << endl;
					}
					break;
				}
				case ObjectType::DOUBLEOBJ:
				{
					DoubleValue* ptr = (DoubleValue*)ret;
					cout << ptr->value << endl;
					break;
				}
				default:
					break;
				}
			}

		}
		if (current_tok == TOKEN::TEOF){
			break;
		}
		mark_sweep();
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

void init_env(){
	GlobalVariable = new EnvTree();
	Env = Env.push_front(GlobalVariable);

	init(GlobalVariable);
}


void clear_env(){
	for (auto iter = GlobalVariable->begin(); iter != GlobalVariable->end(); ++iter) {
		if (iter->second->obtype == ObjectType::BuiltInOBJ){
			delete iter->second;
		}
	}

	for (auto iter = ExpAST_pool.begin(); iter != ExpAST_pool.end(); ++iter) {
		auto ptr = iter->first;
		delete ptr;
	}

	for (auto iter = Object_pool.begin(); iter != Object_pool.end(); ++iter) {
		auto ptr = iter->first;
		delete ptr;
	}

	Env.clearLocal();
}


int main(){

	init_env();

	std::ifstream file_in("E:\\BaiduDisk\\Code\\Compiler\\PGWT\\CppScheme\\Data\\input.txt");
	//mainloop(Env, file_in);

	mainloop(Env, std::cin);


	file_in.close();
	clear_env();
	cout << "ending construct" << endl;
	
	//_CrtDumpMemoryLeaks();
	return 0;
}