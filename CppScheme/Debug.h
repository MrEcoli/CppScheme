#ifndef _CPP_SCHEME_DEBUG_H
#define _CPP_SCHEME_DEBUG_H
#include <windows.h>
#include <iostream>

#define _DEBUG_HEAP_POS
#ifdef _DEBUG_HEAP_POS
#define DEBUG_PTR(x) std::cout<<(x)<<std::endl;
#else
#define DEBUG_PTR(x)
#endif	//_DEBUG_HEAP_POS


#ifdef _DEBUG_DESTRUCTOR
#define _DEBUG_DESTRUCTOR(x) std::cout<<(x)<<std::endl;
#else
#define _DEBUG_DESTRUCTOR(x)
#endif //_DEBUG_DESTRUCTOR

//#define  DEBUG_TOKEN
#ifdef DEBUG_TOKEN
#include <iostream>
#define DEBUG_STDERR_TOKEN_TOKEN(x, y) (std::cerr <<"current TOKEN is " << (x) << " "<< y<< "\n");
#define DEBUG_STDOUT_TOKEN(x, y) (std::cout <<"current TOKEN is " << (x) << " "<< y<< "\n");
#include <type_traits>
//... etc
#else 
#define DEBUG_STDERR_TOKEN_TOKEN(x, y)
#define DEBUG_STDOUT_TOKEN(x, y)
//... etc
#endif

class Fout{
public:
	//https://msdn.microsoft.com/en-us/library/windows/desktop/ms682073(v=vs.85).aspx
	//restore the cosole info
	static HANDLE console_handle;
	static CONSOLE_SCREEN_BUFFER_INFO console_info;
public:
	//init the "console_handle";
	static void init_Console(){
		SetConsoleCP(437);
		SetConsoleTitle("CppScheme");
		GetConsoleScreenBufferInfo(console_handle, &console_info);
	}

	template<typename... Args>
	static std::ostream& error_out(Args&&... _args){
		return format_print(std::cerr, FOREGROUND_RED | FOREGROUND_INTENSITY, std::forward <Args>(_args)...);
	}

	template<typename... Args>
	static std::ostream& normal_out(Args&&... _args){
		return format_print(std::cout, FOREGROUND_GREEN|FOREGROUND_RED|FOREGROUND_BLUE| FOREGROUND_INTENSITY, std::forward <Args>(_args)...);
	}

	template<typename... Args>
	static std::ostream& base_out(Args&&... _args){
		return format_print(std::cout, FOREGROUND_GREEN|FOREGROUND_INTENSITY, std::forward <Args>(_args)...);
	}

	

	template<typename... Args>
	static std::ostream& begin_out(Args&&... _args){
		return format_print(std::cout, FOREGROUND_GREEN|FOREGROUND_RED| FOREGROUND_INTENSITY, std::forward <Args>(_args)...);
	}

private:
	template<typename... Args>
	static std::ostream& format_print(std::ostream& os, WORD _wAttribute, Args&&... _args){
		SetConsoleTextAttribute(console_handle, _wAttribute);
		print(os, std::forward<Args>(_args)...);
		SetConsoleTextAttribute(console_handle, console_info.wAttributes);
		return os;
	}

	template<typename T>
	static std::ostream& print(std::ostream& os, const T& _x){
		return os << _x;
	}

	template<typename T, typename... Args>
	static std::ostream& print(std::ostream& os, const T& _x, Args&&... _others){
		os << _x << " ";
		return print(os, std::forward<Args>(_others)...);
	}

};

HANDLE Fout::console_handle = GetStdHandle(STD_OUTPUT_HANDLE);
CONSOLE_SCREEN_BUFFER_INFO Fout::console_info;














#endif