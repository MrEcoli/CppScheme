#ifndef _MINISCHEME_BUILD_IN_H
#define _MINISCHEME_BUILD_IN_H

#include <iostream>
#include <map>
#include "Object.h"
#include "slist.h"
#include <random>
#include <time.h>
using namespace CppScheme;

typedef std::map<std::string, Object*> EnvTree;
typedef slist<EnvTree> EnvTreeList;

static EnvTreeList Env;
static EnvTree* GlobalVariable;

namespace CppScheme{


	class ExpAST;

	class BuiltIn :public Object{
	public:
		BuiltIn() :Object(ObjectType::BuiltIn_OBJ){}
		virtual Object* operator()(std::vector<Object*>& args) = 0;

		static double get_value(Object* _obj){
			switch (_obj->obtype)
			{
			case ObjectType::INTEGER_OBJ:
				return ((IntegerValue*)_obj)->value;
			case ObjectType::DOUBLE_OBJ:
				return ((DoubleValue*)_obj)->value;
			case ObjectType::BOOL_OBJ:
			{
				BoolValue* ptr = (BoolValue*)_obj;
				if (ptr->value){
					return 1;
				}
				else{
					return 0;
				}

			}
			default:
				std::cerr << "Invalid operator in Builitin function" << std::endl;
				return 0;
			}
		}
	};


	class Cons :public BuiltIn{
	public:
		Object* operator()(std::vector<Object*>& args){
			if (args.size() != 2){
				std::cout << "cons operator error, illegal arguments number, expect to be 2, give " << args.size() << std::endl;
				return nullptr;
			}
			Pair* ret = Pair::factory(args[0], args[1]);
			return ret;
		}
	};


	class IsPair :public BuiltIn{
	public:
		Object* operator()(std::vector<Object*>& args){
			if (args.size () < 1){
				std::cerr << "pair? operator error, illegal arguments number, expect more than 1, give " << args.size() << std::endl;
				return nullptr;
			}

			for (auto _obj : args) {
				if (_obj->obtype != ObjectType::PAIR_OBJ){
					return BoolValue::factory(false);
				}
			}
			return BoolValue::factory(true);
		}
	};

	class isNumber : public BuiltIn{
		Object* operator()(std::vector<Object*>& args){
			if (args.size() < 1){
				std::cerr << "number? operator error, illegal arguments number, expect more than 1, give " << args.size() << std::endl;
				return nullptr;
			}

			for (auto _obj : args) {
				if (_obj->obtype != ObjectType::DOUBLE_OBJ && _obj->obtype != ObjectType::INTEGER_OBJ){
					return BoolValue::factory(false);
				}
			}
			return BoolValue::factory(true);
		}
	};

	class isInteger : public BuiltIn{
		Object* operator()(std::vector<Object*>& args){
			if (args.size() < 1){
				std::cerr << "integer? operator error, illegal arguments number, expect more than 1, give " << args.size() << std::endl;
				return nullptr;
			}

			for (auto _obj : args) {
				if ( _obj->obtype != ObjectType::INTEGER_OBJ){
					return BoolValue::factory(false);
				}
			}
			return BoolValue::factory(true);
		}
	};
	
	class Equal_address :public BuiltIn{
		Object* operator()(std::vector<Object*>& args){
			if (args.size() != 2){
				std::cerr << "eq? operator error, illegal arguments number, expect more than 1, give " << args.size() << std::endl;
				return nullptr;
			}

			if (args[0] == args[1]){
				return BoolValue::factory(true);
			}
			else{
				return BoolValue::factory(false);
			}
		}
	};

	class Length :public BuiltIn{
		Object* operator()(std::vector<Object*>& args){
			if (args.size() != 1){
				std::cerr << "length operator error, illegal arguments number, expect more than 1, give " << args.size() << std::endl;
				return nullptr;
			}

			int n = 0;

			Object *cur = args[0];

			while (cur && cur->obtype == ObjectType::PAIR_OBJ) {
				++n;
				cur = ((Pair*)cur)->second;
			}
			return IntegerValue::factory(n);
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

			Pair* result = Pair::factory ();
			size_t n = args.size();

			if (n == 0){
				return result;
			}


			result->first = args[0];
			Pair* prev = result;
			Pair* cur;

			for (size_t idx = 1; idx != n; ++idx) {
				cur = Pair::factory(args[idx], nullptr);
				prev->second = cur;
				prev = cur;
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

			double result = 0;

			bool find_double = false;

			for (size_t i = 0; i < n; i++)
			{
				if (args[i]->obtype == ObjectType::DOUBLE_OBJ){
					find_double = true;
				}
				result += get_value(args[i]);
			}

			if (find_double = false){
				return IntegerValue::factory(result);
			}
			else{
				return DoubleValue::factory(result);
			}
		}
	};

	class Sub :public BuiltIn{
	public:
		Object* operator()(std::vector<Object*>& args){
			if (args.size() < 2){
				std::cout << "Sub operator error, needs more than one operator, expression give " << args.size() << std::endl;
				return nullptr;
			}

			double result = get_value(args[0]);

			bool find_double = args[0]->obtype == ObjectType::INTEGER_OBJ ? false : true;
			size_t n = args.size();

			for (size_t i = 1; i < n; i++)
			{
				if (args[i]->obtype == ObjectType::DOUBLE_OBJ){
					find_double = true;
				}
				result -= get_value(args[i]);
			}

			if (find_double = false){
				return IntegerValue::factory(result);
			}
			else{
				return DoubleValue::factory(result);
			}
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

			double result = get_value(args[0]);

			bool find_double = args[0]->obtype == ObjectType::INTEGER_OBJ ? false : true;

			for (size_t i = 1; i < n; i++)
			{
				if (args[i]->obtype == ObjectType::DOUBLE_OBJ){
					find_double = true;
				}
				result *= get_value(args[i]);
			}

			if (find_double = false){
				return IntegerValue::factory(result);
			}
			else{
				return DoubleValue::factory(result);
			}
		}
	};


	class Div :public BuiltIn{
	public:
		Object* operator()(std::vector<Object*>& args){
			if (args.size() != 2){
				std::cout << "Div operator error, needs more than one operator, expression give " << args.size() << std::endl;
				return nullptr;
			}

			if (args[0]->obtype == ObjectType::INTEGER_OBJ && args[1]->obtype == ObjectType::INTEGER_OBJ){
				int v1 = get_value(args[0]);
				int v2 = get_value(args[1]);
				if (v2 == 0){
					std::cerr << "Invalid Div operator, denominator could not be ZERO" << std::endl;
					return nullptr;
				}

				return IntegerValue::factory(v1 / v2);
			}
			else{
				double v1 = get_value(args[0]);
				double v2 = get_value(args[1]);
				if (v2 == 0){
					std::cerr << "Invalid Div operator, denominator could not be ZERO" << std::endl;
					return nullptr;
				}
				return DoubleValue::factory(v1 / v2);
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

			int v1 = ((IntegerValue*)args[0])->value;
			int v2 = ((IntegerValue*)args[1])->value;

			return IntegerValue::factory(v1%v2);
		}
	};


	class Abs :public BuiltIn{
	public:
		Object* operator()(std::vector<Object*>& args){
			if (args.size() != 1){
				std::cout << "Abs operator error, needs more than one operator, expression give " << args.size() << std::endl;
				return nullptr;
			}

			if (args[0]->obtype == ObjectType::INTEGER_OBJ){
				int _x = ((IntegerValue*)args[0])->value;
				if (_x < 0){
					return IntegerValue::factory(-_x);
				}
				else{
					return IntegerValue::factory(_x);
				}
			}else if (args[0]->obtype == ObjectType::DOUBLE_OBJ){
				double _x = ((DoubleValue*)args[0])->value;
				if (_x < 0){
					return DoubleValue::factory(-_x);
				}
				else{
					return DoubleValue::factory(_x);
				}
			}
			else{
				std::cerr << "Invalid value for abs operator" << std::endl;
				return nullptr;
			}
		}
	};




	class Less :public BuiltIn{
	public:
		Object* operator()(std::vector<Object*>& args){
			if (args.size() != 2){
				std::cout << "Less operator error, needs more than one operator, expression give " << args.size() << std::endl;
				return nullptr;
			}

			double v1 = get_value(args[0]);
			double v2 = get_value(args[1]);

			if (v1 < v2){
				return BoolValue::factory(true);
			}
			else
				return BoolValue::factory(false);

		}
	};

	class Less_equal :public BuiltIn{
		Object* operator()(std::vector<Object*>& args){
			if (args.size() != 2){
				std::cout << "Less_equal operator error, needs more than one operator, expression give " << args.size() << std::endl;
				return nullptr;
			}

			double v1 = get_value(args[0]);
			double v2 = get_value(args[1]);

			if (v1 <= v2){
				return BoolValue::factory(true);
			}
			else
				return BoolValue::factory(false);

		}
	};


	class Greater :public BuiltIn{
	public:
		Object* operator()(std::vector<Object*>& args){
			if (args.size() != 2){
				std::cout << "Less operator error, needs more than one operator, expression give " << args.size() << std::endl;
				return nullptr;
			}

			double v1 = get_value(args[0]);
			double v2 = get_value(args[1]);

			if (v1 > v2){
				return BoolValue::factory(true);
			}
			else
				return BoolValue::factory(false);

		}
	};

	class Greater_equal :public BuiltIn{
	public:
		Object* operator()(std::vector<Object*>& args){
			if (args.size() != 2){
				std::cout << "Less operator error, needs more than one operator, expression give " << args.size() << std::endl;
				return nullptr;
			}

			double v1 = get_value(args[0]);
			double v2 = get_value(args[1]);

			if (v1 >= v2){
				return BoolValue::factory(true);
			}
			else
				return BoolValue::factory(false);

		}
	};



	class Equal :public BuiltIn{
	public:
		Object* operator()(std::vector<Object*>& args){
			if (args.size() != 2){
				std::cout << "Equal operator error, needs more than one operator, expression give " << args.size() << std::endl;
				return nullptr;
			}

			double v1 = get_value(args[0]);
			double v2 = get_value(args[1]);

			if (v1 == v2){
				return BoolValue::factory(true);
			}
			else{
				return BoolValue::factory(false);
			}

		}
	};

	class Not_Equal :public BuiltIn{
	public:
		Object* operator()(std::vector<Object*>& args){
			if (args.size() != 2){
				std::cout << "Not_Equal operator error, needs more than one operator, expression give " << args.size() << std::endl;
				return nullptr;
			}

			double v1 = get_value(args[0]);
			double v2 = get_value(args[1]);

			if (v1 != v2){
				return BoolValue::factory(true);
			}
			else{
				return BoolValue::factory(false);
			}

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

		if (!obj){ return false; }

		switch (obj->obtype)
		{
		case ObjectType::INTEGER_OBJ:
		{
			IntegerValue* ptr = (IntegerValue*)obj;
			if (ptr->value == 0){
				result = false;
			}
			break;
		}
		case ObjectType::DOUBLE_OBJ:
		{
			DoubleValue* ptr = (DoubleValue*)obj;
			if (ptr->value == 0){
				result = false;
			}
			break;
		}
		case ObjectType::BOOL_OBJ:
		{
			BoolValue* ptr = (BoolValue*)obj;
			if (!ptr->value){
				result = false;
			}
			break;
		}
		default:
			break;
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
						ret = BoolValue::factory(true);
					}

				}
				else{
					std::cerr << "Not valid Object " << std::endl;
					ret = nullptr;
				}
			}

			return BoolValue::factory(false);

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

						return BoolValue::factory(false);
					}

				}
				else{
					std::cerr << "Not valid Object " << std::endl;

					return nullptr;
				}
			}
			return BoolValue::factory(true);
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
					ret = BoolValue::factory(false);
				}
				else{
					ret = BoolValue::factory(true);
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

			switch (args[0]->obtype) {
			case ObjectType::INTEGER_OBJ:
			{
				int _x = ((IntegerValue*)args[0])->value;
				if (_x & 1){
					return BoolValue::factory(false);
				}
				else
					return BoolValue::factory(true);
				break;
			}
			default:
				std::cerr << "Invalid input fucntion \"IsEven\"" << std::endl;
				return nullptr;
			}
		}
	};

	class IsOdd :public BuiltIn{
		Object* operator()(std::vector<Object*>& args){
			if (args.size() != 1){
				std::cout << "IsOdd? operator error, expected only one operator, expression give " << args.size() << std::endl;
				return nullptr;
			}

			switch (args[0]->obtype) {
			case ObjectType::INTEGER_OBJ:
			{
				int _x = ((IntegerValue*)args[0])->value;
				if (_x && 1){
					return BoolValue::factory(true);
				}
				else
					return BoolValue::factory(false);
				break;
			}
			default:
				std::cerr << "Invalid input fir \"IsEven\"" << std::endl;
				return nullptr;
			}
		}
	};


	class Square :public BuiltIn{
		Object* operator()(std::vector<Object*>& args){
			if (args.size() != 1){
				std::cout << "Square operator error, expected only one operator, expression give " << args.size() << std::endl;
				return nullptr;
			}

			if (args[0]->obtype == ObjectType::INTEGER_OBJ){
				int _x = ((IntegerValue*)args[0])->value;
				return IntegerValue::factory(_x*_x);
			}
			else if (args[0]->obtype == ObjectType::DOUBLE_OBJ){
				double _x = ((DoubleValue*)args[0])->value;
				return DoubleValue::factory(_x *_x);
			}
			else{
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
			std::cout << args[0]->to_string();
			return nullptr;
		}
	};

	class Random :public BuiltIn{
		Object* operator()(std::vector<Object*>& args){
			if (args.size() != 1 ){
				std::cout << "Display operator error, expected only one operator, expression give " << args.size() << std::endl;
				return nullptr;
			}
			srand(time(nullptr));

			if (args[0]->obtype == ObjectType::INTEGER_OBJ){
				int _x = ((IntegerValue*)args[0])->value;
				return IntegerValue::factory(rand() % _x);
			}
			else if (args[0]->obtype == ObjectType::DOUBLE_OBJ){
				double _x = ((DoubleValue*)args[0])->value;
				return DoubleValue::factory((_x * rand()) / RAND_MAX);
			}
			else{
				return nullptr;
			}
		}
	};

	class Newline : public BuiltIn{
		Object* operator()(std::vector<Object*>& args){
			if (args.size () > 0){
				std::cerr << "Newline operator error, expected no argument" << std::endl;
				return nullptr;
			}
			fprintf(stdout, "\n");
			fflush(stdout);
			return nullptr;
		}
	};
}

#endif