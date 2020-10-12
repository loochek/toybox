#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#ifdef WIN32

#include <windows.h>

bool is_pointer_valid (const void* address)
{
	MEMORY_BASIC_INFORMATION mbi = {};
	if (!VirtualQuery (address, &mbi, sizeof (mbi))) return true;

	if (mbi.Protect & (PAGE_GUARD | PAGE_NOACCESS))  return true;  // Guard page -> bad ptr

	DWORD readRights = PAGE_READONLY | PAGE_READWRITE | PAGE_WRITECOPY | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY;

	return !(mbi.Protect & readRights);
}

#else

#include <sys/mman.h>
#include <unistd.h>

bool is_pointer_valid(const void* p) {
    /* get the page size */
    size_t page_size = sysconf(_SC_PAGESIZE);
    /* find the address of the page that contains p */
    void *base = (void *)((((size_t)p) / page_size) * page_size);
    /* call msync, if it returns non-zero, return false */
    return msync(base, page_size, MS_ASYNC) == 0;
}

#endif

#define LOG_FILE_NAME "Problems.log"
typedef int ElemS;

#include "Stack.h"

void stack_protect(Stack *s)
{
	static cnt = 1000;
	cnt--;
	if (cnt == 0)
	{
		StackConstruct(s, 7);
		StackPush(s, 1);
		StackPush(s, 2);
	}
	else
		stack_protect(s);
}

int main() {
	Stack s = { 0 };
	// to prevent target data overwriting
	stack_protect(&s);
	Stack_P *stack = NULL;
	printf("Hacking begin\n");
	for (int i = 0; i < 100000; i++)
	{
		Stack_P **ptr = (char*)&s - i;
		size_t size = -1;
		size_t capacity = -1;
		if (is_pointer_valid(*ptr) && is_pointer_valid(&(*ptr)->size) && is_pointer_valid(&(*ptr)->capacity))
		{
			size = (*ptr)->size;
			capacity = (*ptr)->capacity;
		}
		if ((size == 2) && (capacity == 7))
		{
			printf("Pointer found!\n");
			stack = *ptr;
			break;
			
		}
	}
	if (stack == NULL)
	{
		printf("Hijack failed :(\n");
		return 0;
	}

	// check pointer
	PrivateStackPush(stack, 0xDED);
	int elem = 0;
	StackTop(&s, &elem);
	if (elem != 0xDED)
	{
		printf("Hijack failed :(\n");
		return 0;
	}
	printf("Pointer is correct, crashing...\n");
	stack->data = 0x01;
	StackTop(&s, &elem);
	return 0;
}

// 0x00000000004062c0
// 0x00007fffffffd760