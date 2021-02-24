## L1962 - Lisp Interpreter in C ##

By Matthew Haahr


Implements a Lisp-1 style environment with Lexical Scope.

Can read, interpret, evaluate, and print from files as well as stdin.

Has a full Lisp environment with support for variables and user-defined functions and prompting based on when the user is using a console using isatty() and fileno() to detect when stdin is being read from.

Implements try-catch and try-finally failure handling utilizing macros and setjmp.h.

Implements a version of struniq in conjunction with a hash set (using the sdbm hashing algorithm) to allow for storage of previously seen symbols.

Supports backquote for data-structure templates.

Utilizes a combination of Lisp and Scheme-like function names and removes some of the historical names that no longer make sense in modern contexts.
