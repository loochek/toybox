
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>

#define null ((void*)0)

#define ASSERT_OK(stack) if (!StackOK(stack)) { PrivateStackDump(stack); assert(!"My Assert"); }

typedef enum {
	STACK_NULL, STACK_OVERFLOW, STACK_UNDERFLOW, STACK_OK
}StackErrCode;

typedef enum {
	NEW_STACK, STACK_PUSH, STACK_POP, STACK_TOP, STACK_SIZE, STACK_DUMP, ALL_STACKS_DESTRUCT, DESTRUCT_STACK
}WorkType;

typedef int ElemS;

typedef struct
{
	size_t size;
	size_t capacity;
	ElemS* data;
} Stack_P;

typedef struct
{
	size_t id;
} Stack;

StackErrCode StackWork(WorkType type, size_t * stackID, size_t capacity, ElemS value, ElemS * top, size_t * size);

void PrivateStackDump(Stack_P * stack) {
	static size_t count = 0;

	FILE* pFile = fopen(LOG_FILE_NAME, count == 0 ? "w" : "a");

	fprintf(pFile, "Stack Dump %zu:\n", ++count);

	fprintf(pFile, "\tStack(OK):[0x%p]\n", &stack);
	fprintf(pFile, "\t{\n");
	fprintf(pFile, "\t\tsize = %zu\n", stack->size);
	fprintf(pFile, "\t\tcapacity = %zu\n", stack->capacity);
	fprintf(pFile, "\t\tdata[0x%p]\n", &stack->data);
	fprintf(pFile, "\t\t{\n");

	if (stack->data != null) {
		fprintf(pFile, "\t\t\t*[%zu] = %d (canary)\n", 0, stack->data[0]);

		for (size_t i = 1; i < stack->size + 1U; ++i)
			fprintf(pFile, "\t\t\t*[%zu] = %d\n", i, stack->data[i]); //fix type

		fprintf(pFile, "\t\t\t*[%zu] = %d (canary)\n", stack->size + 1, stack->data[stack->size + 1]);
	}

	fprintf(pFile, "\t\t}\n");
	fprintf(pFile, "\t}\n\n");

	fclose(pFile);
}

char StackOK(Stack_P * stack) {
	assert(stack);

	if (stack == null) return 0;
	if (stack->data == null) return 0;
	if (stack->size > stack->capacity) return 0;

	if (stack->data[0] != stack->data[stack->size + 1]) return 0;

	size_t mask = 0;
	for (size_t i = 1; i < stack->size + 1U; ++i)
		mask ^= stack->data[i];
	if (stack->data[0] != mask) return 0;

	return 1;
}

StackErrCode PrivateStackConstruct(Stack_P * stack, size_t capacity) {
	assert(stack);
	if (!stack) return STACK_NULL;

	stack->size = 0;
	stack->capacity = capacity;

	stack->data = (ElemS*)calloc(capacity + 2, sizeof(ElemS)); // +2 for canary
	if (stack->data == null) return assert(!"No Memory"), STACK_OVERFLOW;

	return STACK_OK;
}

void PrivateStackDestruct(Stack_P * stack) {
	ASSERT_OK(stack);
	stack->size = 0;
	stack->capacity = 0;

	free(stack->data);
	stack->data = null;
}

StackErrCode StackResizeUp(Stack_P * stack) {
	ASSERT_OK(stack);
	if (stack->capacity > SIZE_MAX / 2) return STACK_OVERFLOW;

	size_t newCapacity = stack->capacity == 0 ? 1 : stack->capacity * 2;
	ElemS* newData = (ElemS*)realloc(stack->data, (newCapacity + 2) * sizeof(ElemS)); // +2 for canary

	if (newData == null) return STACK_OVERFLOW;
	else stack->capacity = newCapacity, stack->data = newData;

	return STACK_OK;
}

StackErrCode StackResizeDown(Stack_P * stack) {
	ASSERT_OK(stack);
	ElemS* newData = (ElemS*)realloc(stack->data, (stack->capacity / 2 + 2) * sizeof(ElemS)); // +2 for canary

	if (newData == null) return STACK_UNDERFLOW;
	else stack->capacity /= 2, stack->data = newData;

	return STACK_OK;
}


StackErrCode PrivateStackPush(Stack_P * stack, ElemS value) {
	ASSERT_OK(stack);

	if (stack->capacity <= stack->size)
		if (StackResizeUp(stack) == STACK_OVERFLOW) return PrivateStackDump(stack), STACK_OVERFLOW;

	stack->data[stack->size + 2] = stack->data[stack->size + 1];
	stack->data[++stack->size] = value;

	stack->data[0] ^= value;
	stack->data[stack->size + 1] ^= value;

	ASSERT_OK(stack);
	return STACK_OK;
}

StackErrCode PrivateStackPop(Stack_P * stack) {
	ASSERT_OK(stack);

	if (stack->size == 0) return STACK_UNDERFLOW;
	if (stack->capacity >= (stack->size - 1) * 4)
		if (StackResizeDown(stack) == STACK_UNDERFLOW) return PrivateStackDump(stack), STACK_UNDERFLOW;

	--stack->size;

	stack->data[0] ^= stack->data[stack->size + 1];
	stack->data[stack->size + 1] = stack->data[0];

	ASSERT_OK(stack);
	return STACK_OK;
}

size_t PrivateStackSize(Stack_P * stack) {
	ASSERT_OK(stack);
	return stack->size;
}

StackErrCode PrivateStackTop(Stack_P * stack, ElemS * top) {
	ASSERT_OK(stack);
	if (stack->size == 0) { PrivateStackDump(stack); return STACK_UNDERFLOW; } 
	*top = stack->data[stack->size];
	return STACK_OK;
}

StackErrCode StackConstruct(Stack * stack, size_t capacity) {
	assert(stack);
	return StackWork(NEW_STACK, &stack->id, capacity, null, null, null);
}

void StackDestruct(Stack * stack) {
	assert(stack);
	StackWork(DESTRUCT_STACK, &stack->id, null, null, null, null);
}

StackErrCode StackPush(Stack * stack, ElemS value) {
	assert(stack);
	return StackWork(STACK_PUSH, &stack->id, null, value, null, null);
}

StackErrCode StackPop(Stack * stack) {
	assert(stack);
	return StackWork(STACK_POP, &stack->id, null, null, null, null);
}

StackErrCode StackTop(Stack * stack, ElemS * top) {
	assert(stack);
	return StackWork(STACK_TOP, &stack->id, null, null, top, null);
}

size_t StackSize(Stack * stack) {
	assert(stack);
	size_t size = 0;
	StackWork(STACK_SIZE, &stack->id, null, null, null, &size);
	return size;
}

void StackDump(Stack * stack) {
	assert(stack);
	StackWork(STACK_DUMP, &stack->id, null, null, null, null);
}

void AllStacksDestruct() {
	StackWork(ALL_STACKS_DESTRUCT, null, null, null, null, null);
}


StackErrCode StackWorkConstruct(Stack_P * **Table, size_t * StackCount, WorkType type, size_t * stackID, size_t capacity) {
	if (*Table == null) {
		++(*StackCount);

		Stack_P** newTable = (Stack_P**)calloc(*StackCount, sizeof(Stack_P*));
		if (newTable != null) *Table = newTable;
		else return STACK_OVERFLOW;

		*stackID = *StackCount - 1;
	}
	else {
		char isFind = 0;
		for (size_t i = 0; i < *StackCount; ++i)
			if ((*Table)[i] == null) *stackID = i, isFind = 1;

		if (!isFind) {
			++(*StackCount);

			Stack_P** newTable = (Stack_P**)realloc(*Table, *StackCount * sizeof(Stack_P*));
			if (newTable != null) *Table = newTable;
			else return STACK_OVERFLOW;

			*stackID = *StackCount - 1;
		}
	}

	(*Table)[*stackID] = (Stack_P*)calloc(1, sizeof(Stack_P));
	if ((*Table)[*stackID] == null) return STACK_OVERFLOW;

	return PrivateStackConstruct((*Table)[*stackID], capacity);
}

StackErrCode StackWork(WorkType type, size_t * stackID, size_t capacity, ElemS value, ElemS * top, size_t * size) {
	static Stack_P** Table = null;
	static size_t StackCount = 0;

	if (type != NEW_STACK && type != ALL_STACKS_DESTRUCT && *stackID >= StackCount) return STACK_NULL;
	if (type != NEW_STACK && Table == null) return STACK_NULL;

	switch (type)
	{
	case NEW_STACK:
		return StackWorkConstruct(&Table, &StackCount, type, stackID, capacity);
		break;

	case DESTRUCT_STACK:
		if (Table[*stackID] == null) return STACK_NULL;

		PrivateStackDestruct(Table[*stackID]);
		free(Table[*stackID]);

		Table[*stackID] = null;
		break;

	case STACK_PUSH:
		return PrivateStackPush(Table[*stackID], value);
		break;

	case STACK_POP:
		return PrivateStackPop(Table[*stackID]);
		break;

	case STACK_TOP:
		return PrivateStackTop(Table[*stackID], top);
		break;

	case STACK_SIZE:
		*size = PrivateStackSize(Table[*stackID]);
		break;
	case ALL_STACKS_DESTRUCT:
		for (size_t i = 0; i < StackCount; ++i)
			if (Table[i] != null) free(Table[i]);

		free(Table);
		Table = null;
		StackCount = 0;
		break;
	case STACK_DUMP:
		PrivateStackDump(Table[*stackID]);
		break;
	default:
		return STACK_NULL;
	}
	return STACK_OK;
}