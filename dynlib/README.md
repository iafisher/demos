A minimal example of loading a shared library at runtime on Linux. The code to load the library is in `main.c`, and the library code itself is in `return_42.c`.

This technique may be useful if:

- Your application needs to load compiled plugins at runtime.
- You are writing a programming language interpreter that needs to dynamically load compiled code as part of a foreign-function interface.

Credit to [David A. Wheeler's chapter on dynamically loaded libraries](https://tldp.org/HOWTO/Program-Library-HOWTO/dl-libraries.html) for explaining this technique to me.
