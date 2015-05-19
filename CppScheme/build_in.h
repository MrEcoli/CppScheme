#ifndef _MINISCHEME_BUILD_IN_H
#define _MINISCHEME_BUILD_IN_H

#include "Object.h"
#include <iostream>


namespace MiniScheme{

	class BuiltIn{
		virtual ~BuiltIn() = 0;
		virtual Object* operator()(std::vector<Object*> args) = 0;
	};


	class Cons :public BuiltIn{
	public:
		Object* operator()(std::vector<Object*> args){
			if (args.size() != 2){
				std::cout << "cons operator error, illegal arguments number, expect to be 2, give " << args.size() << std::endl;
				return nullptr;
			}
			return new Pair(args[0], args[1]);
		}
	};

	class Car :public BuiltIn{
	public:
		Object* operator()(std::vector<Object*> args){
			if (args.size() != 1){
				std::cout << "car operator error, illegal arguments number, expect to be 1, give " << args.size() << std::endl;
				return nullptr;
			}

			if (Pair* ptr = dynamic_cast<Pair*> (args[0])){
				return ptr->cur;
			}
			else{
				std::cout << "illegal operator, not a Pair" << std::endl;
				return nullptr;
			}
		}
	};

	class Cdr :public BuiltIn{
	public:
		Object* operator()(std::vector<Object*> args){
			if (args.size() != 1){
				std::cout << "cdr operator error, illegal arguments number, expect to be 1, give " << args.size() << std::endl;
				return nullptr;
			}

			if (Pair* ptr = dynamic_cast<Pair*> (args[0])){
				return ptr->next;
			}
			else{
				std::cout << "illegal operator, not a Pair" << std::endl;
				return nullptr;
			}
		}
	};

	class Add :public BuiltIn{
	public:
		Object* operator()(std::vector<Object*> args){
			if (args.size() != 2){
				std::cout << "add operator error, needs more than one operator, expression give " << args.size() << std::endl;
				return nullptr;
			}


			if (DoubleValue* ptr1 = dynamic_cast<DoubleValue*>(args[0])){
				if (DoubleValue* ptr2 = dynamic_cast<DoubleValue*>(args[1])){
					return new DoubleValue(ptr1->value + ptr2->value);
				}
				else if (IntegerValue* ptr2 = dynamic_cast<IntegerValue*>(args[1])){
					return new DoubleValue(ptr1->value + ptr2->value);
				}
			}
			else if (IntegerValue* ptr1 = dynamic_cast<IntegerValue*>(args[0])){
				if (IntegerValue* ptr2 = dynamic_cast<IntegerValue*>(args[1])){
					return new IntegerValue(ptr1->value + ptr2->value);
				}
				else if (DoubleValue *ptr2 = dynamic_cast<DoubleValue*>(args[1])){
					return new DoubleValue(ptr1->value + ptr2->value);
				}
			}

			std::cerr << "Invalid arguments" << std::endl;
			return nullptr;
		}
	};

	class Sub :public BuiltIn{
	public:
		Object* operator()(std::vector<Object*> args){
			if (args.size() != 2){
				std::cout << "sub operator error, needs more than one operator, expression give " << args.size() << std::endl;
				return nullptr;
			}


			if (DoubleValue* ptr1 = dynamic_cast<DoubleValue*>(args[0])){
				if (DoubleValue* ptr2 = dynamic_cast<DoubleValue*>(args[1])){
					return new DoubleValue(ptr1->value - ptr2->value);
				}
				else if (IntegerValue* ptr2 = dynamic_cast<IntegerValue*>(args[1])){
					return new DoubleValue(ptr1->value - ptr2->value);
				}
			}
			else if (IntegerValue* ptr1 = dynamic_cast<IntegerValue*>(args[0])){
				if (IntegerValue* ptr2 = dynamic_cast<IntegerValue*>(args[1])){
					return new IntegerValue(ptr1->value - ptr2->value);
				}
				else if (DoubleValue *ptr2 = dynamic_cast<DoubleValue*>(args[1])){
					return new DoubleValue(ptr1->value - ptr2->value);
				}
			}

			std::cerr << "Invalid arguments" << std::endl;
			return nullptr;
		}
	};

	class Mul :public BuiltIn{
	public:
		Object* operator()(std::vector<Object*> args){
			if (args.size() != 2){
				std::cout << "mul operator error, needs more than one operator, expression give " << args.size() << std::endl;
				return nullptr;
			}


			if (DoubleValue* ptr1 = dynamic_cast<DoubleValue*>(args[0])){
				if (DoubleValue* ptr2 = dynamic_cast<DoubleValue*>(args[1])){
					return new DoubleValue(ptr1->value * ptr2->value);
				}
				else if (IntegerValue* ptr2 = dynamic_cast<IntegerValue*>(args[1])){
					return new DoubleValue(ptr1->value * ptr2->value);
				}
			}
			else if (IntegerValue* ptr1 = dynamic_cast<IntegerValue*>(args[0])){
				if (IntegerValue* ptr2 = dynamic_cast<IntegerValue*>(args[1])){
					return new IntegerValue(ptr1->value * ptr2->value);
				}
				else if (DoubleValue *ptr2 = dynamic_cast<DoubleValue*>(args[1])){
					return new DoubleValue(ptr1->value * ptr2->value);
				}
			}

			std::cerr << "Invalid arguments" << std::endl;
			return nullptr;
		}
	};


	class Div :public BuiltIn{
	public:
		Object* operator()(std::vector<Object*> args){
			if (args.size() != 2){
				std::cout << "Div operator error, needs more than one operator, expression give " << args.size() << std::endl;
				return nullptr;
			}


			if (DoubleValue* ptr1 = dynamic_cast<DoubleValue*>(args[0])){
				if (DoubleValue* ptr2 = dynamic_cast<DoubleValue*>(args[1])){
					return new DoubleValue(ptr1->value / ptr2->value);
				}
				else if (IntegerValue* ptr2 = dynamic_cast<IntegerValue*>(args[1])){
					return new DoubleValue(ptr1->value / ptr2->value);
				}
			}
			else if (IntegerValue* ptr1 = dynamic_cast<IntegerValue*>(args[0])){
				if (IntegerValue* ptr2 = dynamic_cast<IntegerValue*>(args[1])){
					return new IntegerValue(ptr1->value / ptr2->value);
				}
				else if (DoubleValue *ptr2 = dynamic_cast<DoubleValue*>(args[1])){
					return new DoubleValue(ptr1->value / ptr2->value);
				}
			}

			std::cerr << "Invalid arguments" << std::endl;
			return nullptr;
		}
	};



}

#endif