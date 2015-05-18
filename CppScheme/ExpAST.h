#ifndef _MINISCHEME_EXPAST_H
#define _MINISCHEME_EXPAST_H


#include <string>
#include <vector>
#include "Object.h"
#include <map>
#include "slist.h"

namespace MiniScheme{

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


	//need??
	class FunctionProto{
	public:
		std::string func_name;
		std::vector<std::string> args;
		ExpAST* expr;
	};


	//represent the lamda expression
	class ProcedureExp :public ExpAST{
	public:
		std::vector<std::string> args;
		ExpAST* expr;
		Object* eval(EnvTreeList) override;
	};


	class CondExp :public ExpAST{
	public:
		std::vector<ExpAST*> conds;
		std::vector<ExpAST*> rets;
		Object* eval(EnvTreeList) override;
	};


	class LetExp :public ExpAST{
	public:
		
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
		
		if (Procedure* proc = dynamic_cast<Procedure*>(func->eval (env))){

			//构建局部变量环境
			EnvTree* localvariable = new EnvTree();
			EnvTreeList local_env = env.push_front(localvariable);

			if (proc->args.size () != parameters.size ()){
				std::cerr << "invalid procedure call, incompatible aguments number, expect aguments number" << proc->args.size() << ", give paramenters " << parameters.size() << std::endl;
				return nullptr;
			}
			
			size_t args_number = parameters.size();

			//??
			for (size_t i = 0; i != args_number; ++i) {
				(*localvariable)[proc->args[i]] = (this->parameters)[i]->eval(env);
			}

			Object* ret = proc->expr->eval(local_env);
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




}




#endif

