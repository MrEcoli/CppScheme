//#define DEBUG_TOKEN

#define _CRTDBG_MAP_ALLOC

#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif	// DBG_NEW
#endif  // _DEBUG

#include <stdlib.h>
#include <crtdbg.h>
#include <iostream>
#include <windows.h>
#include "Parser.h"
#include "build_in.h"
#include "gc.h"
#include <iosfwd>
#include <fstream>

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
				mark_sweep();
				break;
			}

			Object* ret = expr->eval(env);

			if (ret) {
				switch (ret->obtype)
				{
				case ObjectType::BOOL_OBJ:
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
				case ObjectType::BuiltIn_OBJ:
				case ObjectType::PROCEDURE_OBJ:
				{
					if (expr->_exp_type == ExpAST_TYPE::VARIABLE_TYPE){
						VariableExp* var_ptr = (VariableExp*)expr;
						cout << "#<Procedure: " << var_ptr->var_name << ">" << endl;
					}
					else{
						std::cout << "#<Procedure>" << std::endl;
					}
					break;
				}
				case ObjectType::DOUBLE_OBJ:
				{
					DoubleValue* ptr = (DoubleValue*)ret;
					std::cout << ptr->value << std::endl;
					break;
				}
				case ObjectType::INTEGER_OBJ:
				{
					IntegerValue* ptr = (IntegerValue*)ret;
					std::cout << ptr->value << std::endl;
					break;
				}
				case ObjectType::PAIR_OBJ:
					std::cout << ret->to_string() << std::endl;
					break;
				default:
					break;
				}
			}

		}
		if (current_tok == TOKEN::EOF_TOK){
			break;
		}
		mark_sweep();
	}
}


void load_file(EnvTreeList env, std::ifstream &input){
	while (true){
		cur_char = input.get();
		ExpAST* expr = parseExpAst(input, env);
		if (expr){
			if (expr->_exp_type == ExpAST_TYPE::EXIT_TYPE){
				delete expr;
				mark_sweep();
				break;
			}

			Object* ret = expr->eval(env);

			if (ret) {
				switch (ret->obtype)
				{
				case ObjectType::BOOL_OBJ:
				{
					BoolValue* ptr = (BoolValue*)ret;
					if (ptr->value){
						std::cout << "#t" << std::endl;
					}
					else{
						std::cout << "#f" << std::endl;
					}
					break;
				}
				case ObjectType::BuiltIn_OBJ:
				case ObjectType::PROCEDURE_OBJ:
				{
					if (expr->_exp_type == ExpAST_TYPE::VARIABLE_TYPE){
						VariableExp* var_ptr = (VariableExp*)expr;
						cout << "#<Procedure: " << var_ptr->var_name << ">" << endl;
					}
					else{
						std::cout << "#<Procedure>" << std::endl;
					}
					break;
				}
				case ObjectType::DOUBLE_OBJ:
				{
					DoubleValue* ptr = (DoubleValue*)ret;
					std::cout << ptr->value << std::endl;
					break;
				}
				case ObjectType::INTEGER_OBJ:
				{
					IntegerValue* ptr = (IntegerValue*)ret;
					std::cout << ptr->value << std::endl;
				}
				case ObjectType::PAIR_OBJ:
					std::cout << "'" <<ret->to_string() << std::endl;
					break;
				case ObjectType::NULL_OBJ:
					std::cout << "'()" << std::endl;
					break;
				case ObjectType::SYMBOL_OBJ:
				{
					SymbolValue* ptr = (SymbolValue*)ret;
					if (ptr->_symbol == "'"){
						std::cout << "'quote" << std::endl;
					}
					else if (ptr->_symbol == "`"){
						std::cout << "'quasiquote" << std::endl;
					}
					else if (ptr->_symbol == ","){
						std::cout << "'unquote" << std::endl;
					}
					else{
						std::cout << ptr->_symbol << std::endl;
					}
				}
				default:
					break;
				}
			}

		}
		if (current_tok == TOKEN::EOF_TOK){
			break;
		}
	}
	mark_sweep();
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
	(*env)["pair?"] = new IsPair();
	(*env)["length"] = new Length();
	(*env)["<"] = new Less();
	(*env)["<="] = new Less_equal();
	(*env)[">="] = new Greater_equal();
	(*env)[">"] = new Greater();
	(*env)["="] = new Equal();
	(*env)["!="] = new Not_Equal();
	(*env)["remainder"] = new Remainder();
	(*env)["and"] = new And();
	(*env)["or"] = new Or();
	(*env)["not"] = new Not();
	(*env)["even?"] = new IsEven();
	(*env)["square"] = new Square();
	(*env)["display"] = new Display();
	(*env)["random"] = new Random();
	(*env)["newline"] = new Newline();
	(*env)["number?"] = new isNumber();
	(*env)["integer?"] = new isInteger();
	(*env)["eq?"] = new Equal_address();

}


void release(EnvTree* env){
	for (auto iter = env->begin(); iter != env->end(); ++iter) {
		delete iter->second;
	}
}

void init_env(){
	SetConsoleCP(437);
	GlobalVariable = new EnvTree();
	Env = Env.push_front(GlobalVariable);
	init(GlobalVariable);
}


void clear_env(){
	for (auto iter = GlobalVariable->begin(); iter != GlobalVariable->end(); ++iter) {
		if (iter->second->obtype == ObjectType::BuiltIn_OBJ){
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
	ExpAST_pool.clear();
	Object_pool.clear();
}


int main(){
	//_CrtSetBreakAlloc(375);
	//_CrtSetBreakAlloc(828);
	init_env();

	auto ExpAST_pool_ptr = &ExpAST_pool;
	auto Object_pool_ptr = &Object_pool;

	std::ifstream file_in("E:\\BaiduDisk\\Code\\Compiler\\PGWT\\CppScheme\\Data\\input.txt");

	//mainloop(Env, file_in);

	mainloop(Env, std::cin);


	file_in.close();

	clear_env();

	cout << "ending construct" << endl;

	
	_CrtDumpMemoryLeaks();
	return 0;
}