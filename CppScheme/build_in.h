#ifndef _MINISCHEME_BUILD_IN_H
#define _MINISCHEME_BUILD_IN_H
#include <algorithm>
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
				Fout::error_out("Invalid operator in Builitin function\n");
				return 0;
			}
		}
	};


	class BeginExpr :public BuiltIn{
	public:	
		Object* operator()(std::vector<Object*>& args){
			return args.back();
		}
	};

	class Cons :public BuiltIn{
	public:
		Object* operator()(std::vector<Object*>& args){
			if (args.size() != 2){
				Fout::error_out("cons operator error, illegal arguments number, expect to be 2, give", args.size(), "\n");
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
				Fout::error_out("pair? operator error, illegal arguments number, expect more than 1, give", args.size(), "\n");
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
				Fout::error_out("number? operator error, illegal arguments number, expect more than 1, give", args.size(), "\n");
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
				Fout::error_out("integer? operator error, illegal arguments number, expect more than 1, give", args.size(), "\n");
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
				Fout::error_out("eq? operator error, illegal arguments number, expect more than 1, give", args.size(), "\n");
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
				Fout::error_out("length operator error, illegal arguments number, expect more than 1, give", args.size(), "\n");
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
				Fout::error_out("car operator error, illegal arguments number, expect to be 1, give", args.size(), "\n");
				return nullptr;
			}
			//如果传递来的是临时变量，使用car后返回first


			if (args[0]->obtype == ObjectType::PAIR_OBJ)
				return ((Pair*)args[0])->first;
			else{
				Fout::error_out("car illegal operator, not a Pair\n");
				return nullptr;
			}
		}
	};

	class Cdr :public BuiltIn{
	public:
		Object* operator()(std::vector<Object*>& args){
			if (args.size() != 1){
				Fout::error_out("cdr operator error, illegal arguments number, expect to be 1, give", args.size(), "\n");
				return nullptr;
			}

			//如果是临时变量，我们会返回并保存Pair中的第二个Object对象
			//不能直接删除Pair
			//需要删除Pair的第一个变量

			if (args[0]->obtype == ObjectType::PAIR_OBJ)
				return ((Pair*)args[0])->second;
			else{
				Fout::error_out("cdr illegal operator, not a Pair\n");
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
				cur = Pair::factory(args[idx], NullValue::factory());
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
				Fout::error_out("add operator expect two operator, expression give", args.size(), "\n");
				return nullptr;
			}

			if (all_of(args.begin(), args.end(), [](Object* _obj) { return _obj->obtype == ObjectType::INTEGER_OBJ; })){
				int result = 0;
				for (auto _obj : args) {
					result += (((IntegerValue*)(_obj))->value);
				}
				return IntegerValue::factory(result);
			}
			else{
				double result = 0;
				for (auto _obj : args){
					result += (get_value(_obj));
				}
				return DoubleValue::factory(result);
			}

		}
	};

	class Sub :public BuiltIn{
	public:
		Object* operator()(std::vector<Object*>& args){
			if (args.size() != 2){
				Fout::error_out("Sub operator expect two operator, expression give", args.size(), "\n");
				return nullptr;
			}

			if (args[0]->obtype == ObjectType::INTEGER_OBJ && args[1]->obtype == ObjectType::INTEGER_OBJ){
				int v1 = ((IntegerValue*)args[0])->value;
				int v2 = ((IntegerValue*)args[1])->value;
				return IntegerValue::factory(v1 - v2);
			}
			else{
				double v1 = get_value(args[0]);
				double v2 = get_value(args[1]);
				return DoubleValue::factory(v1 - v2);
			}

		}
	};

	class Mul :public BuiltIn{
	public:
		Object* operator()(std::vector<Object*>& args){
			if (args.size() < 2){
				Fout::error_out("Mul operator error, needs more than one operator, expression give", args.size(), "\n");
				return nullptr;
			}

			if (all_of(args.begin(), args.end(), [](Object* _obj) { return _obj->obtype == ObjectType::INTEGER_OBJ; })){
				int result = 1;
				for (auto _obj : args) {
					result *= (((IntegerValue*)(_obj))->value);
				}
				return IntegerValue::factory(result);
			}
			else{
				double result = 1;
				for (auto _obj : args){
					result *= (get_value(_obj));
				}
				return DoubleValue::factory(result);
			}


		}
	};


	class Div :public BuiltIn{
	public:
		Object* operator()(std::vector<Object*>& args){
			if (args.size() != 2){
				Fout::error_out("Div operator error, expected twp operand, expression give", args.size(), "\n");
				return nullptr;
			}

			if (args[0]->obtype == ObjectType::INTEGER_OBJ && args[1]->obtype == ObjectType::INTEGER_OBJ){
				int v1 = ((IntegerValue*)args[0])->value;
				int v2 = ((IntegerValue*)args[1])->value;
				if (v2 == 0){
					Fout::error_out ( "Invalid Div operator, denominator could not be ZERO\n");
					return nullptr;
				}

				return IntegerValue::factory(v1 / v2);
			}
			else{
				double v1 = get_value(args[0]);
				double v2 = get_value(args[1]);
				if (v2 == 0){
					Fout::error_out ("Invalid Div operator, denominator could not be ZERO\n");
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
				Fout::error_out("Remainder operator error, needs more than one operator, expression give", args.size(), "\n");
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
				Fout::error_out("Abs operator error, needs more than one operator, expression give", args.size(), "\n");
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

				Fout::error_out ("Invalid value for abs operator\n");
				return nullptr;
			}
		}
	};




	class Less :public BuiltIn{
	public:
		Object* operator()(std::vector<Object*>& args){
			if (args.size() != 2){
				Fout::error_out("Less operator error, needs more than one operator, expression give", args.size(), "\n");	
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
				Fout::error_out("Less_equal operator error, needs more than one operator, expression give", args.size(), "\n");
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
				Fout::error_out("Greater operator error, needs more than one operator, expression give", args.size(), "\n");
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
				Fout::error_out("Greater_equal operator error, needs more than one operator, expression give", args.size(), "\n");
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
				Fout::error_out("Equal operator error, needs more than one operator, expression give", args.size(), "\n");
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
				Fout::error_out("Not_equal operator error, needs more than one operator, expression give", args.size(), "\n");
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
				Fout::error_out("Set operator error, needs more than one operator, expression give", args.size(), "\n");
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
				Fout::error_out("Or operator error, needs more than one operator, expression give", args.size(), "\n");
				return nullptr;
			}

			bool cur_cond;
			BoolValue* ret;

			bool res = all_of(args.begin(), args.end(),
				[](Object* _obj){ return _obj->obtype == ObjectType::BOOL_OBJ && !((BoolValue*)_obj)->value; });

			if (res)
				return BoolValue::factory(false);
			else
				return BoolValue::factory(true);

		}
	};

	class And :public BuiltIn{
	public:
		Object* operator()(std::vector<Object*>& args){
			if (args.size() < 2){
				Fout::error_out("And operator error, needs more than one operator, expression give", args.size(), "\n");
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
					Fout::error_out("Not valid Object\n");
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
				Fout::error_out("Not operator error, needs more than one operator, expression give", args.size(), "\n");
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
				Fout::error_out("Invalid object\n");
				ret = nullptr;
			}

			return ret;
		}
	};


	class Display :public BuiltIn{
		Object* operator()(std::vector<Object*>& args){
			if (args.size() != 1){
				Fout::error_out("display operator error, needs more than one operator, expression give", args.size(), "\n");
				return nullptr;
			}
			std::cout << args[0]->to_string();
			return nullptr;
		}
	};

	class Random :public BuiltIn{
		Object* operator()(std::vector<Object*>& args){
			if (args.size() != 1 ){
				Fout::error_out("Random operator error, needs more than one operator, expression give", args.size(), "\n");
				return nullptr;
			}
			srand((unsigned int)time(nullptr));

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
				Fout::error_out("Newline operator error, give", args.size(), "parameter\n");
				return nullptr;
			}
			fprintf(stdout, "\n");
			return nullptr;
		}
	};
	

	class IsList :public BuiltIn{
		Object* operator()(std::vector<Object*>& args){
			if (args.size () != 1){
				Fout::error_out("list? error, expect one operand, give", args.size(), "\n");
				return nullptr;
			}

			if (!args[0]){
				Fout::error_out("list? error, Null object is given for list");
				return nullptr;
			}

			Object* cur = args[0];


			while (cur && cur->obtype == ObjectType::PAIR_OBJ) {
				cur = ((Pair*)cur)->second;
			}

			if (cur && cur->obtype == ObjectType::NULL_OBJ){
				return BoolValue::factory(true);
			}
			else{
				return BoolValue::factory(false);
			}
		}
	};


	class IsNull :public BuiltIn{
		Object* operator()(std::vector<Object*>& args){
			if (args.size () != 1){
				Fout::error_out("null? error, expect one operand, given", args.size(), "\n");
				return nullptr;
			}

			Object* cur = args[0];


			if (cur && cur->obtype == NULL_OBJ){
				return BoolValue::factory(true);
			}
			else{
				return BoolValue::factory(false);
			}

		}
	};

	class IsSymnol :public BuiltIn{
		Object* operator()(std::vector<Object*>& args){
			if (args.size() != 1){
				Fout::error_out("symbol? error, expect one operand, given", args.size(), "\n");
				return nullptr;
			}

			Object* cur = args[0];

			if (cur && cur->obtype == ObjectType::SYMBOL_OBJ){
				return BoolValue::factory(true);
			}
			else{
				return BoolValue::factory(false);
			}
		}
	};

	class IsString :public BuiltIn{
		Object* operator()(std::vector<Object*>& args){
			if (args.size() != 1){
				std::cerr << "string? error, expect one operand, given " << args.size() << std::endl;
				return nullptr;
			}

			Object* cur = args[0];

			if (cur && cur->obtype == ObjectType::STRING_OBJ){
				return BoolValue::factory(true);
			}
			else{
				return BoolValue::factory(false);
			}
		}
	};


	class IsVector :public BuiltIn{
		Object* operator()(std::vector<Object*>& args){
			if (args.size() != 1){
				std::cerr << "vector? error, expect one operand, given " << args.size() << std::endl;
				return nullptr;
			}

			Object* cur = args[0];

			if (cur && cur->obtype == ObjectType::VECTOR_OBJ){
				return BoolValue::factory(true);
			}
			else{
				return BoolValue::factory(false);
			}
		}
	};



	class IsProcedure :public BuiltIn{
		Object* operator()(std::vector<Object*>& args){
			if (args.size() != 1){
				std::cerr << "procedure? error, expect one operand, given " << args.size() << std::endl;
				return nullptr;
			}

			Object* cur = args[0];

			if (cur && cur->obtype == ObjectType::PROCEDURE_OBJ){
				return BoolValue::factory(true);
			}
			else{
				return BoolValue::factory(false);
			}
		}
	};


	class IsBoolean :public BuiltIn{
		Object* operator()(std::vector<Object*>& args){
			if (args.size() != 1){
				std::cerr << "boolean? error, expect one operand, given " << args.size() << std::endl;
				return nullptr;
			}

			Object* cur = args[0];

			if (cur && cur->obtype == ObjectType::BOOL_OBJ){
				return BoolValue::factory(true);
			}
			else{
				return BoolValue::factory(false);
			}
		}
	};


	




	//scheme的内置谓词
	//没有任何对象同时满足下面所列的两个或两个以上的谓词
	//boolean?
	//pair?
	//symbol?
	//char?
	//number?
	//string?
	//vector?
	//port?
	//procedure?
}

#endif