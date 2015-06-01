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


ExpAST* parseVecotrExp(std::istream& in, EnvTreeList current_env, bool quasiquote);

ExpAST* parseQuoteExp(std::istream &in, EnvTreeList env, bool quasiquote);



//Main parser function
ExpAST* parseExpAst(std::istream& in, EnvTreeList current_env, int left = 0){
	if (left == 0){
		TOKEN _tok = get_token(in);
		switch (_tok)
		{
		case TOKEN::DOUBLE_TOK:
		{
			return SimpleExp::factory(DoubleValue::factory(current_double));
		}
		case TOKEN::INTEGER_TOK:
		{
			return SimpleExp::factory(IntegerValue::factory(current_int));
		}
		case TOKEN::BOOL_FALSE_TOK:
		{
			return SimpleExp::factory(BoolValue::factory(false));
		}
		case TOKEN::BOOL_TRUE_TOK:
		{
			return SimpleExp::factory(BoolValue::factory(true));
		}
		case TOKEN::STR_TOK:
			return StrExp::factory(current_identifer);
		case TOKEN::IDENTIFIER_TOK:
			return VariableExp::factory(current_identifer);
		case TOKEN::LBRACE_TOK:
			return parseExpAst(in, current_env, 1);
		case TOKEN::RBRACE_TOK:
			return nullptr;
		case TOKEN::ELSE_TOK:
			return nullptr;
		case TOKEN::EXIT_TOK:
			return new ExitExp();
			//常量表达式
			//'<datum>
			//(quote <datum>)
			//<constant>
			//三者的使用方式是一致的
		case TOKEN::QUOTE_TOK:
		{
			return parseQuoteExp(in, current_env, false);
		}
		case TOKEN::QUASIQUOTE_TOK:
		{
			return parseQuoteExp(in, current_env, true);
		}
		case TOKEN::POUND_TOK:	//'#(<datum>) 与 #(<datum>), (vecotr <datum>)的用法是一致的，定义一个vector
		{
			get_token(in);	//eat the left brace;
			return parseVecotrExp(in, current_env, false);
		}
		default:
			std::cerr << "unknown token  " << current_line_number << std::endl;
			return nullptr;
		}
	}
	else {
		TOKEN _tok = get_token(in);
		switch (_tok)
		{
		case TOKEN::DEFINE_TOK:
		{
			ExpAST* def = parseDefineExp(in, current_env);
			return def;
		}
		case TOKEN::IF_TOK:
			return parseIFElseAST(in, current_env);
		case TOKEN::LAMBDA_TOK:
			return parseLambdaExp(in, current_env);
		case TOKEN::COND_TOK:
			return parseCondExp(in, current_env);
		case TOKEN::LET_TOK:
			return parseLetExp(in, current_env);
		case TOKEN::RBRACE_TOK:
			return nullptr;
		case TOKEN::DOUBLE_TOK:
		case TOKEN::INTEGER_TOK:
			std::cerr << "Uncallable with Number " << current_line_number << std::endl;
			return nullptr;
		default:
			push_token(_tok);
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
	if (_tok == TOKEN::LBRACE_TOK){
		_tok = get_token(in);

		if (_tok != IDENTIFIER_TOK){
			std::cerr << "invalid name at " << current_line_number << std::endl;
			return nullptr;
		}

		DefineProcedureExp *result = DefineProcedureExp::factory();
		result->_name = current_identifer;
		//parse aguments
		parseArgs(in, result->_args);

		//parse procedure expressions;
		while (ExpAST* exp = parseExpAst(in, current_env)) {
			result->exprs.push_back(exp);
		}

		return result;
	}
	else if (_tok == TOKEN::IDENTIFIER_TOK){
		DefineVariableExp* result = DefineVariableExp::factory();
		result->_name = current_identifer;
		result->_expr = parseExpAst(in, current_env);
		_tok = get_token(in);
		if (_tok != TOKEN::RBRACE_TOK){
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
	while (cur_token == TOKEN::IDENTIFIER_TOK) {
		if (cur_token == TOKEN::IDENTIFIER_TOK){
			args.push_back(current_identifer);
		}
		cur_token = get_token(in);
	}

	if (cur_token != TOKEN::RBRACE_TOK){	//eat ')'
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
	if (_tok != LBRACE_TOK){
		std::cerr << "Invalid syntax, expect to be LBRACE \')\', line number " << current_line_number << std::endl;
		return nullptr;
	}

	ProcedureExp* result = ProcedureExp::factory();
	parseArgs(in, result->args);

	while (ExpAST* exp = parseExpAst(in, current_env)) {
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

	IfelseExp* result = IfelseExp::factory();

	result->ifexp = parseExpAst(in, current_env, 0);
	result->thenexp = parseExpAst(in, current_env, 0);
	result->elseexp = parseExpAst(in, current_env, 0);

	TOKEN _tok = get_token(in);
	if (_tok != TOKEN::RBRACE_TOK){
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

	CondExp* result = CondExp::factory();

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
	while (_tok == TOKEN::LBRACE_TOK) {
		exp1 = parseExpAst(in, current_env);
		conds.push_back(exp1);
		rets.push_back(std::vector<ExpAST*>());
		while (exp2 = parseExpAst(in, current_env)) {
			rets[pos].push_back(exp2);
		}

		++pos;
		_tok = get_token(in);
	}


	if (exp1 != nullptr || _tok != TOKEN::RBRACE_TOK){
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

	CallProcedureExp *result = CallProcedureExp::factory();
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

	CallProcedureExp* result = CallProcedureExp::factory();
	ProcedureExp* proc = ProcedureExp::factory();

	parseLetArgs(in, current_env, proc->args, result->parameters);



	while (ExpAST* exp = parseExpAst(in, current_env)) {
		proc->exprs.push_back(exp);
	}
	result->func = proc;

	if (current_tok != TOKEN::RBRACE_TOK){
		std::cerr << "expect to be right brace, invalid let syntax " << current_line_number << std::endl;
		return nullptr;
	}

	return result;
}


void parseLetArgs(std::istream& in, EnvTreeList current_env, std::vector<std::string>& args, std::vector<ExpAST*>& parameters){
	//let syntax:: (let   ( (v1 expr1)  (v2 epxr2)  (v3 expr3) ... (vx exprx) )    expr1 expr2 ... exprn )

	TOKEN _tok = get_token(in);
	if (_tok != LBRACE_TOK){
		std::cerr << "expect to be Left braec '(' " << current_line_number << std::endl;
		return;
	}

	while (1) {
		_tok = get_token(in);
		if (_tok != TOKEN::LBRACE_TOK && _tok != TOKEN::RBRACE_TOK){
			std::cerr << "expect to Left brace or right brace, invalid let syntax " << current_line_number << std::endl;
			return;
		}
		if (_tok == TOKEN::RBRACE_TOK){
			break;
		}

		_tok = get_token(in);

		if (_tok != TOKEN::IDENTIFIER_TOK){
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

		if (_tok != TOKEN::RBRACE_TOK){
			std::cerr << "invalid let syntax, expect to be right brace ')'" << current_line_number << std::endl;
			return;
		}
	}

}

ExpAST* parseQuoteExp(std::istream &in, EnvTreeList env, bool quasiquote = false) {
	TOKEN _tok = get_token(in);
	switch (_tok)
	{
	case TOKEN::BOOL_FALSE_TOK:	//Boolean常量
		return SimpleExp::factory(BoolValue::factory(false));
	case TOKEN::BOOL_TRUE_TOK:	//Boolean常量
		return SimpleExp::factory(BoolValue::factory(true));
	case TOKEN::DOUBLE_TOK:
		return SimpleExp::factory(DoubleValue::factory(current_double));
	case TOKEN::INTEGER_TOK:
		return SimpleExp::factory(IntegerValue::factory(current_int));
	case TOKEN::DO_TOK:
	case TOKEN::IF_TOK:
	case TOKEN::COND_TOK:
	case TOKEN::ELSE_TOK:
	case TOKEN::DEFINE_TOK:
	case TOKEN::EXIT_TOK:
	case TOKEN::LAMBDA_TOK:
	case TOKEN::LET_TOK:
	case TOKEN::IDENTIFIER_TOK:
	{
		return SimpleExp::factory(SymbolValue::factory(current_identifer));
	}
	case TOKEN::STR_TOK:
		return SimpleExp::factory(StringValue::factory(current_identifer));
	case TOKEN::LBRACE_TOK:
	{
		TOKEN cur_tok = get_token(in);
		if (cur_tok == TOKEN::RBRACE_TOK){
			return SimpleExp::factory(NullValue::factory());
		}

		PairExp *ret = PairExp::factory();
		PairExp* prev = ret;
		PairExp* prev_prev = nullptr;
		ExpAST* cur = nullptr;

		bool is_last_comma = false;

		while (1){
			switch (cur_tok)
			{
			case TOKEN::RBRACE_TOK:
			{
				cur = SimpleExp::factory(NullValue::factory());
				break;
			}
			case TOKEN::DOUBLE_TOK:
			{
				cur = SimpleExp::factory(DoubleValue::factory(current_double));
				break;
			}
			case TOKEN::INTEGER_TOK:
			{
				cur = SimpleExp::factory(IntegerValue::factory(current_int));
				break;
			}
			case TOKEN::STR_TOK:
			{
				cur = SimpleExp::factory(StringValue::factory(current_identifer));
				break;
			}
			case TOKEN::BOOL_FALSE_TOK:
			{
				cur = SimpleExp::factory(BoolValue::factory(false));
				break;
			}
			case TOKEN::BOOL_TRUE_TOK:
			{
				cur = SimpleExp::factory(BoolValue::factory(true));
				break;
			}
			case TOKEN::DO_TOK:
			case TOKEN::IF_TOK:
			case TOKEN::COND_TOK:
			case TOKEN::DEFINE_TOK:
			case TOKEN::ELSE_TOK:
			case TOKEN::EXIT_TOK:
			case TOKEN::IDENTIFIER_TOK:
			{
				cur = SimpleExp::factory(SymbolValue::factory(current_identifer));
				break;
			}
			case TOKEN::QUOTE_TOK:
			{
				cur = PairExp::factory();
				((PairExp*)cur)->first = SimpleExp::factory(SymbolValue::factory("'"));
				((PairExp*)cur)->second = parseQuoteExp(in, env, quasiquote);
				break;
			}
			case TOKEN::QUASIQUOTE_TOK:
			{
				cur = PairExp::factory();
				((PairExp*)cur)->first = SimpleExp::factory(SymbolValue::factory("`"));
				((PairExp*)cur)->second = parseQuoteExp(in, env, quasiquote);
				break;
			}
			case TOKEN::POUND_TOK:
			{
				get_token(in);
				cur = parseVecotrExp(in, env, quasiquote);
				break;
			}
			case TOKEN::DOT_TOK:
			{
				is_last_comma = true;
				break;
			}
			case TOKEN::COMMA_TOK:
			{
				if (quasiquote){
					cur = parseExpAst(in, env);
				}
				else{
					cur = SimpleExp::factory(SymbolValue::factory(","));
				}
				break;
			}
			default:
				std::cerr << "Parser: Bad Syntax" << std::endl;
				break;
			}


			if (cur_tok != TOKEN::RBRACE_TOK && cur_tok != TOKEN::DOT_TOK && !is_last_comma){
				prev_prev = prev;
				prev->first = cur;
				prev->second = PairExp::factory();
				prev = (PairExp*)(prev->second);
			}
			else if (cur_tok == TOKEN::RBRACE_TOK && prev_prev != nullptr){
				prev_prev->second = SimpleExp::factory(NullValue::factory());
				break;
			}
			else if (is_last_comma && cur_tok != TOKEN::DO_TOK){
				is_last_comma = false;
				prev_prev->second = cur;
				break;
			}
			else if (cur_tok == TOKEN::EOF_TOK){
				break;
			}
			else if (cur_tok == TOKEN::RBRACE_TOK){
				break;
			}
			else{
				std::cerr << "PraseQuote: error, Bad Syntax" << std::endl;
				break;
			}
			cur_tok = get_token(in);
		}

		return ret;
	}
	default:
	{
		std::cerr << "Parser: Bad syntax" << std::endl;
		break;
	}
	}
	return nullptr;
}


ExpAST* parseVecotrExp(std::istream& in, EnvTreeList current_env, bool is_quasiquote){
	VectorExp* ret = VectorExp::factory();
	ExpAST* cur;
	TOKEN _tok = get_token(in);
	while (1) {
		switch (_tok)
		{
		case TOKEN::BOOL_FALSE_TOK:
		{
			cur = SimpleExp::factory(BoolValue::factory(false));
			break;
		}
		case TOKEN::BOOL_TRUE_TOK:
		{
			cur = SimpleExp::factory(BoolValue::factory(true));
			break;
		}
		case TOKEN::DOUBLE_TOK:
		{
			cur = SimpleExp::factory(DoubleValue::factory(current_double));
			break;
		}
		case TOKEN::INTEGER_TOK:
		{
			cur = SimpleExp::factory(IntegerValue::factory(current_int));
			break;
		}
		case TOKEN::STR_TOK:
		{
			cur = SimpleExp::factory(StringValue::factory(current_identifer));
			break;
		}
		case TOKEN::DO_TOK:
		case TOKEN::IF_TOK:
		case TOKEN::ELSE_TOK:
		case TOKEN::COND_TOK:
		case TOKEN::EXIT_TOK:
		case TOKEN::DEFINE_TOK:
		case TOKEN::LAMBDA_TOK:
		case TOKEN::IDENTIFIER_TOK:
		{
			cur = SimpleExp::factory(SymbolValue::factory(current_identifer));
			break;
		}
		case TOKEN::COMMA_TOK:
		{
			cur = parseExpAst(in, current_env);
			break;
		}
		case TOKEN::QUOTE_TOK:
		{
			cur = parseQuoteExp(in, Env, false);
			break;
		}
		case TOKEN::QUASIQUOTE_TOK:
		{
			cur = parseQuoteExp(in, Env, false);
			break;
		}
		case TOKEN::DOT_TOK:
		{
			std::cerr << "Parse vectorexp, bad syntax" << std::endl;
			break;
		}
		default:
			break;
		}

		ret->vecs.push_back(cur);
	}
	return ret;
}




#endif //_MINISCHEME_PARSER_H