#ifndef _MINISCHEME_EXPAST_H
#define _MINISCHEME_EXPAST_H


#include <string>
#include <vector>
#include <map>
#include "Object.h"
#include "slist.h"
#include "build_in.h"
using namespace CppScheme;

namespace CppScheme{
	class BuiltIn;

	class ExpAST{
	public:
		typedef std::map<std::string, Object*> EnvTree;
		typedef slist<EnvTree> EnvTreeList;

		virtual Object* eval(EnvTreeList) = 0;
		virtual ~ExpAST(){}
	};


	//Simple ExpAST
	//represent the DoubleValue;
	class SimpleExp :public ExpAST{
	public:
		Object* obj;
		SimpleExp(Object* _obj) :obj(_obj){}
		SimpleExp() :obj(nullptr){}
		Object* eval(EnvTreeList){ return obj; }
		~SimpleExp(){}
	};


	//一个表示仅有变量名称的表达式
	//它的codgen将会在给予它的Envtreelist中查找它的Object
	class VariableExp :public ExpAST{
	public:
		std::string var_name;
		VariableExp(std::string _name) :var_name(_name){}
		Object* eval(EnvTreeList) override;
	};


	//调用过程的表达式
	//如果有名字，则以name进行调用，这个procedure应该是在此处可以访问
	//如果没有名字，那是一个匿名函数，通过Procedure进行调用
	class CallProcedureExp :public ExpAST{
	public:
		ExpAST* func;
		std::vector<ExpAST*> parameters;
		Object* eval(EnvTreeList) override;
		~CallProcedureExp() override;
	};



	//define variable syntax
	//		(define   variable_name   one_expression )

	class DefineVariableExp :public ExpAST{
	public:
		std::string _name;
		ExpAST* _expr;
		Object* eval(EnvTreeList) override;
		virtual ~DefineVariableExp(){
			delete _expr;
		}
	};

	//define procedure expression
	// define a variable or function is different;
	//		(define   (func args...) (expr1) (expr2) (expr3) .... (expr n) )
	class DefineProcedureExp :public ExpAST{
	public:
		std::string _name;
		std::vector<std::string> _args;
		std::vector<ExpAST*> exprs;
		Object* eval(EnvTreeList) override;
		virtual ~DefineProcedureExp() { };
	};


	//IF语句生成的ExpAST;
	class IfelseExp :public ExpAST{
	public:
		ExpAST* ifexp, *thenexp, *elseexp;

		Object* eval(EnvTreeList)override;
		virtual ~IfelseExp(){
			delete ifexp;
			delete thenexp;
			delete elseexp;
		}
	};


	//represent the lamda expression
	//eval生成一个Procedure Object
	class ProcedureExp :public ExpAST{
	public:
		std::vector<std::string> args;
		std::vector<ExpAST*> exprs;
		Object* eval(EnvTreeList) override;
	};

	//表示cond起始的条件语句
	class CondExp :public ExpAST{
	public:
		std::vector<ExpAST*> conds;
		std::vector<std::vector<ExpAST*>> rets;
		Object* eval(EnvTreeList) override;
	};

	//根据var_name在EnvTree中查找Object*;
	Object* VariableExp::eval(EnvTreeList env){
		//std::cout << "var_name is " << var_name << std::endl;
		auto cur_env = env.head;
		while (cur_env) {
			auto iter = cur_env->ptr_to_data->find(var_name);
			if (iter != cur_env->ptr_to_data->end()){
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
		if (VariableExp* var = dynamic_cast<VariableExp*>(func)){
			auto iter = GlobalVariable->find(var->var_name);
			if (iter != GlobalVariable->end()){
				if (BuiltIn *func = dynamic_cast<BuiltIn*>(iter->second)){
					size_t n = parameters.size();
					std::vector<Object*> args(n);
					for (size_t i = 0; i != n; ++i) {
						args[i] = parameters[i]->eval(env);
					}
					return (*func)(args);
				}
			}

		}

		if (Procedure* proc = (Procedure*)(func->eval(env))){

			if (proc->args.size() != parameters.size()){
				std::cerr << "invalid procedure call, incompatible aguments number, expect aguments number" << proc->args.size() << ", give paramenters " << parameters.size() << std::endl;
				return nullptr;
			}

			//构建局部变量环境
			EnvTree* localvariable = new EnvTree();
			EnvTreeList local_env = env.push_front(localvariable);


			size_t args_number = parameters.size();

			//??
			for (size_t i = 0; i != args_number; ++i) {

				if (Object* p = (this->parameters)[i]->eval(env)){
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
			
			auto iter = env.head->ptr_to_data->find(this->_name);

			if (iter != env.head->ptr_to_data->end ()){
				delete iter->second;
				iter->second = ret;
			}
			else{
				(*env.head->ptr_to_data)[this->_name] = ret;
			}
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

		if (BoolValue* ptr = dynamic_cast<BoolValue*>(cond)){
			if (ptr->value){
				std::cout << "Condition is true" << std::endl;
			}

			if (!ptr->value){
				res = false;
			}
		}
		else if (DoubleValue* ptr = dynamic_cast<DoubleValue*>(cond)){
			if (ptr->value == 0){
				res = false;
			}
		}
		else if (Procedure* ptr = dynamic_cast<Procedure*>(cond)){
			res = true;
		}
		else{
			return false;
		}

		return true;

	}

	Object* ProcedureExp::eval(EnvTreeList env){
		Procedure* ret = new Procedure();
		ret->args = this->args;
		ret->exprs = this->exprs;
		return ret;
	}


	Object* DefineProcedureExp::eval(EnvTreeList env){
		Procedure *proc = new Procedure();
		proc->args = this->_args;
		proc->exprs = this->exprs;
		
		auto iter = env.head->ptr_to_data->find(_name);

		if (iter != env.head->ptr_to_data->end ()){
			delete iter->second;
			iter->second = proc;
		}
		else{
			(*env.head->ptr_to_data)[_name] = proc;
		}

		return nullptr;
	}


	CallProcedureExp::~CallProcedureExp(){

			delete func;

		//parameter ExpAST*的删除同func是类似的，删除临时的ExpAST，从局部环境中提取而得到的变量则交由EnvTree管理
		for (size_t i = 0; i != this->parameters.size(); ++i) {
			delete parameters[i];
		}
	}

}




#endif

