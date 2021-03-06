#ifndef _MINISCHEME_EXPAST_H
#define _MINISCHEME_EXPAST_H
#include "Debug.h"

#include <string>
#include <vector>
#include <map>
#include "Object.h"
#include "slist.h"
#include <unordered_map>
#include "build_in.h"
using namespace CppScheme;

namespace CppScheme{
	class BuiltIn;
	std::map<ExpAST*, bool> ExpAST_pool;

	class ExitExp : public ExpAST{
	public:
		Object* eval(EnvTreeList){ return nullptr; }
		ExitExp() :ExpAST(ExpAST_TYPE::EXIT_TYPE){}
		virtual ~ExitExp(){
			_DEBUG_DESTRUCTOR("ExitExp destroy");
		}
		static ExitExp* factory(){
			ExitExp* ret = new ExitExp();
			ExpAST_pool[ret] = false;
			return ret;
		}
	};

	class NullExp :public ExpAST{
	private:
		NullExp() :ExpAST(ExpAST_TYPE::NULL_TYPE){}
		NullExp(const NullExp&);
		NullExp operator=(const NullExp&);
	public:
		static NullExp* factory(){
			static NullExp Singleton;
			return &Singleton;
		}

		ExpAST* deep_copy(){ return this; }

		virtual Object* eval(EnvTreeList){ return NullValue::factory(); }
	};


	//Simple ExpAST
	//represent the DoubleValue;
	class SimpleExp :public ExpAST{
	public:
		Object* obj;
		SimpleExp(Object* _obj) :obj(_obj), ExpAST(ExpAST_TYPE::SIMPLE_TYPE){}
		SimpleExp() :obj(nullptr), ExpAST(ExpAST_TYPE::SIMPLE_TYPE){}
		Object* eval(EnvTreeList){ return obj; }
		~SimpleExp(){
			_DEBUG_DESTRUCTOR("SimpleExp destructor function");
		}
		virtual ExpAST* deep_copy(){
			SimpleExp* ret = SimpleExp::factory();
			ret->obj = this->obj->deep_copy();
			return ret;
		}

		inline static SimpleExp* factory(){
			SimpleExp* ret = new SimpleExp();
			ExpAST_pool[ret] = false;
			return ret;
		}

		inline static SimpleExp* factory(Object* _x){
			SimpleExp* ret = new SimpleExp();
			ret->obj = _x;
			ExpAST_pool[ret] = false;
			return ret;
		}


		inline static SimpleExp* factory(double _db){
			DoubleValue* _dv = DoubleValue::factory(_db);
			SimpleExp* ret = new SimpleExp(_dv);
			ExpAST_pool[ret] = false;
			return ret;
		}

		inline static SimpleExp* factory(int _x){
			IntegerValue* _iv = IntegerValue::factory(_x);
			SimpleExp* ret = new SimpleExp(_iv);
			ExpAST_pool[ret] = false;
			return ret;
		}
	};

	class PairExp : public ExpAST{
	public:
		ExpAST* first;
		ExpAST* second;
		PairExp() :ExpAST(ExpAST_TYPE::PAIR_EXP), first(NullExp::factory ()), second(NullExp::factory ()){}
		Object* eval(EnvTreeList env){
			Pair* ret = Pair::factory();
			ret->first = first->eval(env);
			ret->second = second->eval(env);
			return ret;
		}
		ExpAST* deep_copy(){
			PairExp* ret = new PairExp();
			ret->first = first->deep_copy();
			ret->second = second->deep_copy();
			return ret;
		}
		
		inline static PairExp* factory(){
			PairExp *ret = new PairExp();
			ExpAST_pool[ret] = false;
			return ret;
		}

	};

	class VectorExp :public ExpAST{
	public:
		std::vector<ExpAST*> vecs;
		VectorExp() :ExpAST(ExpAST_TYPE::VECTOR_EXP){}

		virtual Object* eval(EnvTreeList env);
		inline static VectorExp* factory(){
			VectorExp* ret = new VectorExp();
			ExpAST_pool[ret] = false;
			return ret;
		}

		ExpAST* deep_copy(){
			VectorExp* ret = new VectorExp();
			ret->vecs.resize(vecs.size());

			for (size_t i = 0; i < vecs.size(); ++i) {
				ret->vecs[i] = this->vecs[i]->deep_copy();
			}
			return ret;
		}
		
	};


	class SymbolExp : public ExpAST{
	public:
		std::string str;
		SymbolExp() :ExpAST(ExpAST_TYPE::SYMBOL_EXP){}
		SymbolExp(std::string _s) : ExpAST(ExpAST_TYPE::SYMBOL_EXP), str(_s){}
		inline static SymbolExp* factory(std::string _x){
			SymbolExp* ret = new SymbolExp(_x);
			ExpAST_pool[ret] = false;
			return ret;
		}
		Object* eval(EnvTreeList){
			return SymbolValue::factory(str);
		}
		ExpAST* deep_copy(){
			return SymbolExp::factory(str);
		}
	};


	class StrExp :public ExpAST{
	public:
		std::string str;
		StrExp(std::string _s) : ExpAST(ExpAST_TYPE::STR_EXP), str(_s){}

		ExpAST* deep_copy(){
			return StrExp::factory(str);
		}
		static StrExp* factory(std::string _s){
			StrExp* ret = new StrExp(_s);
			ExpAST_pool[ret] = false;
			return ret;
		}
		Object* eval(EnvTreeList){
			return StringValue::factory(str);
		}
	};



	//一个表示仅有变量名称的表达式
	//它的codgen将会在给予它的Envtreelist中查找它的Object
	class VariableExp :public ExpAST{
	public:
		std::string var_name;
		VariableExp(std::string _name) :var_name(_name), ExpAST(ExpAST_TYPE::VARIABLE_TYPE){}
		VariableExp() :ExpAST(ExpAST_TYPE::VARIABLE_TYPE){}
		Object* eval(EnvTreeList);
		~VariableExp(){}
		virtual VariableExp* deep_copy(){
			VariableExp* ret = VariableExp::factory(var_name);
			return ret;
		}
		static VariableExp* factory(std::string _s){
			VariableExp* ret = new VariableExp(_s);
			ExpAST_pool[ret] = false;
			return ret;
		}
	};


	//调用过程的表达式
	//如果有名字，则以name进行调用，这个procedure应该是在此处可以访问
	//如果没有名字，那是一个匿名函数，通过Procedure进行调用
	class CallProcedureExp :public ExpAST{
	public:
		ExpAST* func;
		CallProcedureExp() :ExpAST(ExpAST_TYPE::CALLPROCEDURE_TYPE){}
		std::vector<ExpAST*> parameters;
		Object* eval(EnvTreeList);
		virtual CallProcedureExp* deep_copy(){
			CallProcedureExp* ret = CallProcedureExp::factory();
			ret->func = func->deep_copy();
			ret->parameters = this->parameters;

			for (size_t i = 0; i != parameters.size(); ++i) {
				ret->parameters[i] = this->parameters[i]->deep_copy();
			}
			return ret;
		}

		static CallProcedureExp* factory(){
			CallProcedureExp *ret = new CallProcedureExp();
			ExpAST_pool[ret] = false;
			return ret;
		}
	};



	//define variable syntax
	//		(define   variable_name   one_expression )
	class DefineVariableExp :public ExpAST{
	public:
		std::string _name;
		ExpAST* _expr;
		DefineVariableExp() :ExpAST(ExpAST_TYPE::DEFINEVARIABLE_TYPE){}
		Object* eval(EnvTreeList);

		virtual ExpAST* deep_copy(){
			DefineVariableExp* ret = DefineVariableExp::factory();
			ret->_name = _name;
			ret->_expr = this->_expr->deep_copy();
			return ret;
		}

		static DefineVariableExp* factory(){
			DefineVariableExp* ret = new DefineVariableExp();
			ExpAST_pool[ret] = false;
			return ret;
		}
	};

	//define procedure expression
	// define a variable or function is different;
	//		(define   (func args...) (expr1) (expr2) (expr3) .... (expr n) )
	class DefineProcedureExp :public ExpAST{
	public:
		std::string _name;
		std::vector<std::string> _args;
		DefineProcedureExp() :ExpAST(ExpAST_TYPE::DEFINEPROCEDURE_TYPE){}
		std::vector<ExpAST*> exprs;
		Object* eval(EnvTreeList);
		virtual ExpAST* deep_copy(){
			DefineProcedureExp *ret = DefineProcedureExp::factory();
			ret->_name = _name;
			ret->_args = _args;
			ret->exprs = ret->exprs;
			for (size_t i = 0; i != exprs.size(); ++i) {
				ret->exprs[i] = this->exprs[i]->deep_copy();
			}
			return ret;
		}

		static DefineProcedureExp* factory(){
			DefineProcedureExp* ret = new DefineProcedureExp();
			ExpAST_pool[ret] = false;
			return ret;
		}
	};


	//IF语句生成的ExpAST;
	class IfelseExp :public ExpAST{
	public:
		ExpAST* ifexp, *thenexp, *elseexp;
		IfelseExp() :ExpAST(ExpAST_TYPE::IFELSE_TYPE){}
		Object* eval(EnvTreeList);

		virtual ExpAST* deep_copy(){
			IfelseExp *ret = IfelseExp::factory();
			ret->ifexp = ifexp->deep_copy();
			ret->thenexp = thenexp->deep_copy();
			ret->elseexp = elseexp->deep_copy();
			return ret;
		}

		static IfelseExp* factory(){
			IfelseExp* ret = new IfelseExp();
			ExpAST_pool[ret] = false;
			return ret;
		}
	};


	//represent the lamda expression
	//eval生成一个Procedure Object
	class ProcedureExp :public ExpAST{
	public:
		std::vector<std::string> args;
		std::vector<ExpAST*> exprs;
		ProcedureExp() :ExpAST(ExpAST_TYPE::PROCEDURE_TYPE){}
		Object* eval(EnvTreeList);

		virtual ExpAST* deep_copy(){
			ProcedureExp* ret = ProcedureExp::factory();
			ret->args = this->args;
			ret->exprs = this->exprs;
			for (size_t i = 0; i != exprs.size(); ++i) {
				ret->exprs[i] = this->exprs[i]->deep_copy();
			}
			return ret;
		}

		static ProcedureExp* factory(){
			ProcedureExp* ret = new ProcedureExp();
			ExpAST_pool[ret] = false;
			return ret;
		}
	};

	//表示cond起始的条件语句
	class CondExp :public ExpAST{
	public:
		std::vector<ExpAST*> conds;
		std::vector<std::vector<ExpAST*>> rets;
		CondExp() :ExpAST(ExpAST_TYPE::COND_EXP){}
		Object* eval(EnvTreeList);
		virtual ExpAST* deep_copy(){
			CondExp* ret = CondExp::factory();
			ret->conds = ret->conds;
			ret->rets = this->rets;

			for (size_t i = 0; i != conds.size(); ++i) {
				if (this->conds[i]){
					ret->conds[i] = this->conds[i]->deep_copy();
				}
			}

			for (size_t i = 0; i != rets.size(); ++i) {
				for (size_t j = 0; j != rets[i].size(); ++j) {
					ret->rets[i][j] = this->rets[i][j]->deep_copy();
				}
			}
			return ret;
		}

		static CondExp* factory(){
			CondExp* ret = new CondExp();
			ExpAST_pool[ret] = false;
			return ret;
		}

	};

	//根据var_name在EnvTree中查找Object*;
	Object* VariableExp::eval(EnvTreeList env){
		//std::cout << "var_name is " << var_name << std::endl;
		auto cur_env = env.head;
		while (cur_env) {
			auto iter = cur_env->ptr_to_tree->find(var_name);
			if (iter != cur_env->ptr_to_tree->end()){
				return iter->second;
			}
			cur_env = cur_env->next;
		}

		std::cerr << "could n't find the variable " << var_name << std::endl;
		return nullptr;
	}


	Object* CallProcedureExp::eval(EnvTreeList env){

		//首先判断是否为内置函数
		//内置函数被注册在Globalvariable的map中，它们为重载了operator()的类，它们与Object类中间有一个BuiltIn 抽象基类的中间层
		if (func->_exp_type == ExpAST_TYPE::VARIABLE_TYPE){
			VariableExp* var = (VariableExp*)func;
			auto iter = GlobalVariable->find(var->var_name);
			if (iter != GlobalVariable->end() && iter->second->obtype == ObjectType::BuiltIn_OBJ){
					BuiltIn *_func = (BuiltIn*)(iter->second);
					size_t n = parameters.size();
					std::vector<Object*> args(n);
					for (size_t i = 0; i != n; ++i) {
						Object* tmp = parameters[i]->eval(env);
						args[i] = tmp;
					}
					return (*_func)(args);
			}
		}

		Object* func_prototype = (func->eval(env));

		if (func_prototype->obtype == ObjectType::BuiltIn_OBJ){
			BuiltIn *_func = (BuiltIn*)func_prototype;
			size_t n = parameters.size();
			std::vector<Object*> args(n);
			for (size_t i = 0; i != n; ++i) {
				args[i] = parameters[i]->eval(env);
			}
			return (*_func)(args);
		}


		
		//如果是可调用对象, Procedure或是Closure
		if (func_prototype && (func_prototype->obtype == ObjectType::PROCEDURE_OBJ || func_prototype->obtype == ObjectType::CLOSURE_OBJ)){
			
			Procedure* proc;

			
			if (func_prototype->obtype == ObjectType::CLOSURE_OBJ){
				proc = ((Closure*)func_prototype)->proc;
				EnvTreeList closure_env = ((Closure*)func_prototype)->env;
				env = env.push_front(closure_env.head->ptr_to_tree);
			}
			else{
				proc = (Procedure*)func_prototype;
			}


			if (proc->args.size() != parameters.size()){
				Fout::error_out("invalid procedure call, incompatible aguments number, expect aguments number", proc->args.size(), ", given", parameters.size(), "parameters\n");
				return nullptr;
			}

			//Construct local enviroment
			EnvTree* localvariable = new EnvTree();
			EnvTreeList local_env = env.push_front(localvariable);

			for (size_t i = 0; i != parameters.size(); ++i) {
				if (parameters[i]->_exp_type == VARIABLE_TYPE){
					VariableExp* ptr = (VariableExp*)parameters[i];
					Object* _obj = ptr->eval(env);
					if (!_obj)
						return nullptr;
					else{
						(*localvariable)[proc->args[i]] = _obj->deep_copy();
					}
				}
				else if (Object* p = (this->parameters)[i]->eval(env)){
					(*localvariable)[proc->args[i]] = p;
				}
				else{
					Fout::error_out("Error in parameter eval\n");
					return nullptr;
				}
			}

			Object* ret = nullptr;

			for (int i = 0; i < proc->exprs.size(); ++i) {
				ret = proc->exprs[i]->eval(local_env);
			}


			if (ret && ret->obtype == ObjectType::PROCEDURE_OBJ){
				Closure* closure_func = Closure::factory(local_env, (Procedure*)ret);
				return closure_func;
			}
			else{
				local_env.clearLocal();
				return ret;
			}
		}
		else{
			Fout::error_out("Invalid procedure call\n");
			return nullptr;
		}
	}



	Object* DefineVariableExp::eval(EnvTreeList env){
		Object* ret = this->_expr->eval(env);
		if (ret){
			(*env.head->ptr_to_tree)[this->_name] = ret;
		}
		else{
			Fout::error_out("Invalid expression in DefineVariableExp\n");
		}
		return nullptr;
	}

	static bool eval_boolean(ExpAST* ifexp, bool& res, EnvTreeList env);

	Object* IfelseExp::eval(EnvTreeList env){

		bool res = true;

		if (!eval_boolean(this->ifexp, res, env)){
			std::cerr << "invalid condition expression" << std::endl;
			return nullptr;
		}

		if (res){
			return this->thenexp->eval(env);
		}
		else{
			return this->elseexp->eval(env);
		}

	}

	Object* CondExp::eval(EnvTreeList env){

		size_t n = conds.size();
		size_t m = rets.size();
		if (n != m || n == 0){
			Fout::error_out("Invalid CondExp conditions\n");
			return nullptr;
		}
		Object *final_result = nullptr;
		bool condBool = true;
		for (size_t idx = 0; idx != n - 1; ++idx) {
			if (eval_boolean(conds[idx], condBool, env)){
				if (condBool){
					int expr_number = rets[idx].size();
					if (expr_number == 0){
						Fout::error_out("Error cond expression return", "\n");
						return nullptr;
					}

					for (int i = 0; i < expr_number; ++i) {
						final_result = rets[idx][i]->eval(env);
					}
					return final_result;
				}
			}
			else{
				Fout::error_out("Error in parsing cond expression\n");
				return nullptr;
			}
		}

		int expr_number = rets.back().size();

		for (int i = 0; i < expr_number; ++i) {
			final_result = rets.back()[i]->eval(env);
		}

		return final_result;
	}


	//将表达式结果获得的Object*转换为布尔值
	static bool eval_boolean(ExpAST* ifexp, bool& res, EnvTreeList env){

		Object* cond = ifexp->eval(env);

		if (!cond){
			std::cerr << "invalid condition expression" << std::endl;
			return false;
		}

		bool condBool = true;

		res = true;

		switch (cond->obtype)
		{
		case ObjectType::BOOL_OBJ:
		{
			BoolValue* ptr = (BoolValue*)cond;
			if (!ptr->value){
				res = false;
			}
			break;
		}
		case ObjectType::INTEGER_OBJ:
		{
			IntegerValue* ptr = (IntegerValue*)cond;
			if (ptr->value == 0){
				res = false;
			}
			break;
		}
		case ObjectType::DOUBLE_OBJ:
		{
			DoubleValue* ptr = (DoubleValue*)cond;
			if (ptr->value == 0){
				res = false;
			}
			break;
		}
		case ObjectType::PROCEDURE_OBJ:
		{
			res = true;
			break;
		}
		default:
			return false;
			break;
		}
		return true;

	}

	Object* ProcedureExp::eval(EnvTreeList env){
		Procedure* ret = Procedure::factory();
		ret->args = this->args;
		ret->exprs = this->exprs;
		return ret;
	}


	Object* DefineProcedureExp::eval(EnvTreeList env){
		Procedure *proc = Procedure::factory();
		proc->args = this->_args;
		proc->exprs = this->exprs;

		(*env.head->ptr_to_tree)[_name] = proc;

		return nullptr;
	}


	Object* VectorExp::eval(EnvTreeList env){
		VectorValue *ret = VectorValue::factory(this->vecs.size ());

		for (size_t i = 0; i != vecs.size(); ++i) {
			ret->vec[i] = this->vecs[i]->eval(env);
		}
		return ret;
	}

}

#endif

