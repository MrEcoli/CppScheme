#ifndef _MINISCHEME_OBJECT_H
#define _MINISCHEME_OBJECT_H

#include <string>
#include <vector>

namespace MiniScheme{

	class ExpAST;
	enum ObjectType{
		PROCEDURE = 1,
		INTEGEROBJ,
		DOUBLEOBJ,
		BOOLOBJ,
	};

	class Object{
	public:
		ObjectType obtype;
		Object(ObjectType _Ty) :obtype(_Ty){}
		virtual ~Object() = 0;
	};

	class Number :public Object{
	public:
		Number(ObjectType _Ty) :Object(_Ty){}
	};

	class DoubleValue :public Number{
	public:
		double value;
		DoubleValue() :Number(DOUBLEOBJ){}
		DoubleValue(double _V) :value(_V), Number(DOUBLEOBJ){}
	};

	class IntegerValue :public Number{
	public:
		int value;
		IntegerValue() :Number(INTEGEROBJ){}
		IntegerValue(int _V) :value(_V), Number(INTEGEROBJ){}
	};

	//���ݱ���
	//Ϊ�˼򻯳������е���ֵ��double���ͱ�ʾ
	class Variable :public Object{
		std::string name;
		double value;
		Variable(double val, std::string s = std::string("")) :name(s), value(val), Object(DOUBLEOBJ){}
	};


	//һ�����̣������Ϊ�������̣�һ������Ӧ����������б�����һ�����ʽ��ExpAST��codgen�ĵ����ܹ�ʹ��
	class Procedure :public Object{
	public:
		std::vector < std::string > args;
		ExpAST* expr;
		Procedure() :Object(ObjectType::PROCEDURE){}
	};

	class Boolean :public Object{
		bool vale;
	};
}


#endif
