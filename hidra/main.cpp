#include <CL/cl.h>
#include <stdio.h>
#include "mycllib.h"

int main (int argc, char* argv[]) {
    if (print_all_platforms_with_all_devices () == -1) {
        printf ("Error!\n");
    }
}