#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// int
#define TYPE int
typedef int elem_t;
#include "stack.h"
#undef TYPE

int main()
{
    my_stack_int stack = {};
    char cmd_buf[11];
    stack_construct_int(&stack, 4);
    for (;;)
    {
        scanf("%10s", cmd_buf);
        if (strcmp(cmd_buf, "push") == 0)
        {
            int elem = 0;
            scanf("%d", &elem);
            if ((stack_push_int(&stack, elem)) != STACK_OK)
                return -1;
            printf("ok\n");
        }
        else if (strcmp(cmd_buf, "back") == 0)
        {
            int elem = 0;
            stack_status_t status = stack_top_int(&stack, &elem);
            if (status == STACK_EMPTY)
                printf("error\n");
            else if (status == STACK_OK)
                printf("%d\n", elem);
            else
                return -1;
        }
        else if (strcmp(cmd_buf, "pop") == 0)
        {
            int elem = 0;
            stack_status_t status = stack_top_int(&stack, &elem);
            if (status == STACK_EMPTY)
                printf("error\n");
            else if (status == STACK_OK)
            {
                if (stack_pop_int(&stack) != STACK_OK)
                    return -1;
                printf("%d\n", elem);
            }
            else
                return -1;
        }
        else if (strcmp(cmd_buf, "size") == 0)
        {
            size_t size = 0;
            stack_size_int(&stack, &size);
            printf("%lu\n", size);
        }
        else if (strcmp(cmd_buf, "clear") == 0)
        {
            if (stack_destruct_int(&stack) != STACK_OK)
                return -1;
            if (stack_construct_int(&stack, 4) != STACK_OK)
                return -1;
            printf("ok\n");
        }
        else if (strcmp(cmd_buf, "exit") == 0)
        {
            printf("bye\n");
            break;
        }
        else if (strcmp(cmd_buf, "corrupt") == 0)
        {
            const char* trash = "           haha";
            memcpy(cmd_buf, trash, 16);
        }
    }
    stack_destruct_int(&stack);
    return 0;
}