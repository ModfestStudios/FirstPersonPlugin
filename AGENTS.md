# Code organization

- When adding or moving functions, place them under the appropriate existing commented group in both the header (`.h`) and implementation (`.cpp`). Follow the groups already laid out in each class.
- Keep function declarations in the header in the same relative order as their definitions in the implementation, including within each commented group, so corresponding functions are easy to locate.
- Preserve required C++ access specifiers when organizing declarations.

# Code style

- For a condition followed by a single return, put the return on the next line, indented one level beyond the condition.
- Concentrate local variable declarations near the top of the function or their relevant scope where dependencies and validation allow. Keep declarations together and separate them from executable statements with a blank line. Preserve initialization order when it affects behavior.
- Keep function-call arguments on one line. For callbacks with a multiline body, prefer a local callback variable so the call itself can stay on one line.
