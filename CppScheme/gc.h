#ifndef _MINISCHEME_GARBAGE_COLLECTION_H
#define _MINISCHEME_GARBAGE_COLLECTION_H
#include "ExpAST.h"
#include <unordered_set>

namespace CppScheme{
	static void dfs_object(Object* cur_obj, std::unordered_set<ExpAST*> &exp_visited, std::unordered_set<Object*> &obj_visited);
	static void dfs_expast(ExpAST* cur_expast, std::unordered_set<ExpAST*> &exp_visited, std::unordered_set<Object*> &obj_visited);
	static void mark_sweep(){
		std::unordered_set<ExpAST*> exp_visited;
		std::unordered_set<Object*> obj_visited;

		for (auto iter = Object_pool.begin(); iter != Object_pool.end(); ++iter) {
			iter->second = false;
		}

		for (auto iter = ExpAST_pool.begin(); iter != ExpAST_pool.end(); ++iter) {
			iter->second = false;
		}

		for (auto iter = GlobalVariable->begin(); iter != GlobalVariable->end(); ++iter) {
			//不处理BuiltIn function
			if (iter->second && iter->second->obtype != ObjectType::BuiltIn_OBJ){
				dfs_object(iter->second, exp_visited, obj_visited);
			}
		}

		for (auto iter = Object_pool.begin(); iter != Object_pool.end();) {
			if (!iter->second){
				Object* ptr = iter->first;
				delete ptr;
				Object_pool.erase(iter++);
			}
			else{
				++iter;
			}
		}

		for (auto iter = ExpAST_pool.begin(); iter != ExpAST_pool.end();) {
			if (!iter->second){
				ExpAST* ptr = iter->first;
				delete ptr;
				ExpAST_pool.erase(iter++);
			}
			else{
				++iter;
			}
		}
	}



	static void dfs_object(Object* cur_obj, std::unordered_set<ExpAST*> &exp_visited, std::unordered_set<Object*> &obj_visited){

		obj_visited.insert(cur_obj);
		Object_pool[cur_obj] = true;

		switch (cur_obj->obtype)
		{
		case ObjectType::DOUBLE_OBJ:
			break;
		case ObjectType::INTEGER_OBJ:
			break;
		case ObjectType::PROCEDURE_OBJ:
		{
			Procedure* proc = (Procedure*)cur_obj;
			for (size_t i = 0; i != proc->exprs.size(); ++i) {
				auto iter = exp_visited.find(proc->exprs[i]);
				//类似condition ExpAST中的nullptr是不需要进行搜索的
				if (proc->exprs[i] && iter == exp_visited.end()){
					dfs_expast(proc->exprs[i], exp_visited, obj_visited);
				}
			}
			break;
		}
		case ObjectType::BOOL_OBJ:
			break;
		case ObjectType::PAIR_OBJ:
		{
			Pair* p = (Pair*)cur_obj;
			//确认序对指向的对象未访问过才进行搜索
			//序对可能会导致闭环，所以这个步骤是必不可少的
			if (p->first->obtype != ObjectType::NULL_OBJ && obj_visited.find(p->first) == obj_visited.end()){
				dfs_object(p->first, exp_visited, obj_visited);
			}

			if (p->second->obtype != ObjectType::NULL_OBJ && obj_visited.find(p->second) == obj_visited.end()){
				dfs_object(p->second, exp_visited, obj_visited);
			}
			break;
		}
		case ObjectType::CLOSURE_OBJ:
		{
			Closure* closur_obj = (Closure*)cur_obj;
			if (obj_visited.find (closur_obj->proc) == obj_visited.end ()){
				dfs_object(closur_obj->proc, exp_visited, obj_visited);
			}

			EnvTree* closure_env = closur_obj->env.head->ptr_to_tree;

			for (auto iter = closure_env->begin(); iter != closure_env->end(); ++iter){

				Object* var = iter->second;
				if (obj_visited.find (var) == obj_visited.end ()){
					dfs_object(var, exp_visited, obj_visited);
				}
			}

		}
		default:
		break;
		}
	}

	static void dfs_expast(ExpAST* cur_expast, std::unordered_set<ExpAST*> &exp_visited, std::unordered_set<Object*> &obj_visited){
		exp_visited.insert(cur_expast);
		ExpAST_pool[cur_expast] = true;

		switch (cur_expast->_exp_type)
		{
		case ExpAST_TYPE::SIMPLE_TYPE:
		{
			SimpleExp* ptr = (SimpleExp*)cur_expast;
			if (obj_visited.find(ptr->obj) == obj_visited.end()){
				dfs_object(ptr->obj, exp_visited, obj_visited);
			}
			break;
		}
		case ExpAST_TYPE::EXIT_TYPE:
			break;
		case ExpAST_TYPE::CALLPROCEDURE_TYPE:
		{
			CallProcedureExp* ptr = (CallProcedureExp*)cur_expast;
			if (exp_visited.find(ptr->func) == exp_visited.end()){
				dfs_expast(ptr->func, exp_visited, obj_visited);
			}

			for (size_t i = 0; i != ptr->parameters.size(); ++i) {
				if (exp_visited.find(ptr->parameters[i]) == exp_visited.end()){
					dfs_expast(ptr->parameters[i], exp_visited, obj_visited);
				}
			}

			break;
		}
		case ExpAST_TYPE::COND_EXP:
		{
			CondExp* ptr = (CondExp*)cur_expast;
			for (auto exp : ptr->conds) {
				if (exp && exp_visited.find(exp) == exp_visited.end()){
					dfs_expast(exp, exp_visited, obj_visited);
				}
			}

			for (auto exp_vec : ptr->rets) {
				for (auto exp : exp_vec) {
					if (exp && exp_visited.find(exp) == exp_visited.end()){
						dfs_expast(exp, exp_visited, obj_visited);
					}
				}
			}

			break;
		}
		case ExpAST_TYPE::DEFINEPROCEDURE_TYPE:
		{
			DefineProcedureExp* ptr = (DefineProcedureExp*)cur_expast;

			for (auto exp : ptr->exprs) {
				if (exp && exp_visited.find(exp) == exp_visited.end()){
					dfs_expast(exp, exp_visited, obj_visited);
				}
			}
		}
		break;
		case ExpAST_TYPE::DEFINEVARIABLE_TYPE:
		{
			DefineVariableExp* ptr = (DefineVariableExp*)cur_expast;

			if (ptr->_expr && exp_visited.find(ptr->_expr) == exp_visited.end()){
				dfs_expast(ptr->_expr, exp_visited, obj_visited);
			}
			break;
		}
		case ExpAST_TYPE::IFELSE_TYPE:
		{
			IfelseExp* ptr = (IfelseExp*)cur_expast;

			if (ptr->ifexp && exp_visited.find(ptr->ifexp) == exp_visited.end()){
				dfs_expast(ptr->ifexp, exp_visited, obj_visited);
			}

			if (ptr->thenexp && exp_visited.find(ptr->thenexp) == exp_visited.end()){
				dfs_expast(ptr->thenexp, exp_visited, obj_visited);
			}

			if (ptr->elseexp && exp_visited.find(ptr->elseexp) == exp_visited.end()){
				dfs_expast(ptr->elseexp, exp_visited, obj_visited);
			}

			break;
		}
		case ExpAST_TYPE::PROCEDURE_TYPE:
		{
			ProcedureExp* ptr = (ProcedureExp*)cur_expast;

			for (auto exp : ptr->exprs) {
				if (exp && exp_visited.find(exp) == exp_visited.end()){
					dfs_expast(exp, exp_visited, obj_visited);
				}
			}

			break;
		}
		case ExpAST_TYPE::VARIABLE_TYPE:
			break;
		default:
			std::cout << "unknown ExpAST_TYPE" << std::endl;
			break;
		}
	}
}



#endif // _MINISCHEME_GARBAGE_COLLECTION_H