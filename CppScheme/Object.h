#ifndef _MINISCHEME_OBJECT_H
#define _MINISCHEME_OBJECT_H

#include <string>
#include <vector>
#include "Debug.h"
#include <unordered_map>
#include <map>
#include "slist.h"

namespace CppScheme{
	class ExpAST;
	class Object;

	std::map<Object*, bool> Object_pool;

	enum ObjectType{
		PROCEDURE_OBJ = 1,
		DOUBLE_OBJ,
		INTEGER_OBJ,
		STRING_OBJ,
		BOOL_OBJ,
		PAIR_OBJ,
		BuiltIn_OBJ,
		NULL_OBJ,
		CHAR_OBJ,
		VECTOR_OBJ,
	};

	enum ExpAST_TYPE{
		EXIT_TYPE,
		SIMPLE_TYPE,
		VARIABLE_TYPE,
		CALLPROCEDURE_TYPE,
		DEFINEPROCEDURE_TYPE,
		DEFINEVARIABLE_TYPE,
		IFELSE_TYPE,
		PROCEDURE_TYPE,
		COND_EXP,
	};

	class Object{
	public:
		ObjectType obtype;
		Object(ObjectType _Ty) :obtype(_Ty){}
		virtual ObjectType get_type(){ return obtype; }
		virtual ~Object(){};
		virtual Object* deep_copy(){
			return this;
		};
		virtual std::string to_string(){
			return std::string();
		}
	};

	class ExpAST{
	public:
		typedef std::map<std::string, Object*> EnvTree;
		typedef slist<EnvTree> EnvTreeList;

		virtual Object* eval(EnvTreeList) = 0;
		virtual ~ExpAST() {
			_DEBUG_DESTRUCTOR("ExpAST base class destructor function");
		};
		ExpAST_TYPE _exp_type;
		ExpAST(ExpAST_TYPE _type = ExpAST_TYPE::SIMPLE_TYPE) :_exp_type(_type){}
		virtual ExpAST* deep_copy(){ return this; }
	};


	class IntegerValue :public Object{
	public:
		int value;
		IntegerValue() :Object(ObjectType::INTEGER_OBJ){}
		IntegerValue(int _v) :value(_v), Object(ObjectType::INTEGER_OBJ){}
		virtual Object* deep_copy(){ return IntegerValue::factory(value); }
		static IntegerValue* factory(int _v){
			IntegerValue* ret = new IntegerValue(_v);
			Object_pool[ret] = false;
			return ret;
		}
		virtual std::string to_string(){
			return std::to_string(static_cast<long long>(value));
		}
	};


	class DoubleValue :public Object{
	public:
		double value;
		DoubleValue() :Object(ObjectType::DOUBLE_OBJ){}
		DoubleValue(double _V) :value(_V), Object(ObjectType::DOUBLE_OBJ){}

		virtual Object* deep_copy(){ return DoubleValue::factory (value); }
		static DoubleValue* factory(double _v){
			DoubleValue* ret = new DoubleValue(_v);
			Object_pool[ret] = false;
			return ret;
		}
		virtual std::string to_string(){
			return std::to_string(value);
		}
	};


	//数据变量
	//为了简化程序，所有的数值以double类型表示
	class Variable :public Object{
		std::string name;
		Variable(std::string s = std::string("")) :name(s),  Object(DOUBLE_OBJ){}
		virtual Object* deep_copy(){ return Variable::factory (name); }
		static Variable* factory(std::string _name){
			Variable *ret = new Variable(_name);
			Object_pool[ret] = false;
			return ret;
		}
		virtual std::string to_string(){
			return name;
		}

	};


	//Represent a Procedure Object
	//Come from define expression or lambda expression
	class Procedure :public Object{
	public:
		std::vector < std::string > args;
		std::vector<ExpAST*> exprs;
		Procedure() :Object(ObjectType::PROCEDURE_OBJ){}
		virtual Object* deep_copy(){
			Procedure* ret = Procedure::factory ();
			ret->args = this->args;
			ret->exprs = this->exprs;

			for (size_t i = 0; i != ret->exprs.size(); ++i) {
				ret->exprs[i] = this->exprs[i]->deep_copy();
			}
			return ret;
		}
		static Procedure* factory(){
			Procedure* ret = new Procedure();
			Object_pool[ret] = false;
			return ret;
		}
		virtual std::string to_string(){
			return std::string("#<procedure>");
		}
	};
	


	class BoolValue :public Object{
	public:
		bool value;
		BoolValue() :Object(ObjectType::BOOL_OBJ){}
		BoolValue(bool _bb) :value(_bb), Object(ObjectType::BOOL_OBJ){}
		virtual Object* deep_copy(){ return BoolValue::factory (this->value); }
		static BoolValue* factory(bool _v){
			BoolValue* ret = new BoolValue(_v);
			Object_pool[ret] = false;
			return ret;
		}

		virtual std::string to_string(){
			if (value){
				return "#t";
			}
			else{
				return "#f";
			}
		}
	};




	class Pair :public Object{
	public:
		Object *first;
		Object *second;
		Pair() :Object(ObjectType::PAIR_OBJ), first(nullptr), second(nullptr){}
		Pair(Object* _x, Object* _y) :first(_x), second(_y), Object(ObjectType::PAIR_OBJ){}
				
		virtual Object* deep_copy(){
			Pair *ret = Pair::factory();
			ret->first = first->deep_copy();
			ret->second = second->deep_copy();
			return ret;
		}

		static Pair* factory(){
			Pair* ret = new Pair();
			Object_pool[ret] = false;
			return ret;
		}
		static Pair* factory(Object* _x, Object* _y){
			Pair* ret = new Pair(_x, _y);
			Object_pool[ret] = false;
			return ret;
		}

		virtual std::string to_string(){
			return "(" + to_string_no_brace() + ")";
		}
		
		std::string to_string_no_brace(){
			std::string s1, s2;
			std::string result;
			if (first){
				s1 = first->to_string();
			}
			if (second){
				if (second->obtype == ObjectType::PAIR_OBJ){
					s2 = " " + ((Pair*)second)->to_string_no_brace();
				}
				else{
					s2 = " . " + second->to_string();
				}
			}
			return result = s1 + s2;
		}

	};

}


#endif
