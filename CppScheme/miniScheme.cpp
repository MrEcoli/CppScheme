#define DEBUG_TOKEN
#include <iostream>
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


void mainloop(EnvTreeList env){
	while (1){
		cur_char = std::cin.get();
		ExpAST* expr = parseExpAst(std::cin, env);

		if (!expr){

		}
		else if (Object* ret = expr->eval(env)){
			if (DoubleValue* ptr = dynamic_cast<DoubleValue*>(ret)){
				cout << ptr->value << endl;
			}else if (Procedure* ptr = dynamic_cast<Procedure*>(ret)){
				cout << "Procedure" << endl;
			}
			else if (BoolValue* ptr = dynamic_cast<BoolValue*>(ret)){
				if (ptr->value){
					cout << "#t" << endl;
				}
				else{
					cout << "#f" << endl;
				}
			}
		}
	}
}


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
}


int main(){


	

	GlobalVariable = new EnvTree();
	init(GlobalVariable);
	Env = Env.push_front(GlobalVariable);

	mainloop(Env);


//Variable List Test
/*
	(*GlobalVariable)["Globalv1"] = nullptr;
	(*GlobalVariable)["Globalv2"] = nullptr;
	printCurrentEnv(Env);

	if (1){
		EnvTree* tmp = new EnvTree();
		(*tmp)["localv1"] = nullptr;
		(*tmp)["localv2"] = nullptr;
		(*tmp)["localv3"] = nullptr;
		auto LocalEnv1 = Env.push_front(tmp);
		printCurrentEnv(LocalEnv1);
		if (1){
			EnvTree* tmp2 = new EnvTree();
			(*tmp2)["localv4"] = nullptr;
			(*tmp2)["localv5"] = nullptr;
			(*tmp2)["localv6"] = nullptr;
			auto LocalEnv2 = LocalEnv1.push_front(tmp2);
			printCurrentEnv(LocalEnv2);
			LocalEnv2.clearLocal();
		}
		printCurrentEnv(LocalEnv1);

	}
*/


//get_token function test
/*
	std::ifstream inputfile;
	#define DEBUG_TOKEN
	inputfile.open("E:\\BaiduDisk\\Code\\Compiler\\PGWT\\CppScheme\\miniScheme\\intput.mscm");
	cur_char = inputfile.get();
	while (cur_char != EOF) {
	get_token(inputfile);
	}*/

	cin.get();


	cout << "ending construct" << endl;

	return 0;
}