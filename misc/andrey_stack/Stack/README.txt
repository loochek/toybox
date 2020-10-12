/*
Пример подключения:

#define LOG_FILE_NAME "Problems.log"
#include "Stack.h"

Пример использования:
	int main(){
		Stack s = { 0 };
		StackConstruct(&s, 3);
		StackPush(&s, 5);
		StackPop(&s);
	}

Разрешённые для свободного использования функции:

StackErrCode StackConstruct(Stack * stack, size_t capacity)

void StackDestruct(Stack * stack) 

StackErrCode StackPush(Stack * stack, ElemS value)

StackErrCode StackPop(Stack * stack) 

StackErrCode StackTop(Stack * stack, ElemS * top)

size_t StackSize(Stack * stack) 

void StackDump(Stack * stack) 

void AllStacksDestruct()

!!!Если функции(типа StackErrCode) возвращает != STACK_OK, стек не гарантирует корректную работу.
*/
