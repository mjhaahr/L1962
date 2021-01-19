## L1962 - Lisp Interpreter in C ##

By Matthew Haahr


Implements a Lisp-1 style environment.

Can read, interpret, evaluate, and print from files as well as stdin.

Has a full Lisp environment with variable support and prompting based on when the user is using a console using isatty() and fileno() to detect when stdin is being read from.

Implements try-catch and try-finally failure handling utilizing macros and setjmp.h.

Implements struniq in conjunction with a hash set (with the sdbm hashing algorithm) to allow for storage of previously seen symbols.
