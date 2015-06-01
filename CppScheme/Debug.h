#ifndef _CPP_SCHEME_DEBUG_H
#define _CPP_SCHEME_DEBUG_H

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
//... etc
#else 
#define DEBUG_STDERR_TOKEN_TOKEN(x, y)
#define DEBUG_STDOUT_TOKEN(x, y)
//... etc
#endif


#endif