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

		static SimpleExp* factory(){
			SimpleExp* ret = new SimpleExp();
			ExpAST_pool[ret] = false;
			return ret;
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

		std::cerr << "couldn't find the variable " << var_name << std::endl;
		return nullptr;
	}


	Object* CallProcedureExp::eval(EnvTreeList env){

		//首先判断是否为内置函数
		//内置函数被注册在Globalvariable的map中，它们为重载了operator()的类，它们与Object类中间有一个BuiltIn 抽象基类的中间层
		if (func->_exp_type == ExpAST_TYPE::VARIABLE_TYPE){
			VariableExp* var = (VariableExp*)func;
			auto iter = GlobalVariable->find(var->var_name);
			if (iter != GlobalVariable->end()){
				if (iter->second->obtype == ObjectType::BuiltIn_OBJ){
					BuiltIn *_func = (BuiltIn*)(iter->second);
					size_t n = parameters.size();
					std::vector<Object*> args(n);
					for (size_t i = 0; i != n; ++i) {
						Object* tmp = parameters[i]->eval(env);
						args[i] = tmp;
					}
					Object* ret = (*_func)(args);
					return ret;
				}
			}
		}

		if (Procedure* proc = (Procedure*)(func->eval(env))){

			if (proc->args.size() != parameters.size()){
				std::cerr << "invalid procedure call, incompatible aguments number, expect aguments number" << proc->args.size() << ", give paramenters " << parameters.size() << std::endl;
				return nullptr;
			}

			//Construct local enviroment
			EnvTree* localvariable = new EnvTree();
			EnvTreeList local_env = env.push_front(localvariable);


			size_t args_number = parameters.size();

			//??
			for (size_t i = 0; i != args_number; ++i) {

				if (VariableExp* ptr = dynamic_cast<VariableExp*>(parameters[i])){
					(*localvariable)[proc->args[i]] = ptr->eval(env)->deep_copy();
				}
				else if (Object* p = (this->parameters)[i]->eval(env)){
					(*localvariable)[proc->args[i]] = p;
				}
				else{
					std::cerr << "Error in parameter eval" << std::endl;
					return nullptr;
				}
			}

			int expr_number = proc->exprs.size();
			Object* ret = nullptr;

			for (int i = 0; i < expr_number; ++i) {
				ret = proc->exprs[i]->eval(local_env);
			}

			local_env.clearLocal();

			return ret;
		}
		else{
			std::cerr << "invalid procedure call" << std::endl;
			return nullptr;
		}
	}

	Object* DefineVariableExp::eval(EnvTreeList env){
		Object* ret = this->_expr->eval(env);

		if (ret){
			(*env.head->ptr_to_tree)[this->_name] = ret;
		}
		else{
			std::cerr << "Invalid expression in DefineVariableExp" << std::endl;
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
			std::cerr << "Invalid CondExp conditons" << std::endl;
			return nullptr;
		}
		Object *final_result = nullptr;
		bool condBool = true;
		for (size_t idx = 0; idx != n - 1; ++idx) {
			if (eval_boolean(conds[idx], condBool, env)){
				if (condBool){
					std::cout << "true" << std::endl;
					int expr_number = rets[idx].size();
					if (expr_number == 0){
						std::cout << "Error cond expression return" << std::endl;
						return nullptr;
					}

					for (int i = 0; i < expr_number; ++i) {
						final_result = rets[idx][i]->eval(env);
					}
					return final_result;
				}
			}
			else{
				std::cerr << "Invalid Cond Expression " << std::endl;
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

}

#endif

