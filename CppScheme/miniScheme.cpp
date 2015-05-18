#define DEBUG_TOKEN
#include <iostream>
#include "Parser.h"
#include <fstream>
#include <xtree>
using namespace MiniScheme;
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

void init_env(EnvTree* gloabl){





}


int main(){


	

	GlobalVariable = new EnvTree();
	Env = Env.push_front(GlobalVariable);

	



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
	std::ifstream inputfile;
	#define DEBUG_TOKEN
	inputfile.open("E:\\BaiduDisk\\Code\\Compiler\\PGWT\\CppScheme\\miniScheme\\intput.mscm");
	cur_char = inputfile.get();
	while (cur_char != EOF) {
	get_token(inputfile);
	}

	cin.get();


	cout << "ending construct" << endl;

	return 0;
}