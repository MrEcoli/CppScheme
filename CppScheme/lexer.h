#ifndef _MINISCHEME_LEXER_H
#define _MINISCHEME_LEXER_H

#include <iostream>
#include <string>
#include "Object.h"
#include <cctype>
#include "Debug.h"

enum TOKEN{

	IDENTIFIER = 1,
	NUMBER,
	INTEGER,


	//关键字
	LAMBDA,
	IF,
	DEFINE,
	COND,
	ELSE,
	LET,

	EXIT,

	//左右括号
	LBRACE,
	RBRACE,

	//换行

	UNKNOWN,
	TEOF,
};


static double current_double;
static int current_int;
static std::string current_identifer;
static char cur_char;
static int current_line_number = 1;
static TOKEN current_tok;

static bool is_identifier_char(char ch){
	return isalpha(ch) || ch == '_' || ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '<' || ch == '>' || ch == '=' || ch == '!' || ch == '&' || ch == '*' || ch == '^';
}

bool push_token = false;
TOKEN token_pushed;

void push(TOKEN _tok){
	if (push_token == false){
		push_token = true;
		token_pushed = _tok;
	}
}

static TOKEN get_token(std::istream &input){
	if (push_token){
		push_token = false;
		return current_tok = token_pushed;
	}

	while (1){
		//重构，以switch语句替代ifelse
		switch (cur_char)
		{
		case EOF:
			std::cout << "EOF" << std::endl;
			return current_tok = TOKEN::TEOF;
		case ' ':
		case '\t':
		case '\r':
			cur_char = input.get();
			break;
		case '\n':
			//DEBUG_STDOUT_TOKEN(cur_char, "\"\\n\"");
			++current_line_number;
			cur_char = input.get();
			break;
		case '(':
			DEBUG_STDOUT_TOKEN("", "\"(\", TOKEN::LBRACE");
			cur_char = input.get();
			return current_tok = TOKEN::LBRACE;
		case ')':
			DEBUG_STDOUT_TOKEN("", "\")\", TOKEN::RBRACE");
			cur_char = input.get();
			return current_tok = TOKEN::RBRACE;
		case ';':		//comment begin;
			DEBUG_STDOUT_TOKEN("", "\";\", Comment");
			while (cur_char != '\n' && cur_char != EOF) {
				cur_char = input.get();
			}
			break;
			//或者在default里利用isdigit处理
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '9':
		{
			std::string tmp;
			int dot_number = 0;
			bool isNumber = true;
			while (!isspace(cur_char) && cur_char != '(' && cur_char != ')' && cur_char != ';' && cur_char != '#' && cur_char != EOF) {
				if (cur_char == '.'){
					++dot_number;
				}

				if (!isdigit(cur_char) && cur_char != '.'){
					isNumber = false;
				}
				tmp += cur_char;
				cur_char = input.get();
			}

			if (isNumber){
				DEBUG_STDOUT_TOKEN(tmp, "TOKEN::DOUBLE");
				current_double = stod(tmp);
				return current_tok = TOKEN::NUMBER;
			}
			else{
				DEBUG_STDOUT_TOKEN(tmp, "TOKEN::IDENTIFIER");
				current_identifer = tmp;
				return current_tok = TOKEN::IDENTIFIER;
			}
		}
		default:
			//identifier类型
			if (is_identifier_char(cur_char) || cur_char == '_'){
				std::string tmp(1, cur_char);
				cur_char = input.get();
				while (!isspace(cur_char) && cur_char != '(' && cur_char != ')' && cur_char != ';' && cur_char != EOF) {					tmp += cur_char;					cur_char = input.get();
				}
				current_identifer = tmp;

				if (tmp == "define"){
					DEBUG_STDOUT_TOKEN(tmp, "TOKEN::DEFINE");
					return current_tok = TOKEN::DEFINE;
				}
				else if (tmp == "if"){
					DEBUG_STDOUT_TOKEN(tmp, "TOKEN::IF");
					return current_tok = TOKEN::IF;
				}
				else if (tmp == "cond"){
					DEBUG_STDOUT_TOKEN(tmp, "TOKEN::COND");
					return current_tok = TOKEN::COND;
				}
				else if (tmp == "else"){
					DEBUG_STDOUT_TOKEN(tmp, "TOKEN::ELSE");
					return current_tok = TOKEN::ELSE;
				}
				else if (tmp == "lambda"){
					DEBUG_STDOUT_TOKEN(tmp, "TOKEN::LAMBDA");
					return current_tok = TOKEN::LAMBDA;
				}
				else if (tmp == "exit" || tmp == "EXIT"){
					DEBUG_STDOUT_TOKEN(tmp, "TOKEN::EXIT");
					return current_tok = TOKEN::EXIT;
				}
				DEBUG_STDOUT_TOKEN(tmp, "TOKEN::IDENTIFER");
				return current_tok = TOKEN::IDENTIFIER;
			}
			else
				DEBUG_STDOUT_TOKEN(cur_char, "TOKEN::UNKNOWN");
			cur_char = input.get();
			return current_tok = TOKEN::UNKNOWN;
		}

	}
}

#endif //_MINISCHEME_LEXER_H
