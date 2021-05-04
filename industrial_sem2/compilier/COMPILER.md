# Compile process description

## Lexer
Lexer splits a source file into tokens called lexems. A complete list of lexems can be seen in lexer.hpp. In a nutshell, source file is split into identifiers, numbers and special sequences.

For example: `fn test(a, b, c) { a = 10 };` is split into such sequence: fn keyword, identifier 'test', open parenthesis, identifier 'a', comma, identifier 'b', comma, identifier 'c', comma, compound begin, identifier 'a', assign, number 10, compound end, semicolon, program end.

[More info about lexing process](lexer.cpp)

## Parser
Parser reads lexems sequence and builds an abstract syntax tree (e.g. AST) of a program. Each node have a type, left and right children. Tiny Compilier can show AST by passing `--show-ast` command line option. This section provides information about AST nodes and describes how different language structures are translated into AST.

- Identifier -> AST_IDENTIFIER
- Number -> AST_NUMBER
- Operators -> AST_OPER_***
    - Left node is a left argument
    - Right node is a right argument
- Function call -> AST_CALL
    - Left node is a function name
    - Right node is arguments glued with AST_COMPOUND
- Expressions
    - Trees, where leaves are AST_IDENTIFIER, AST_IDENTIFIER or AST_CALL, non-leaf nodes are operators
    - Classic expression trees
- Statements
    - expresion statements -> AST_EXPR_STMT
        - Left node is an expression
    - if -> AST_IF
        - Left node is a condition expression
        - Right node is AST_IF_BRANCHES
            - Left branch is an if body statement
            - Right branch is an else body statement if present
    - while -> AST_WHILE
        - Left node is a condition expression
        - Right node is an while body statement
    - Variable declaration -> AST_VAR_DECL
        - Left node is a variable name
        - Right node is an initial value expression
    - return -> AST_RETURN
        - Left node is a return value expression
    - Compound expression -> statements glued with AST_COMPOUND
        - Also compound expressions declare scopes - AST_SCOPE
            - Left node is a statement tree
- Function definition -> AST_FUNC_DECL
    - Left node is AST_FUNC_HEAD
        - Left node is AST_IDENTIFIER (function name)
        - Right node is chain of AST_IDENTIFIERs (arguments) glued with AST_COMPOUND
    - Right node is a function body statement
- Program -> AST_FUNC_DECLs glued with AST_COMPOUND
- AST_COMPOUND
    - Universal glue
    - Ordered
        - Must be traversed from a left child to a right child

[More info about parsing process](parser.cpp)

## Code generator
- All variables are stored on the stack
    - There are a variable table which controls usage of stack space and handles scopes
- From this point, the most difficult task is evaluating expressions efficiently
    - Tiny Compilier uses an approach similar to a register stack with stack usage when there are not enough registers
    - Function calls are handled like this: all currently used registers are pushed, function is called, registers are restored
- Other constructions are compiled quite straightforward
- Stack frame contents (from higher addresses to lower)
    - Saved caller-saved registers if used (r12-r14 uses for evaluating expresions whel all other scratch register are used, r15 used by division)
    - Variables
    - Temporary stack which for evaluating expresions (desribed before)
    - Space for stack arguments for function calls
    
[More info about code generation](code_gen.cpp)
    
## Binary emitter
- Translates AMD64 assembly to a native code and resolves symbols
- Small "linker" is embedded into binary emitter
- Puts generated code and standart library into ELF file
    - Minimal ELF - no sections, only one program header
        
[More info about binary generation](binary_emitter.cpp)
    
