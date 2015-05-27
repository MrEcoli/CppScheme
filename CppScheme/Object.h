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

	std::unordered_map<Object*, bool> Object_pool;

	enum ObjectType{
		PROCEDURE = 1,
		DOUBLEOBJ,
		BOOLOBJ,
		PAIROBJ,
		BuiltInOBJ,
	};
	enum ExpAST_TYPE{
		EXIT_TYPE,
		SIMPLE_TYPE,
		VARIABLE_TYPE,
		CALLPROCEDURE_TYPE,
		DEFINEPROCEDURE_TYPE,
		DEFINEVARIABLE_TYPE,
		IFELSETYPE,
		PROCEDURETYPE,
		CONDEXP,
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




	class DoubleValue :public Object{
	public:
		double value;
		DoubleValue() :Object(ObjectType::DOUBLEOBJ){}
		DoubleValue(double _V) :value(_V), Object(ObjectType::DOUBLEOBJ){}

		virtual Object* deep_copy(){ return DoubleValue::factory (value); }
		static DoubleValue* factory(double _v){
			DoubleValue* ret = new DoubleValue(_v);
			Object_pool[ret] = false;
			return ret;
		}
	};


	//数据变量
	//为了简化程序，所有的数值以double类型表示
	class Variable :public Object{
		std::string name;
		Variable(std::string s = std::string("")) :name(s),  Object(DOUBLEOBJ){}
		virtual Object* deep_copy(){ return Variable::factory (name); }
		static Variable* factory(std::string _name){
			Variable *ret = new Variable(_name);
			Object_pool[ret] = false;
			return ret;
		}

	};


	//Represent a Procedure Object
	//Come from define expression or lambda expression
	class Procedure :public Object{
	public:
		std::vector < std::string > args;
		std::vector<ExpAST*> exprs;
		Procedure() :Object(ObjectType::PROCEDURE){}
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
	};
	


	class BoolValue :public Object{
	public:
		bool value;
		BoolValue() :Object(ObjectType::BOOLOBJ){}
		BoolValue(bool _bb) :value(_bb), Object(ObjectType::BOOLOBJ){}
		virtual Object* deep_copy(){ return BoolValue::factory (this->value); }
		static BoolValue* factory(bool _v){
			BoolValue* ret = new BoolValue(_v);
			Object_pool[ret] = false;
			return ret;
		}
	};




	class Pair :public Object{
	public:
		Object *first;
		Object *second;
		Pair() :Object(ObjectType::PAIROBJ), first(nullptr), second(nullptr){}
		Pair(Object* _x, Object* _y) :first(_x), second(_y), Object(ObjectType::PAIROBJ){}
				
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
	};

}


#endif
