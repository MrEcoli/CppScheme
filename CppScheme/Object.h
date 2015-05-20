#ifndef _MINISCHEME_OBJECT_H
#define _MINISCHEME_OBJECT_H

#include <string>
#include <vector>

namespace CppScheme{

	class ExpAST;
	enum ObjectType{
		PROCEDURE = 1,
		DOUBLEOBJ,
		BOOLOBJ,
		PAIROBJ,
	};

	class Object{
	public:
		ObjectType obtype;
		Object(ObjectType _Ty) :obtype(_Ty){}
		virtual ObjectType get_type(){ return obtype; }
	};

	class DoubleValue :public Object{
	public:
		double value;
		DoubleValue() :Object(ObjectType::DOUBLEOBJ){}
		DoubleValue(double _V) :value(_V), Object(ObjectType::DOUBLEOBJ){}
	};


	//数据变量
	//为了简化程序，所有的数值以double类型表示
	class Variable :public Object{
		std::string name;
		Variable(std::string s = std::string("")) :name(s),  Object(DOUBLEOBJ){}
	};


	//一个过程，如果不为匿名过程，一个过程应该有其参数列表，并且一个表达式，ExpAST的codgen的调用能够使用
	class Procedure :public Object{
	public:
		std::vector < std::string > args;
		ExpAST* expr;
		Procedure() :Object(ObjectType::PROCEDURE){}
	};
	


	class BoolValue :public Object{
	public:
		bool value;
		BoolValue() :Object(ObjectType::BOOLOBJ){}
		BoolValue(bool _bb) :value(_bb), Object(ObjectType::BOOLOBJ){}
	};


	class Pair :public Object{
	public:
		Object *first;
		Object *next;
		Pair() :Object(ObjectType::PAIROBJ), first(nullptr), next(nullptr){}
		Pair(Object* _x, Object* _y) :first(_x), next(_y), Object(ObjectType::PAIROBJ){}
	};
}


#endif
