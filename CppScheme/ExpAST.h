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


	//��Ϊ�򵥵ı��ʽ
	//��ֱ�Ӹ���һ����ֵ
	//����ֱ�Ӹ���һ��Procedure
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


	//һ����ʾ���б������Ƶı��ʽ
	//����codgen�����ڸ�������Envtreelist�в�������Object
	//��������local enviroment�����٣�ֻ���𱣴�����Ͳ���
	//local envriment �������������͵�
	class VariableExp :public ExpAST{
	public:
		std::string var_name;
		VariableExp(std::string _name ) :var_name(_name){}
		Object* eval(EnvTreeList) override;
	};

	



	//���ù��̵ı��ʽ
	//��������֣�����name���е��ã����procedureӦ�����ڴ˴����Է���
	//���û�����֣�����һ������������ͨ��Procedure���е���
	class CallProcedureExp :public ExpAST{
	public:
		ExpAST* func;
		std::vector<ExpAST*> parameters;

		Object* eval(EnvTreeList) override;
	};


	//define��������ExpAST
	//define��䲻��һ���ս���䣬define����ܽ�����������ExpAST
	class DefineExp :public ExpAST{
	public:
		std::string name;
		ExpAST* expr;
		ExpAST* next;
		DefineExp() :next(nullptr){}
		Object* eval(EnvTreeList) override;
	};


	//IF������ɵ�ExpAST;
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

			//�����ֲ���������
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

