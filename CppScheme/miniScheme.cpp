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


int print_eval_result(ExpAST* expr, EnvTreeList env){
	if (expr){
		if (expr->_exp_type == ExpAST_TYPE::EXIT_TYPE){
			delete expr;
			mark_sweep();
			return -1;
		}

		Object* ret = expr->eval(env);

		if (ret) {
			switch (ret->obtype)
			{
			case ObjectType::BOOL_OBJ:
			{
				BoolValue* ptr = (BoolValue*)ret;
				if (ptr->value){
					Fout::base_out("#t", "\n");
				}
				else{
					Fout::base_out("#f", "\n");
				}
				break;
			}
			case ObjectType::BuiltIn_OBJ:
			case ObjectType::CLOSURE_OBJ:
			case ObjectType::PROCEDURE_OBJ:
			{
				if (expr->_exp_type == ExpAST_TYPE::VARIABLE_TYPE){
					VariableExp* var_ptr = (VariableExp*)expr;
					Fout::normal_out("#<Procedure", var_ptr->var_name, ">\n");
				}
				else{
					Fout::normal_out("#<Procedure>\n");
				}
				break;
			}
			case ObjectType::DOUBLE_OBJ:
			{
				DoubleValue* ptr = (DoubleValue*)ret;
				Fout::base_out(ptr->value, "\n");
				break;
			}
			case ObjectType::INTEGER_OBJ:
			{
				//cout << "this Integer" << endl;
				IntegerValue* ptr = (IntegerValue*)ret;
				Fout::base_out(ptr->value, "\n");
				break;
				//cout << "Integer output over" << endl;
			}
			case ObjectType::STRING_OBJ:
			{
				StringValue* ptr = (StringValue*)ret;
				Fout::base_out("\""+ptr->value+"\"", "\n");
				break;
			}
			case ObjectType::PAIR_OBJ:
				Fout::normal_out("'", ret->to_string(), "\n");
				break;
			case ObjectType::NULL_OBJ:
				Fout::normal_out("'()\n");
				break;
			case ObjectType::SYMBOL_OBJ:
			{
				SymbolValue* ptr = (SymbolValue*)ret;
				if (ptr->_symbol == "'"){
					Fout::normal_out("'quote\n");
				}
				else if (ptr->_symbol == "`"){
					Fout::normal_out("`quasiquote\n");
				}
				else if (ptr->_symbol == ","){
					Fout::normal_out(",unquote\n");
				}
				else{
					Fout::normal_out(ptr->_symbol, "\n");
				}
			}
			default:
				break;
			}
		}

	}
	//cout << "Over" << endl;
	return 1;
}

void mainloop(EnvTreeList env, std::istream& input){
	while (true){
		Fout::begin_out("Scheme >> ");
		cur_char = input.get();
		ExpAST* expr = parseExpAst(input, env);
		int ret = print_eval_result(expr, env);
		if (ret < 0 || current_tok == TOKEN::EOF_TOK){
			break;
		}
		mark_sweep();
	}
}


void load_file(EnvTreeList env, std::ifstream &input){
	while (true){
		cur_char = input.get();
		ExpAST* expr = parseExpAst(input, env);
		int ret = print_eval_result(expr, env);
		if (ret < 0 || current_tok == TOKEN::EOF_TOK){
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
	(*env)["display"] = new Display();
	(*env)["random"] = new Random();
	(*env)["newline"] = new Newline();
	(*env)["number?"] = new isNumber();
	(*env)["integer?"] = new isInteger();
	(*env)["eq?"] = new Equal_address();
	(*env)["begin"] = new BeginExpr();
	(*env)["null?"] = new IsNull();
	(*env)["append"] = new Append();

}


void release(EnvTree* env){
	for (auto iter = env->begin(); iter != env->end(); ++iter) {
		delete iter->second;
	}
}

void init_env(){
	//setting the Console name, handle, Bufferinfo(font color)
	Fout::init_Console();
	GlobalVariable = new EnvTree();
	Env = Env.push_front(GlobalVariable);
	//add Builtin function;
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
		if (ptr->obtype != ObjectType::NULL_OBJ){
			delete ptr;
		}
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

	std::ifstream file_in("..\\Data\\input.scm");

	load_file(Env, file_in);
	mainloop(Env, std::cin);


	file_in.close();

	clear_env();

	cout << "ending construct" << endl;

	
	//_CrtDumpMemoryLeaks();
	return 0;
}