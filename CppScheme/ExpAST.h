#ifndef _MINISCHEME_EXPAST_H
#define _MINISCHEME_EXPAST_H


#include <string>
#include <vector>
#include "Object.h"
#include <map>
#include "slist.h"

namespace MiniScheme{


	//抽象AST的基类
	class ExpAST{
	public:
		typedef std::map<std::string, Object*> EnvTree;
		typedef slist<EnvTree> EnvTreeList;
		virtual ~ExpAST();
		virtual Object* eval(EnvTreeList) = 0;
	};


	//最为简单的表达式
	//如直接给出一个数值
	//或是直接给予一个Procedure
	class SimpleExp :public ExpAST{
	public:
		Object* obj;
		SimpleExp(Object* _obj) :obj(_obj){}
		SimpleExp() :obj(nullptr){}
		Object* eval(EnvTreeList){ return obj; }
		~SimpleExp(){
			delete obj;
		}
	};


	//一个表示仅有变量名称的表达式
	//它的codgen将会在给予它的Envtreelist中查找它的Object
	//它不负责local enviroment的销毁，只负责保存变量和查找
	//local envriment 将会是引用类型的
	class VariableExp :public ExpAST{
	public:
		std::string var_name;
		VariableExp(std::string _name ) :var_name(_name){}
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
	};


	//define语句产生的ExpAST
	//define语句不是一个终结语句，define后可能紧跟着其他的ExpAST
	class DefineExp :public ExpAST{
	public:
		std::string name;
		ExpAST* expr;
		ExpAST* next;
		DefineExp() :next(nullptr){}
		Object* eval(EnvTreeList) override;
	};


	//IF语句生成的ExpAST;
	class IfelseExp :public ExpAST{
	public:
		ExpAST* ifexp, *thenexp, *elseexp;

		Object* eval(EnvTreeList)override;
	};


	//represent the lamda expression
	//eval生成一个Procedure Object
	class ProcedureExp :public ExpAST{
	public:
		std::vector<std::string> args;
		ExpAST* expr;
		Object* eval(EnvTreeList) override;
	};

	//表示cond起始的条件语句
	class CondExp :public ExpAST{
	public:
		std::vector<ExpAST*> conds;
		std::vector<ExpAST*> rets;
		Object* eval(EnvTreeList) override;
	};

	
	Object* VariableExp::eval(EnvTreeList local_env){
		auto cur_env = local_env.head;

		while (cur_env) {

			auto iter = cur_env->ptr_to_data->find(var_name);
			if (iter != cur_env->ptr_to_data->end ()){
				return iter->second;
			}

			cur_env = cur_env->next;
		}

		std::cerr << "couldn't find the variable " << var_name << std::endl;
		return nullptr;
	}

	
	Object* CallProcedureExp::eval(EnvTreeList env){
				

		if (VariableExp* var = dynamic_cast<VariableExp*>(func)){
			
		}
		if (Procedure* proc = dynamic_cast<Procedure*>(func->eval (env))){
			
			if (proc->args.size () != parameters.size ()){
				std::cerr << "invalid procedure call, incompatible aguments number, expect aguments number" << proc->args.size() << ", give paramenters " << parameters.size() << std::endl;
				return nullptr;
			}

			//构建局部变量环境
			EnvTree* localvariable = new EnvTree();
			EnvTreeList local_env = env.push_front(localvariable);

			
			size_t args_number = parameters.size();

			//??
			for (size_t i = 0; i != args_number; ++i) {
				(*localvariable)[proc->args[i]] = (this->parameters)[i]->eval(env);
			}

			Object* ret = proc->expr->eval(local_env);
			
			local_env.clearLocal();
			return ret;
		}
		else{
			std::cerr << "invalid procedure call" << std::endl;
			return nullptr;
		}
	}

	Object* DefineExp::eval(EnvTreeList env){
		Object* ret = this->expr->eval(env);

		(*env.head->ptr_to_data)[this->name] = ret;

		return next->eval(env);
	}

	Object* IfelseExp::eval(EnvTreeList env){

		bool res = true;

		if (!eval_boolean (this->ifexp, res, env)){
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

		bool condBool = true;
		for (size_t idx = 0; idx != n - 1; ++idx) {
			if (eval_boolean(conds[idx], condBool, env)){
				if (condBool){
					return rets[idx]->eval(env);
				}
			}
			else{
				std::cerr << "Invalid Cond Expression " << std::endl;
				return nullptr;
			}
		}
		return rets.back()->eval(env);
	}


	//将表达式结果获得的Object*转换为布尔值
	static bool eval_boolean(ExpAST* ifexp, bool& res, EnvTreeList env){

		Object* cond = ifexp->eval(env);

		if (!cond){
			std::cerr << "invalid condition expression" << std::endl;
			return false;
		}

		bool condBool = true;
		

		if (BoolValue* ptr = dynamic_cast<BoolValue*>(cond)){
			if (!ptr->value){
				res = false;
			}
		}
		else if (IntegerValue* ptr = dynamic_cast<IntegerValue*>(cond)){
			if (ptr->value == 0){
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
	



}




#endif

