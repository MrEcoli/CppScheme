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

bool parseCondExp(std::istream& in, EnvTreeList current_env, std::vector<ExpAST*> &conds, std::vector<ExpAST*> &rets);

ExpAST* parseCallExp(std::istream& in, EnvTreeList current_env);

ExpAST* parseDefineExp(std::istream& in, EnvTreeList current_env);

ExpAST* parseLambdaExp(std::istream& in, EnvTreeList current_env);

ExpAST* parseLetExp(std::istream& in, EnvTreeList current_env);

void parseLetArgs(std::istream& in, EnvTreeList current_env, std::vector<std::string>& args, std::vector<ExpAST*>& parameters);






//解析语法
ExpAST* parseExpAst(std::istream& in, EnvTreeList current_env, int left = 0){
	if (left == 0){
		TOKEN _tok = get_token(in);
		switch (_tok)
		{
		case TOKEN::DOUBLE:
			return new SimpleExp(new DoubleValue(current_double));
		case TOKEN::IDENTIFIER:
			return new VariableExp(current_identifer);
		case TOKEN::LBRACE:
			return parseExpAst(in, current_env, 1);
		case TOKEN::RBRACE:
			return nullptr;
		case TOKEN::ELSE:
			return nullptr;
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
			return parseDefineExp(in, current_env);
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
		case TOKEN::DOUBLE:
			std::cerr << "Uncallable with Number " << current_line_number << std::endl;
			return nullptr;
		default:
			push(_tok);
			return parseCallExp(in, current_env);
		}
	}
}


/*
void parseBegin(std::istream& in){
while (1){
TOKEN cur_token = get_token(in);
//使用switch语句效率更高，需进行重构
if (cur_token == TOKEN::IDENTIFIER){
auto iter = GlobalVariable->find(current_identifer);
if (iter == GlobalVariable->end()){
std::cerr << "Couldn't find the identifier \"" << current_identifer << "\"" << std::endl;
}
else{
if (iter->second->obtype == ObjectType::PROCEDURE && dynamic_cast<Procedure*>(iter->second)){
std::cout << "<procedure> " << current_identifer << std::endl;
}
else if (iter->second->obtype == ObjectType::DOUBLEOBJ){
if (DoubleValue* ptr = dynamic_cast<DoubleValue*>(iter->second)){
std::cout << ptr->value << std::endl;
}
}
else if (iter->second->obtype == ObjectType::INTEGEROBJ){
if (IntegerValue* ptr = dynamic_cast<IntegerValue*>(iter->second)){
std::cout << ptr->value << std::endl;
}
}
}
}
else if (cur_token == TOKEN::INTEGER){
std::cout << current_int << std::endl;
}
else if (cur_token == TOKEN::DOUBLE){
std::cout << current_double << std::endl;
}
else if (cur_token == TOKEN::LBRACE){
int left = 1;
cur_token = get_token(in);
if (cur_token == TOKEN::DEFINE){
EnvTree* current_env = GlobalVariable;

}


}
else if (cur_token == TOKEN::TEOF){
break;
}
else{

}
}
}
*/





ExpAST* parseDefineExp(std::istream& in, EnvTreeList current_env){
	//		(define   (func args...) (expr ... ... ) )
	//                Λ                              Λ
	//                |                              |
	//           function start token;              eat the last brace;

	std::string _name;
	TOKEN _tok = get_token(in);
	if (_tok == TOKEN::LBRACE){
		_tok = get_token(in);
		if (_tok != IDENTIFIER){
			std::cerr << "invalid name at " << current_line_number << std::endl;
			return nullptr;
		}
		DefineExp* result = new DefineExp();
		result->name = current_identifer;
		ProcedureExp* proc = new ProcedureExp();
		parseArgs(in, proc->args);

		proc->expr = parseExpAst(in, current_env);

		result->expr = proc;

		return result;
	}
	else if (_tok == TOKEN::IDENTIFIER){
		DefineExp *result = new DefineExp();
		result->name = current_identifer;
		result->expr = parseExpAst(in, current_env);
		_tok = get_token(in);
		if (_tok != TOKEN::RBRACE){
			std::cout << "Expected to be right brace ')'" << std::endl;
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
	//    ( arg1 arg2 arg3 ... ...)
	//      Λ                     Λ
	//      |                     |
	//  function start;          eat the last brace;

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
	TOKEN _tok = get_token(in);
	if (_tok != LBRACE){
		std::cerr << "Invalid syntax, expect to be LBRACE \')\', line number " << current_line_number << std::endl;
		return nullptr;
	}

	ProcedureExp* result = new ProcedureExp();
	parseArgs(in, result->args);
	result->expr = parseExpAst(in, current_env);

	_tok = get_token(in);
	//eat last right brace:: (lambda  (args) (expr ..) ) 
	//								Λ			      Λ
	//								|			      |	
	//		this function start from here;       eat this brace

	if (_tok != RBRACE){
		std::cerr << "Invalid syntax, expect to be RBRACE \')\', line number " << current_line_number << std::endl;
		return nullptr;
	}

	return result;
}





ExpAST* parseIFElseAST(std::istream& in, EnvTreeList current_env){

	IfelseExp* result = new IfelseExp();

	result->ifexp = parseExpAst(in, current_env, 0);
	result->thenexp = parseExpAst(in, current_env, 0);
	result->elseexp = parseExpAst(in, current_env, 0);
	return result;
}

ExpAST* parseCondExp(std::istream& in, EnvTreeList current_env){
	// (cond   (cond1, ret1) (cond1, ret 1) ... ( else , retx) )
	//         Λ     Λ                                         Λ
	//         |     |                                         |
	//  func start   get the conditions and returns           eat the last lbrace;

	CondExp* result = new CondExp();
	parseCondExp(in, current_env, result->conds, result->rets);
	TOKEN _tok = get_token(in);	//eat last ')'
	if (_tok != TOKEN::RBRACE){
		std::cerr << "syntax error, expect ')'" << std::endl;
		return nullptr;
	}
	return result;
}


//get the condition and return expAST of cond expression
bool parseCondExp(std::istream& in, EnvTreeList current_env, std::vector<ExpAST*> &conds, std::vector<ExpAST*> &rets){
	TOKEN _tok = get_token(in);
	if (_tok != TOKEN::LBRACE){
		std::cerr << "syntax error, expect '('" << std::endl;
		return false;
	}
	ExpAST* exp1, *exp2;
	while (exp1 = parseExpAst(in, current_env)) {
		exp2 = parseExpAst(in, current_env);
		conds.push_back(exp1);
		rets.push_back(exp2);
		_tok = get_token(in);
		if (_tok != TOKEN::RBRACE){
			std::cerr << "syntax error, expect ')'" << std::endl;
			return false;
		}
		_tok = get_token(in);
		if (_tok != TOKEN::LBRACE){
			std::cerr << "syntax error, expect '('" << std::endl;
			return false;
		}
	}

	conds.push_back(exp1);
	rets.push_back(parseExpAst(in, current_env));
	_tok = get_token(in);	//eat ')'
	if (_tok != TOKEN::RBRACE){
		std::cerr << "syntax error, expect ')'" << std::endl;
		return false;
	}
	return true;
}



//左边已有一个左括号'(';
ExpAST* parseCallExp(std::istream& in, EnvTreeList current_env){
	// CallExpression ->    (expr_of_procedure  arg1  arg2  arg3  ...  )
	//                       Λ                                         Λ
	//                       |                                         |
	//                   function start;                     no need to eat explict;


	CallProcedureExp * result = new CallProcedureExp();

	result->func = parseExpAst(in, current_env);

	if (result->func == nullptr){
		std::cerr << "invalid function call " << current_line_number << std::endl;
		return nullptr;
	}

	ExpAST * cur;

	while (cur = parseExpAst(in, current_env)) {
		result->parameters.push_back(cur);
	}

	return result;
}



//let语句其实就是一个调用lambda语句的语法糖，变量初始化阶段等同于给予lambda函数
//可以直接解析为CallProcedureExp
ExpAST* parseLetExp(std::istream& in, EnvTreeList current_env){

	//let syntax:: (let   ( (v1 expr1)  (v2 epxr2)  (v3 expr3) ... (vx exprx) )    (function body) )
	//               Λ    Λ                                                   Λ                    Λ
	//               |    |                                                   |                    |
	//    function start    aguments parse start                    aguments parse end           eat the last brace;

	CallProcedureExp* result = new CallProcedureExp();
	ProcedureExp* proc = new ProcedureExp();

	parseLetArgs(in, current_env, proc->args, result->parameters);

	proc->expr = parseExpAst(in, current_env);
	result->func = proc;

	TOKEN _tok = get_token(in);
	if (_tok != TOKEN::RBRACE){
		std::cerr << "expect to be right brace, invalid let syntax " << current_line_number << std::endl;
		return nullptr;
	}

	return result;
}


void parseLetArgs(std::istream& in, EnvTreeList current_env, std::vector<std::string>& args, std::vector<ExpAST*>& parameters){

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