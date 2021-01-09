//
//  try.h
//  L1962
//
//  Created by Matthew Haahr on 1/4/21.
//

#ifndef try_h
#define try_h

#include <assert.h>
#include <stdlib.h>
#include <setjmp.h>
#include <stdnoreturn.h>

/**
	FailureHandler Struct, handles failures
	Contains a jmp_buf and a pointer to the outer (next) FailureHandler
*/
typedef struct FailureHandler FailureHandler;
struct FailureHandler {
    jmp_buf target;
    FailureHandler *outer;
};
extern FailureHandler *failureHandler; // General FailureHandler

/**
	Failure Struct, wrapper for the message
*/
typedef struct Failure Failure;
struct Failure{
    char *message;
};
extern Failure currentFailure; // currentFailure to handle


/**
	Handles currentFailure, and jumps to the the associate location on the stack for the jmp_buf
	Updates the general failureHandler to the next (outer) handler
	Calls lompjmp
*/
noreturn void failInternal(void);

/**
	Accepts a failure and verifies that it is not empty
	and that the currentFailure (the failure being handled at the moment) is empty
	If both above are true, it copies the given failure to currentFailure
	Calls failInternal
@param failure The failure to process
*/
noreturn void failNoCopy(Failure failure);

/**
	Creates a failure with the given message and strdups it
	Calls failNoCopy
@param fmt The message to copy (with sprintf style fill-ins)
*/
noreturn void fail(const char *fmt, ...);

/*
	Commence macro hell, for reference read the email
*/

#define TRY_CATCH(var, body, catch) \
    do { \
        FailureHandler __handler; \
        assert(failureHandler != &__handler); \
        __handler.outer = failureHandler; \
        failureHandler = &__handler; \
        if (setjmp(__handler.target) == 0) { \
            { body; } \
            failureHandler = __handler.outer; \
        } else { \
            Failure var = currentFailure; \
            currentFailure.message = NULL; \
            { catch; } \
             \
        } \
    } while (0)

#define TRY_FINALLY(body, finally) \
    do { \
        FailureHandler __handler; \
        assert(failureHandler != &__handler); \
        __handler.outer = failureHandler; \
        failureHandler = &__handler; \
        int __failed = 0; \
        if (setjmp(__handler.target) == 0) { \
            { body; } \
            failureHandler = __handler.outer; \
        } else { __failed = 1; } \
            { finally; } \
        if (__failed) { \
            failInternal(); \
        } \
    } while (0)

#define RETHROW(v) failNoCopy(v)

/**
	Prints the stack of handlers
*/
void printHandlerStack(void);


/**
	Replace assertions so they work with fail
*/
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define check(e) \
    do { \
        if (!(e)) { \
            fail(__FILE__ ":" TOSTRING(__LINE__) \
                 ": check failed: " #e); \
        } \
    } while (0)
    
#endif /* try_h */
