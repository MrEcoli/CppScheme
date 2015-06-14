#ifndef _MINISCHEME_LEXER_H
#define _MINISCHEME_LEXER_H

#include <iostream>
#include <string>
#include "Object.h"
#include <cctype>
#include "Debug.h"
#define TOKEN_QUEUE_SIZE 50
#define CHAR_QUEUE_SIZE 50

enum TOKEN{

	IDENTIFIER_TOK = 1,
	DOUBLE_TOK,
	INTEGER_TOK,
	BOOL_TRUE_TOK,
	BOOL_FALSE_TOK,

	//�ؼ���
	LAMBDA_TOK,
	IF_TOK,
	DEFINE_TOK,
	COND_TOK,
	ELSE_TOK,
	LET_TOK,
	DO_TOK,			//not implement
	QUOTE_TOK,		//'
	POUND_TOK,		//#
	QUASIQUOTE_TOK,	//`
	COMMA_TOK,		//,
	DOT_TOK,		//.
	


	STR_TOK,

	EXIT_TOK,		//"EXIT", "exit"

	//��������
	LBRACE_TOK,
	RBRACE_TOK,

	//����

	UNKNOWN_TOK,
	EOF_TOK,
};


static double current_double;
static int current_int;
static std::string current_identifer;
static char cur_char;
static int current_line_number = 1;
static TOKEN current_tok;

static bool is_identifier_char(char ch){
	return isalpha(ch) || ch == '_' || ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '<' || ch == '>' || ch == '=' || ch == '!' || ch == '&' || ch == '@' || ch == '^'||ch == '$'||ch == '%'||ch == ':' ||ch == '.';
	
	//scheme r5rs ֧�ֵ����µ��ַ���Ϊ������/������
	//  !$ % & * +-. / : < = > ? @ ^ _
}


//token ջ
TOKEN tok_queue[TOKEN_QUEUE_SIZE];
//char ջ
char char_queue[CHAR_QUEUE_SIZE];

//tokenջ�ĵ�ǰջ��
int token_queue_index = 0;
//charջ��ջ��
int char_queue_index = 0;

//tokenջ��push����
void push_token(TOKEN _tok){
	tok_queue[token_queue_index++] = _tok;
	if (token_queue_index >= TOKEN_QUEUE_SIZE){
		std::cerr << " TOKEN Stack overflow" << std::endl;
	}
}

//������������ȡһ���ַ�
char get_char(std::istream &input){
	if (char_queue_index > 0){
		return char_queue[--char_queue_index];
	}
		return input.get();
}

//��һ���ַ���ջ
void push_char(char ch){
	char_queue[char_queue_index++] = ch;
	if (char_queue_index >= CHAR_QUEUE_SIZE){
		std::cerr << "Character Stack overflow" << std::endl;
	}
}


//���һ��TOKEN
//ʹ��ǰ����ʹ��cur_char = input.get(),��õ�ǰ����Ҫ�����character
static TOKEN get_token(std::istream &input){
	//���TOKENջ��Ϊ�գ���ֱ����ȡ
	if (token_queue_index > 0){
		return current_tok = tok_queue[--token_queue_index];
	}

	while (1){
		//�ع�����switch������ifelse
		switch (cur_char)
		{
		case EOF:
			std::cout << "EOF" << std::endl;
			return current_tok = TOKEN::EOF_TOK;
		//����հ��ַ�
		case ' ':
		case '\t':
		case '\r':
			cur_char = get_char (input);	
			break;
		//������'\n'ʱ���кż�1
		case '\n':
			//DEBUG_STDOUT_TOKEN(cur_char, "\"\\n\"");
			++current_line_number;
			cur_char = get_char (input);
			break;
		case '(':
			DEBUG_STDOUT_TOKEN("", "\"(\", TOKEN::LBRACE");
			cur_char = get_char (input);
			return current_tok = TOKEN::LBRACE_TOK;
		case ')':
			DEBUG_STDOUT_TOKEN("", "\")\", TOKEN::RBRACE");
			cur_char = get_char (input);
			return current_tok = TOKEN::RBRACE_TOK;
		case ';':		//comment begin, ����ע��
			DEBUG_STDOUT_TOKEN("", "\";\", Comment");
			while (cur_char != '\n' && cur_char != EOF) {
				cur_char = get_char (input);
			}
			break;
			//������default������isdigit����
		//�����
		case ',':
			DEBUG_STDOUT_TOKEN(",", "TOKEN::COMMA");
			cur_char = get_char(input);
			if (isspace(cur_char)){
				current_identifer = std::string(1, ',');
				return TOKEN::IDENTIFIER_TOK;
			}
			else{
				return TOKEN::COMMA_TOK;
			}

		case '`':
			DEBUG_STDOUT_TOKEN( "`", "TOKEN::BACKQUOTE");
			cur_char = get_char(input);
			return TOKEN::QUASIQUOTE_TOK;
		case '#':
		{
			
			cur_char = get_char(input);
			char nxt_char = get_char(input);

			if (cur_char == 't' || isspace(nxt_char)){
				DEBUG_STDOUT_TOKEN("#t", "TOKEN::POUND");
				cur_char = get_char(input);
				return TOKEN::BOOL_TRUE_TOK;
			}
			else if (cur_char == 'f' || isspace(nxt_char)){
				DEBUG_STDOUT_TOKEN("#f", "TOKEN::POUND");
				cur_char = get_char(input);
				return TOKEN::BOOL_FALSE_TOK;
			}
			else if (cur_char == '('){
				DEBUG_STDOUT_TOKEN("#", "TOKEN::POUND");
				return TOKEN::POUND_TOK;
			}
			else{
				std::cerr << "Lexer : bad syntax #" << std::endl;
				return TOKEN::UNKNOWN_TOK;
			}

		}
		case '\'':
			DEBUG_STDOUT_TOKEN( "'", "TOKEN::QUOTE");
			cur_char = get_char(input);
			return TOKEN::QUOTE_TOK;
		case '"':	//�ַ����Ĵ���ʽͬRacket..
		{
			std::string tmp;
			cur_char = get_char(input);
			while (cur_char != '"' && cur_char != EOF) {
				tmp += cur_char;
				if (cur_char == '\\'){
					cur_char = get_char(input);
					tmp += cur_char;
				}
				cur_char = get_char(input);
			}

			if (cur_char == '"'){
				cur_char = get_char(input);
				current_identifer = tmp;
				return TOKEN::STR_TOK;
			}
			else{
				return TOKEN::EOF_TOK;
			}
		}
		case '.':
			cur_char = get_char(input);
			if (isspace(cur_char)){
				DEBUG_STDOUT_TOKEN( ".", "TOKEN::DOT")
				return TOKEN::DOT_TOK;
			}
			else{
				push_char(cur_char);
				cur_char = '.';
			}
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '8':
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
				cur_char = get_char (input);
			}

			if (isNumber && dot_number == 0){
				DEBUG_STDOUT_TOKEN(tmp, "TOKEN::INTEGER");
				current_int = stoi(tmp);
				return current_tok = TOKEN::INTEGER_TOK;
			}
			else if (isNumber && dot_number == 1){
				DEBUG_STDOUT_TOKEN(tmp, "TOKEN::DOUBLE");
				current_double = stod(tmp);
				return current_tok = TOKEN::DOUBLE_TOK;
			}
			else{
				DEBUG_STDOUT_TOKEN(tmp, "TOKEN::IDENTIFIER");
				current_identifer = tmp;
				return current_tok = TOKEN::IDENTIFIER_TOK;
			}
		}
		default:
			//identifier����
			if (is_identifier_char(cur_char) || cur_char == '_'){
				std::string tmp(1, cur_char);
				cur_char = get_char (input);
				while (!isspace(cur_char) && cur_char != '(' && cur_char != ')' && cur_char != ';' && cur_char != EOF) {					tmp += cur_char;					cur_char = get_char (input);
				}
				current_identifer = tmp;

				if (tmp == "define"){
					DEBUG_STDOUT_TOKEN(tmp, "TOKEN::DEFINE");
					return current_tok = TOKEN::DEFINE_TOK;
				}
				else if (tmp == "if"){
					DEBUG_STDOUT_TOKEN(tmp, "TOKEN::IF");
					return current_tok = TOKEN::IF_TOK;
				}
				else if (tmp == "cond"){
					DEBUG_STDOUT_TOKEN(tmp, "TOKEN::COND");
					return current_tok = TOKEN::COND_TOK;
				}
				else if (tmp == "else"){
					DEBUG_STDOUT_TOKEN(tmp, "TOKEN::ELSE");
					return current_tok = TOKEN::ELSE_TOK;
				}
				else if (tmp == "lambda"){
					DEBUG_STDOUT_TOKEN(tmp, "TOKEN::LAMBDA");
					return current_tok = TOKEN::LAMBDA_TOK;
				}
				else if (tmp == "exit" || tmp == "EXIT"){
					DEBUG_STDOUT_TOKEN(tmp, "TOKEN::EXIT");
					return current_tok = TOKEN::EXIT_TOK;
				}
				DEBUG_STDOUT_TOKEN(tmp, "TOKEN::IDENTIFER");
				return current_tok = TOKEN::IDENTIFIER_TOK;
			}
			else
				DEBUG_STDOUT_TOKEN(cur_char, "TOKEN::UNKNOWN");
			cur_char = get_char (input);
			return current_tok = TOKEN::UNKNOWN_TOK;
		}

	}
}

#endif //_MINISCHEME_LEXER_H
