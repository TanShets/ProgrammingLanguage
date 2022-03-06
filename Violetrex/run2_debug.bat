gcc mem_allocator/mem_alloc.h mem_allocator/mem_hasher.h mem_allocator/mem_lib.h
gcc symbols.h errors.h Lexer.h Context.h Parser.h value.h Interpreter.h
gcc -g compiler.c -o Violetrex2
gdb --args Violetrex2 tests/test4.vrx