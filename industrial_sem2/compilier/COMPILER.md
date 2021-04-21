# Compile process description

## Lexer
Lexer splits a source file into tokens called lexems. A complete list of lexems can be seen in lexer.hpp. In a nutshell, source file is split into identifiers, numbers and special sequences.

For example: `fn test(a, b, c) { a = 10 };` is split into such sequence: fn keyword, identifier 'test', open parenthesis, identifier 'a', comma, identifier 'b', comma, identifier 'c', comma, compound begin, identifier 'a', assign, number 10, compound end, semicolon, program end.

## Parser
Parser reads lexems sequence and builds an abstract syntax tree (e.g. AST) of a program. Each node have a type, left and right children. Compilier can show AST by passing `--show-ast` command line option. This section provides information about AST nodes and describes how different language structures are translated into AST.

- Identifier -> AST_IDENTIFIER
- Number -> AST_NUMBER
- Operators -> AST_OPER_***
    - Left node is a left argument
    - Right node is a right argument
- Function call -> AST_CALL
    - Left node is a function name
    - Right node is arguments glued with AST_COMPOUND
- if and while -> AST_IF and AST_WHILE
    - Left node is condition
    - Right node is an if/while body
- Variable declaration -> AST_VAR_DECL
    - Left node is a variable name
    - Right node is an initial value
- Function definition -> AST_FUNC_DECL
    - Left node is AST_FUNC_HEAD
        - Left node is AST_IDENTIFIER (function name)
        - Right node is chain of AST_IDENTIFIERs (arguments) glued with AST_COMPOUND
    - Right node is a function body

TODO: complete this section

## Code generator

- All variables are stored on the stack
- From this point, the most difficult task is evaluating expressions efficiently
    - There are some good things like Ershov numbers and Sethi–Ullman algorithm, but the need to call functions in the middle of evaluating an expression complicates these algorithms.
        - Now function calls pushes all used registers and independently calculates 

TODO: complete this section