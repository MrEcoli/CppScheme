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

	//All of Object 
	std::map<Object*, bool> Object_pool;

	enum ObjectType{
		PROCEDURE_OBJ = 1,	//Procedure Object
		DOUBLE_OBJ,			//DoubleValue marker
		INTEGER_OBJ,		//IntegerValue marker
		STRING_OBJ,			//StringValue marker
		BOOL_OBJ,			//BoolValue marker
		PAIR_OBJ,			//Pair Object marker
		BuiltIn_OBJ,		//BuiltIn Procedure marker
		NULL_OBJ,			//NullValue marker
		CHAR_OBJ,			//Charcter Object marker
		VECTOR_OBJ,			//Vector Object marker
		SYMBOL_OBJ,			//Symbol Object marker
	};

	enum ExpAST_TYPE{
		EXIT_TYPE,		//"exit" or "EXIT"
		NULL_TYPE,		// represent ExpAST result '()
		SIMPLE_TYPE,	// represent simple value, integer, double
		VARIABLE_TYPE,	// variable, just a name of a variable
		CALLPROCEDURE_TYPE,	//surrond with brace, function call
		DEFINEPROCEDURE_TYPE,	//define procedrue (define (f arg ...) expr...)
		DEFINEVARIABLE_TYPE,	//define variable (define var_name expr)
		IFELSE_TYPE,		//represent ifelse expression (if condexp thenexp elseexp)
		PROCEDURE_TYPE,		//represent the procedur, as lambda expression
		COND_EXP,			//represent the expression start with cond syntax
		STR_EXP,			//represent the constant string
		SYMBOL_EXP,			//represent the symbol
		PAIR_EXP,			//represent the pair expression '(1 . 2)
		VECTOR_EXP,			//represent the exp eval to vecotrvalue
		LIST_EXP,			//represent the list expression, eval the expression result a list
		
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

	class StringValue : public Object{
	public:
		std::string value;
		StringValue() :Object(ObjectType::STRING_OBJ){}
		StringValue(std::string _x) : value(_x), Object(ObjectType::STRING_OBJ){}
		virtual Object* deep_copy(){ return StringValue::factory(value); }
		static StringValue* factory(std::string _x){
			StringValue* ret = new StringValue(_x);
			Object_pool[ret] = false;
			return ret;
		}

		virtual std::string to_string(){
			return value;
		}
	};

	//NullValue Singleton
	class NullValue : public Object{
	private:
		NullValue() :Object(ObjectType::NULL_OBJ){}
		NullValue(const NullValue& others);
		NullValue& operator=(const NullValue&);
	public:
		virtual Object* deep_copy(){ return this; }
		inline static NullValue* factory(){
			static NullValue Singleton;
			return &Singleton;
		}
		virtual std::string to_string(){
			return std::string();
		}
	};


	class CharValue :public Object{
		char value;
		CharValue() :Object(ObjectType::CHAR_OBJ){}
		CharValue(char _x) :value(_x), Object(ObjectType::CHAR_OBJ){}
		virtual Object* deep_copy(){ return CharValue::factory(value); }

		static CharValue* factory(char _x){
			CharValue* ret = new CharValue(_x);
			Object_pool[ret] = false;
			return ret;
		}

		virtual std::string to_string(){
			return std::string(1, value);
		}
	};

	class VectorValue :public Object{
	public:
		/*Object* (*vec);
		int arr_size;
		VectorValue() :vec(nullptr), arr_size(0), Object(ObjectType::VECTOR_OBJ){}
		VectorValue(int n) :vec((Object**)malloc((sizeof(Object*)) * n)), arr_size(n), Object(ObjectType::VECTOR_OBJ){}
		VectorValue(int n, Object* _obj) :vec((Object**)malloc((sizeof(Object*)) * n)), arr_size(n), Object(ObjectType::VECTOR_OBJ){
		for (int i = 0; i != n; ++i) {
		vec[i] = _obj->deep_copy();
		}
		}*/

		std::vector<Object*> vec;
		VectorValue() :Object(ObjectType::VECTOR_OBJ){}
		VectorValue(int n) :vec(n), Object(ObjectType::VECTOR_OBJ){}
		VectorValue(int n, Object* _obj) :vec(n, _obj), Object(ObjectType::VECTOR_OBJ){}

		static VectorValue* factory(int n){
			VectorValue* ret = new VectorValue(n);
			Object_pool[ret] = false;
			return ret;
		}

		static VectorValue* factory(int n, Object* _obj){
			VectorValue* ret = new VectorValue(n, _obj);
			Object_pool[ret] = false;
			return ret;
		}

		Object* deep_copy(){
			VectorValue* ret = VectorValue::factory(vec.size());
			for (size_t i = 0; i != vec.size(); ++i) {
				ret->vec[i] = this->vec[i]->deep_copy();
			}
			return ret;
		}

		std::string to_string(){
			std::string result;
			for (size_t i = 0; i != vec.size(); ++i) {
				result += vec[i]->to_string();
				if (i < vec.size() - 1){
					result += " ";
				}
			}
			return "#(" + result + ")";
		}
	};

	class SymbolValue :public Object{
	public:
		std::string _symbol;
		SymbolValue():Object(ObjectType::SYMBOL_OBJ){}
		SymbolValue(std::string _s) :Object(ObjectType::SYMBOL_OBJ), _symbol(_s){}
		static SymbolValue* factory(std::string _s){
			SymbolValue* ret = new SymbolValue(_s);
			Object_pool[ret] = false;
			return ret;
		}

		std::string to_string(){
			return _symbol;
		}
		Object* deep_copy(){
			return SymbolValue::factory(_symbol);
		}
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

		virtual Object* deep_copy(){ return DoubleValue::factory(value); }
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
		Variable(std::string s = std::string("")) :name(s), Object(DOUBLE_OBJ){}
		virtual Object* deep_copy(){ return Variable::factory(name); }
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
			Procedure* ret = Procedure::factory();
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
		virtual Object* deep_copy(){ return BoolValue::factory(this->value); }
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
		Pair() :Object(ObjectType::PAIR_OBJ), first(NullValue::factory()), second(NullValue::factory()){}
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
			if (first->obtype != ObjectType::NULL_OBJ){
				s1 = first->to_string();
			}
			if (second->obtype != ObjectType::NULL_OBJ){
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
