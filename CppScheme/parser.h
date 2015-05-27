#ifndef _MINISCHEME_PARSER_H
#define _MINISCHEME_PARSER_H
#include "lexer.h"
#include "ExpAST.h"
#include <map>
#include <list>
#include "slist.h"
#include <iostream>
#define DEBUG(S) std::cout<<(S)<<std::endl;

using namespace CppScheme;

//Scheme的范式比较简单，包括
// number				常量数值·1
// variable				对象,可以是变量或是过程
// if else expr			条件语句
// (define x expr)		定义语句，注册一个对象到当前环境
// (proc arg ... )		过程调用语句

//延伸出来的一些语法包括
//(lambda (arg ...)	(expr）					定义一个匿名函数
//(let ((v1 exp1) (v2 exp2) ...) (expr..)	定义一个匿名函数过程，并且立刻调用
//(cond (c1 r1) (c2 r2) ... (else rx))		条件语句



ExpAST* parseExpAst(std::istream& in, EnvTreeList current_env, int left);

ExpAST* parseIFElseAST(std::istream& in, EnvTreeList current_env);

void parseArgs(std::istream& in, std::vector< std::string > &args);

ExpAST* parseCondExp(std::istream& in, EnvTreeList current_env);

bool parseCondExp(std::istream& in, EnvTreeList current_env, std::vector<ExpAST*> &conds, std::vector<std::vector<ExpAST*>> &rets);

ExpAST* parseCallExp(std::istream& in, EnvTreeList current_env);

ExpAST* parseDefineExp(std::istream& in, EnvTreeList current_env);

ExpAST* parseLambdaExp(std::istream& in, EnvTreeList current_env);

ExpAST* parseLetExp(std::istream& in, EnvTreeList current_env);

void parseLetArgs(std::istream& in, EnvTreeList current_env, std::vector<std::string>& args, std::vector<ExpAST*>& parameters);






//Main parser function
ExpAST* parseExpAst(std::istream& in, EnvTreeList current_env, int left = 0){
	if (left == 0){
		TOKEN _tok = get_token(in);
		switch (_tok)
		{
		case TOKEN::NUMBER:
		{
			SimpleExp* ret = SimpleExp::factory();
			ret->obj = DoubleValue::factory(current_double);
			return ret;
		}
		case TOKEN::IDENTIFIER:
			return VariableExp::factory(current_identifer);
		case TOKEN::LBRACE:
			return parseExpAst(in, current_env, 1);
		case TOKEN::RBRACE:
			return nullptr;
		case TOKEN::ELSE:
			return nullptr;
		case TOKEN::EXIT:
			return new ExitExp();
		default:
			std::cerr << "unknown token  " << current_line_number << std::endl;
			return nullptr;
		}
	}
	else {
		TOKEN _tok = get_token(in);
		switch (_tok)
		{
		case TOKEN::DEFINE:
		{
			ExpAST* def = parseDefineExp(in, current_env);
			return def;
		}
		case TOKEN::IF:
			return parseIFElseAST(in, current_env);
		case TOKEN::LAMBDA:
			return parseLambdaExp(in, current_env);
		case TOKEN::COND:
			return parseCondExp(in, current_env);
		case TOKEN::LET:
			return parseLetExp(in, current_env);
		case TOKEN::RBRACE:
			return nullptr;
		case TOKEN::NUMBER:
			std::cerr << "Uncallable with Number " << current_line_number << std::endl;
			return nullptr;
		default:
			push(_tok);
			return parseCallExp(in, current_env);
		}
	}
}


// define a variable or function is different;
//**********************************************************
//******************* define procedure *********************
//		(define   (func args...) (expr1) (expr2) (expr3) .... (expr n) )
//                Λ                              Λ
//                |                              |
//           function start token;              eat the last brace;
//
//************************* or *****************************
//******************* define variable **********************
//
//		(define   variable_name   one_expression )
//                Λ                        Λ
//                |                        |
//           function start token;        eat the last brace;
//**********************************************************
ExpAST* parseDefineExp(std::istream& in, EnvTreeList current_env){
	std::string _name;
	TOKEN _tok = get_token(in);
	if (_tok == TOKEN::LBRACE){
		_tok = get_token(in);

		if (_tok != IDENTIFIER){
			std::cerr << "invalid name at " << current_line_number << std::endl;
			return nullptr;
		}

		DefineProcedureExp *result = DefineProcedureExp::factory ();
		result->_name = current_identifer;
		//parse aguments
		parseArgs(in, result->_args);

		//parse procedure expressions;
		while (ExpAST* exp = parseExpAst (in, current_env)) {
			result->exprs.push_back(exp);
		}

		return result;
	}
	else if (_tok == TOKEN::IDENTIFIER){
		DefineVariableExp* result = DefineVariableExp::factory ();
		result->_name = current_identifer;
		result->_expr = parseExpAst(in, current_env);
		_tok = get_token(in);
		if (_tok != TOKEN::RBRACE){
			std::cerr << "Invalid Define Syntax" << std::endl;
			return nullptr;
		}
		return result;
	}
	else{
		std::cerr << "Invalid define syntax " << current_line_number << std::endl;
		return nullptr;
	}

}



void parseArgs(std::istream& in, std::vector < std::string > &args){	//构建参数列表
	//    ( .. arg1 arg2 arg3 ... ...)
	//         Λ                     Λ
	//         |                     |
	//     function start;          eat the last brace;

	TOKEN cur_token = get_token(in);
	while (cur_token == TOKEN::IDENTIFIER) {
		if (cur_token == TOKEN::IDENTIFIER){
			args.push_back(current_identifer);
		}
		cur_token = get_token(in);
	}

	if (cur_token != TOKEN::RBRACE){	//eat ')'
		std::cerr << "invalid end, expect the RBRACE token" << std::endl;
	}
}



ExpAST* parseLambdaExp(std::istream& in, EnvTreeList current_env){
	//eat last right brace:: (lambda  (args) (expr 1) (expr 2) (expr 3)... (expr n) ) 
	//								Λ			                                    Λ
	//								|			                                    |	
	//		this function start from here;                               eat this brace

	std::cout << "parseing lambda exp ................" << std::endl;
	TOKEN _tok = get_token(in);
	if (_tok != LBRACE){
		std::cerr << "Invalid syntax, expect to be LBRACE \')\', line number " << current_line_number << std::endl;
		return nullptr;
	}

	ProcedureExp* result = ProcedureExp::factory ();
	parseArgs(in, result->args);

	while (ExpAST* exp = parseExpAst (in, current_env)) {
		result->exprs.push_back(exp);
	}


	return result;
}




// ***************  ifelse expression syntax ***************
//       ( if condition_expr then_expr else_expr )
//            Λ                                  Λ
//            |                                  |
//          function start                       eat
//**********************************************************
ExpAST* parseIFElseAST(std::istream& in, EnvTreeList current_env){

	IfelseExp* result = IfelseExp::factory ();

	result->ifexp = parseExpAst(in, current_env, 0);
	result->thenexp = parseExpAst(in, current_env, 0);
	result->elseexp = parseExpAst(in, current_env, 0);
	
	TOKEN _tok = get_token(in);
	if (_tok != TOKEN::RBRACE){
		std::cerr << "Invalid IfElse expression, expect right brace at end" << std::endl;
		return nullptr;
	}

	return result;
}

//************************************ cond expression syntax **************************************
// (cond  (cond1 ret1_1 ... ret1_n ) (cond1 ret2_1 ... ret2_n ) ... ( else retx_1 ... retx_n) )
//        Λ   Λ                                                                               Λ
//        |   |                                                                               |
//func start  get the conditions and returns                                         eat the last lbrace;
//**************************************************************************************************
ExpAST* parseCondExp(std::istream& in, EnvTreeList current_env){

	CondExp* result = CondExp::factory ();

	if (!parseCondExp(in, current_env, result->conds, result->rets)){
		return nullptr;
	}

	return result;
}


//get the condition and return expAST of cond expression
bool parseCondExp(std::istream& in, EnvTreeList current_env, std::vector<ExpAST*> &conds, std::vector<std::vector<ExpAST*>> &rets){

	// (cond  (cond1 ret1_1 ret1_2 ... ret1_n ) (cond1 ret2_1 ret2_2 ... ret2_n ) ... ( else retx_1 retx_2 ... retx_n) )

	TOKEN _tok = get_token(in);
	
	ExpAST* exp1 = nullptr, *exp2 = nullptr;

	int pos = 0;
	while(_tok == TOKEN::LBRACE) {
		exp1 = parseExpAst(in, current_env);
		conds.push_back(exp1);
		rets.push_back(std::vector<ExpAST*>());
		while (exp2 = parseExpAst (in, current_env)) {
			rets[pos].push_back(exp2);
		}

		++pos;
		_tok = get_token(in);
	}


	if (exp1 != nullptr || _tok != TOKEN::RBRACE){
		std::cerr << "Cond expression syntax error" << std::endl;
		return false;
	}

	return true;
}


//***************************** Call Procedure Syntax ****************************
// CallExpression ->    (expr_of_procedure  arg1  arg2  arg3  ...  )
//                       Λ                                         Λ
//                       |                                         |
//                   function start;                     no need to eat explict;
//********************************************************************************

ExpAST* parseCallExp(std::istream& in, EnvTreeList current_env){

	CallProcedureExp *result = CallProcedureExp::factory ();
	result->func = parseExpAst(in, current_env);

	if (result->func == nullptr){
		std::cerr << "invalid function call " << current_line_number << std::endl;
		return nullptr;
	}

	//if get TOKEN::RBRACE return nullptr
	while (ExpAST* exp = parseExpAst(in, current_env)) {
		result->parameters.push_back(exp);
	}

	return result;
}



//let语句其实就是一个调用lambda语句的语法糖，变量初始化阶段等同于给予lambda函数
//可以直接解析为CallProcedureExp
// *************************** Let Expression Syntax ************************************************
// **************************************************************************************************
//let syntax:: (let   ( (v1 expr1)  (v2 epxr2)  (v3 expr3) ... (vx exprx) )    expr1 expr2 ... exprn )
//               Λ    Λ                                                   Λ                          Λ
//               |    |                                                   |                          |
//    function start    aguments parse start                    aguments parse end           eat the last brace;
// **************************************************************************************************
ExpAST* parseLetExp(std::istream& in, EnvTreeList current_env){
	
	CallProcedureExp* result = CallProcedureExp::factory ();
	ProcedureExp* proc = ProcedureExp::factory ();

	parseLetArgs(in, current_env, proc->args, result->parameters);



	while (ExpAST* exp = parseExpAst (in, current_env)) {
		proc->exprs.push_back(exp);
	}
	result->func = proc;

	if (current_tok != TOKEN::RBRACE){
		std::cerr << "expect to be right brace, invalid let syntax " << current_line_number << std::endl;
		return nullptr;
	}

	return result;
}


void parseLetArgs(std::istream& in, EnvTreeList current_env, std::vector<std::string>& args, std::vector<ExpAST*>& parameters){
	//let syntax:: (let   ( (v1 expr1)  (v2 epxr2)  (v3 expr3) ... (vx exprx) )    expr1 expr2 ... exprn )

	TOKEN _tok = get_token(in);
	if (_tok != LBRACE){
		std::cerr << "expect to be Left braec '(' " << current_line_number << std::endl;
		return;
	}

	while (1) {
		_tok = get_token(in);
		if (_tok != TOKEN::LBRACE && _tok != TOKEN::RBRACE){
			std::cerr << "expect to Left brace or right brace, invalid let syntax " << current_line_number << std::endl;
			return;
		}
		if (_tok == TOKEN::RBRACE){
			break;
		}

		_tok = get_token(in);

		if (_tok != TOKEN::IDENTIFIER){
			std::cerr << "invalid let syntax, expect to be identifier " << current_line_number << std::endl;
			return;
		}

		args.push_back(current_identifer);

		ExpAST* exp = parseExpAst(in, current_env);
		if (exp == nullptr){
			std::cerr << "invalid exprssion in let syntax " << current_line_number << std::endl;
			return;
		}

		parameters.push_back(exp);
		_tok = get_token(in);

		if (_tok != TOKEN::RBRACE){
			std::cerr << "invalid let syntax, expect to be right brace ')'" << current_line_number << std::endl;
			return;
		}
	}

}


#endif //_MINISCHEME_PARSER_H