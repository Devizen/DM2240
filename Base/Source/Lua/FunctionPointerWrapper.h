#ifndef FUNCTIONPOINTERWRAPPER_H
#define FUNCTIONPOINTERWRAPPER_H

/*
Done by Lai Wei Jing, Eugene and Ow Hong Yu.
2nd February 2018.
*/
#include <functional>
#include <string>
#include <vector>

/*
Interface class for Function Pointer polymorphism.
Compile will be able to compile at the compile time because it is a concrete class.
*/
class TFunction
{
public:
	virtual ~TFunction() {}
	virtual void Run() {}
	/*For polymorphism after casting.*/
	virtual void Run(std::vector<std::string>&, std::vector<std::string>&) {}
};

template<typename ReturnType>
class FunctionPointerWrapper : public TFunction
{
public:
	/*Allow initialisation of any type, takes in a function pointer of any return type with no parameter as default constructor parameter.*/
	FunctionPointerWrapper<ReturnType>(std::function<ReturnType(void)> _functionPointer) : functionPointer(_functionPointer) {}
	~FunctionPointerWrapper() {}

	virtual void Run() { functionPointer(); }
private:
	std::function<ReturnType()>functionPointer;
};

template<typename ReturnType, typename ... Parameters>
class FunctionPointerWrapper<ReturnType(Parameters ...)> : public TFunction
{
public:
	/*Allow initialisation of any type, takes in a function pointer of any return type with any amount of parameters of any type as default constructor parameter.*/
	/*-------------------Constructor------------------|--------------Parameters Type------------|---Parameters---|------------Assign---------------|*/
	FunctionPointerWrapper<ReturnType(Parameters ...)>(std::function<ReturnType(Parameters ...)> _functionPointer) : functionPointer(_functionPointer) {}
	~FunctionPointerWrapper() {}

	virtual void Run(Parameters ... _parameters) { functionPointer(_parameters ...); }
private:
	std::function<ReturnType(Parameters ...)>functionPointer;
};

#endif