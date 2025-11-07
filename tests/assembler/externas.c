#include <stdio.h>
#include <stdlib.h>

int assert_equals(int obtained, int expected) {
    if (obtained == expected) {
        return 0;
    } else {
        printf("Test failed: obtained %d != expected %d\n", obtained, expected);
        return 1;
    }
}