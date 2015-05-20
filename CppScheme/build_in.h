#ifndef _MINISCHEME_BUILD_IN_H
#define _MINISCHEME_BUILD_IN_H

#include <iostream>
#include <map>
#include "Object.h"
#include "slist.h"
using namespace CppScheme;

typedef std::map<std::string, Object*> EnvTree;
typedef slist<EnvTree> EnvTreeList;

static EnvTreeList Env;
static EnvTree* GlobalVariable;

namespace CppScheme{
	class ExpAST;


	class BuiltIn:public Object{
	public:
		BuiltIn() :Object(ObjectType::PROCEDURE){}
		virtual Object* operator()(std::vector<Object*>& args) = 0;
	};


	class Cons :public BuiltIn{
	public:
		Object* operator()(std::vector<Object*>& args){
			if (args.size() != 2){
				std::cout << "cons operator error, illegal arguments number, expect to be 2, give " << args.size() << std::endl;
				return nullptr;
			}
			return new Pair(args[0], args[1]);
		}
	};

	class Car :public BuiltIn{
	public:
		Object* operator()(std::vector<Object*>& args){
			if (args.size() != 1){
				std::cout << "car operator error, illegal arguments number, expect to be 1, give " << args.size() << std::endl;
				return nullptr;
			}

			if (Pair* ptr = dynamic_cast<Pair*> (args[0])){
				return ptr->first;
			}
			else{
				std::cout << "illegal operator, not a Pair" << std::endl;
				return nullptr;
			}
		}
	};

	class Cdr :public BuiltIn{
	public:
		Object* operator()(std::vector<Object*>& args){
			if (args.size() != 1){
				std::cout << "cdr operator error, illegal arguments number, expect to be 1, give " << args.size() << std::endl;
				return nullptr;
			}

			if (Pair* ptr = dynamic_cast<Pair*> (args[0])){
				return ptr->next;
			}
			else{
				std::cout << "illegal operator, not a Pair" << std::endl;
				return nullptr;
			}
		}
	};

	class List :public BuiltIn{
	public:
		Object* operator()(std::vector<Object*> &args){
			Pair* result = new Pair();
			size_t n = args.size();
			if (n == 0){
				return result;
			}

			result->first = args[0];
			Pair* prev = result;
			Pair* cur;
			for (size_t idx = 1; idx != n; ++idx) {
				cur = new Pair(args[idx], nullptr);
				prev->next = cur;
				cur = prev;
			}
			return result;
		}
	};



	class Add :public BuiltIn{
	public:
		Object* operator()(std::vector<Object*>& args){
			if (args.size() < 2){
				std::cout << "add operator error, needs more than one operator, expression give " << args.size() << std::endl;
				return nullptr;
			}
			size_t n = args.size();
			double ret = ((DoubleValue*)args[0])->value + ((DoubleValue*)args[1])->value;
			for (size_t i = 2; i < n; i++)
			{
				ret += ((DoubleValue*)args[i])->value;
			}
			return new DoubleValue(ret);
		}
	};

	class Sub :public BuiltIn{
	public:
		Object* operator()(std::vector<Object*>& args){
			if (args.size() < 2){
				std::cout << "Sub operator error, needs more than one operator, expression give " << args.size() << std::endl;
				return nullptr;
			}
			size_t n = args.size();
			double ret = ((DoubleValue*)args[0])->value - ((DoubleValue*)args[1])->value;
			for (size_t i = 2; i < n; i++)
			{
				ret -= ((DoubleValue*)args[i])->value;
			}
			return new DoubleValue(ret);
		}
	};

	class Mul :public BuiltIn{
	public:
		Object* operator()(std::vector<Object*>& args){
			if (args.size() < 2){
				std::cout << "Mul operator error, needs more than one operator, expression give " << args.size() << std::endl;
				return nullptr;
			}
			size_t n = args.size();
			double ret = ((DoubleValue*)args[0])->value * ((DoubleValue*)args[1])->value;
			for (size_t i = 2; i < n; i++)
			{
				ret *= ((DoubleValue*)args[i])->value;
			}
			return new DoubleValue(ret);
		}
	};


	class Div :public BuiltIn{
	public:
		Object* operator()(std::vector<Object*>& args){
			if (args.size() < 2){
				std::cout << "Div operator error, needs more than one operator, expression give " << args.size() << std::endl;
				return nullptr;
			}
			size_t n = args.size();
			double ret = ((DoubleValue*)args[0])->value / ((DoubleValue*)args[1])->value;
			for (size_t i = 2; i < n; i++)
			{
				ret /= ((DoubleValue*)args[i])->value;
			}
			return new DoubleValue(ret);
		}
	};

	class Less :public BuiltIn{
	public:
		Object* operator()(std::vector<Object*>& args){
			if (args.size() != 2){
				std::cout << "Less operator error, needs more than one operator, expression give " << args.size() << std::endl;
				return nullptr;
			}

			double v1 = ((DoubleValue*)(args[0]))->value;
			double v2 = ((DoubleValue*)(args[1]))->value;
			
			if (v1 < v2){
				return new BoolValue(true);
			}
			else
				return new BoolValue(false);
			
		}
	};

	class Greater :public BuiltIn{
	public:
		Object* operator()(std::vector<Object*>& args){
			if (args.size() != 2){
				std::cout << "Less operator error, needs more than one operator, expression give " << args.size() << std::endl;
				return nullptr;
			}

			double v1 = ((DoubleValue*)(args[0]))->value;
			double v2 = ((DoubleValue*)(args[1]))->value;

			if (v1 > v2){
				return new BoolValue(true);
			}
			else
				return new BoolValue(false);

		}
	};

	class Equal :public BuiltIn{
	public:
		Object* operator()(std::vector<Object*>& args){
			if (args.size() != 2){
				std::cout << "Equal operator error, needs more than one operator, expression give " << args.size() << std::endl;
				return nullptr;
			}

			Object* result1 = Greater()(args);
			Object* result2 = Less()(args);

			if (((BoolValue*)result1)->value == false && ((BoolValue*)result2)->value == false){
				return new BoolValue(true);
			}
			else{
				return new BoolValue(false);
			}
		}
	};

	class Remainder :public BuiltIn{
	public:
		Object* operator()(std::vector<Object*>& args){
			if (args.size() != 2){
				std::cout << "Remainder operator error, needs more than one operator, expression give " << args.size() << std::endl;
				return nullptr;
			}
			int v1 = ((DoubleValue*)args[0])->value;
			int v2 = ((DoubleValue*)args[1])->value;
			return new DoubleValue(v1 % v2);
		}
	};

}

#endif