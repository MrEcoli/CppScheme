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

	class BuiltIn :public Object{
	public:
		BuiltIn() :Object(ObjectType::BuiltInOBJ){}
		virtual Object* operator()(std::vector<Object*>& args) = 0;
	};


	class Cons :public BuiltIn{
	public:
		Object* operator()(std::vector<Object*>& args){
			if (args.size() != 2){
				std::cout << "cons operator error, illegal arguments number, expect to be 2, give " << args.size() << std::endl;
				return nullptr;
			}
			Pair* ret = new Pair(args[0], args[1]);
			return ret;
		}
	};

	class Car :public BuiltIn{
	public:
		Object* operator()(std::vector<Object*>& args){
			if (args.size() != 1){
				std::cout << "car operator error, illegal arguments number, expect to be 1, give " << args.size() << std::endl;
				return nullptr;
			}
			//如果传递来的是临时变量，使用car后返回first

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

			//如果是临时变量，我们会返回并保存Pair中的第二个Object对象
			//不能直接删除Pair
			//需要删除Pair的第一个变量

			if (Pair* ptr = dynamic_cast<Pair*> (args[0])){
				return ptr->second;
			}
			else{
				std::cout << "illegal operator, not a Pair" << std::endl;
				return nullptr;
			}
		}
	};

	class List :public BuiltIn{
	public:
		Object* operator()(std::vector<Object*>& args){
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
				prev->second = cur;
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
			/*

						std::cout << "Greater operator" << std::endl;
						std::cout << "current left value v1 = " << v1 << std::endl;
						std::cout << "current left value v2 = " << v2 << std::endl;
						*/


			

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


			

			BoolValue * ret;
			if (((BoolValue*)result1)->value == false && ((BoolValue*)result2)->value == false){
				ret = new BoolValue(true);
			}
			else{
				ret = new BoolValue(false);
			}

			delete result1;
			delete result2;

			return ret;
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

	class Set :public BuiltIn{
	public:
		Object* operator()(std::vector<Object*>& args){
			if (args.size() != 2){
				std::cout << "Set! operator error, needs more than one operator, expression give " << args.size() << std::endl;
				return nullptr;
			}
			args[0] = args[1];
			return args[0];
		}
	};

	inline bool is_condition_true(Object* obj, bool& result){
		result = true;
		if (!obj){
			return false;
		}

		if (BoolValue* ptr = dynamic_cast<BoolValue*>(obj)){
			if (!ptr->value){
				result = false;
			}
		}
		else if (DoubleValue* ptr = dynamic_cast<DoubleValue*>(obj)){
			if (ptr->value == 0)
				result = false;
		}
		return true;
	}

	class Or :public BuiltIn{
	public:
		Object* operator()(std::vector<Object*>& args){
			if (args.size() < 2){
				std::cout << "Or operator error, needs more than one operator, expression give " << args.size() << std::endl;
				return nullptr;
			}

			bool cur_cond;
			BoolValue* ret;
			for (size_t idx = 0; idx != args.size(); ++idx) {
				if (is_condition_true(args[idx], cur_cond)){
					if (cur_cond){
						
						ret = new BoolValue(true);
					}

				}
				else{
					std::cerr << "Not valid Object " << std::endl;
					
					ret = nullptr;
				}
			}
			
			return new BoolValue(false);

		}
	};

	class And :public BuiltIn{
	public:
		Object* operator()(std::vector<Object*>& args){
			if (args.size() < 2){
				std::cout << "And operator error, needs more than one operator, expression give " << args.size() << std::endl;
				return nullptr;
			}

			bool cur_cond;
			for (size_t idx = 0; idx != args.size(); ++idx) {
				if (is_condition_true(args[idx], cur_cond)){
					if (!cur_cond){
						
						return new BoolValue(false);
					}

				}
				else{
					std::cerr << "Not valid Object " << std::endl;
					
					return nullptr;
				}
			}
			
			return new BoolValue(true);
		}
	};

	class Not :public BuiltIn{
	public:
		Object* operator()(std::vector<Object*>& args){
			if (args.size() != 1){
				std::cout << "Not operator error, expected only one operator, expression give " << args.size() << std::endl;
				return nullptr;
			}
			bool cur_cond = true;
			BoolValue* ret;
			if (is_condition_true(args[0], cur_cond)){
				if (cur_cond){
					ret = new BoolValue(false);
				}
				else{
					ret = new BoolValue(true);
				}
			}
			else{
				std::cerr << "Invalid Object given" << std::endl;
				ret = nullptr;
			}
			
			return ret;
		}
	};

	class IsEven :public BuiltIn{
		Object* operator()(std::vector<Object*>& args){
			if (args.size() != 1){
				std::cout << "IsEven? operator error, expected only one operator, expression give " << args.size() << std::endl;
				return nullptr;
			}
			BoolValue* ret;
			if (DoubleValue* ptr = dynamic_cast<DoubleValue*>(args[0])){
				int val = ptr->value;
				if (!(val && 1)){
					ret = new BoolValue(true);
				}
				else{
					ret = new BoolValue(false);
				}
			}
			else{
				std::cerr << "Expected Number Object in even? operator!" << std::endl;
				ret = nullptr;
			}
			
			return ret;
		}
	};

	class Square :public BuiltIn{
		Object* operator()(std::vector<Object*>& args){
			if (args.size() != 1){
				std::cout << "Square operator error, expected only one operator, expression give " << args.size() << std::endl;
				return nullptr;
			}

			if (DoubleValue* ptr = dynamic_cast<DoubleValue*>(args[0])){
				double val = ptr->value;
				
				return new DoubleValue(val*val);
			}
			else{
				std::cerr << "Expected Number Object in square operator!" << std::endl;
				
				return nullptr;
			}
		}
	};

	class Display :public BuiltIn{
		Object* operator()(std::vector<Object*>& args){
			if (args.size() != 1){
				std::cout << "Display operator error, expected only one operator, expression give " << args.size() << std::endl;
				return nullptr;
			}

			printf("%lf\n", ((DoubleValue*)args[0])->value);

			return nullptr;
		}
	};

}

#endif