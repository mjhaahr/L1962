//
//  try.c
//  L1962
//
//  Created by Matthew Haahr on 1/4/21.
//

#include <stdio.h>
#include <string.h>

#include "try.h"

FailureHandler *failureHandler = NULL;

Failure currentFailure = { NULL };

noreturn void failInternal() {
    assert(currentFailure.message != NULL);
    FailureHandler *handler = failureHandler;
    failureHandler = handler->outer;
    longjmp(handler->target, 1);
}    

noreturn void failNoCopy(Failure failure) {
    if (failureHandler == NULL) {
        fprintf(stderr, "%s\n", failure.message);
        exit(1);                // or abort()
    }
    if (currentFailure.message != NULL) {
        fprintf(stderr, "failure (%s) while failure pending (%s)\n",
                failure.message, currentFailure.message);
    }
    currentFailure = failure;
    failInternal();
}

noreturn void fail(const char *message) {
    Failure failure;
    failure.message = strdup(message);
    failNoCopy(failure);
}

void printHandlerStack() {
    int n = 0;
    printf("[");
    for (FailureHandler *handler = failureHandler; handler != NULL; handler = handler->outer, n++) {
        printf("%s%p", (n > 0) ? " " : "", handler);
    }
    printf("]\n");
}


