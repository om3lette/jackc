# Jackc frontend

Overall steps taken to produce vm-code form `.jack` source:

1. Source code parsing and syntax validation. Produces AST. See: [lexer](lexer), [parser](parser), [ast](ast)
2. IR handling
    - The first AST pass: Collect class names and subroutine signatures. See [first-ast-pass](ast/traversals/symtab_traversal.c), [function-registry](function-registry)
    - The second AST pass: Verify semantic validity by checking types, subroutine calls signatures, missing method definitions such as `dispose`, etc... See [second-ast-pass](ast/traversals/semantic_validity_traversal.c), [symbol table](symtable)
    - The third AST pass: Produce vm-code.  See [third-ast-pass](ast/traversals/code-gen)

The second and the third passes can be merged, but were intentionally separated to simplify the codebase.

All of the steps above support error recovery and error reporing. If the parser encounters an invalid syntax it will try to recover. No fail fast by design. See [error reporting](diagnostics-engine)
