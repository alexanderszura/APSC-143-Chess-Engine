#include "panic.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include "tools.h"


// Used when the input is not properly formatted or missing elements
// Must output "parse error at character 'X', where X is replaced by the problematic character"
// Already implemented.
void panicf(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    exit(1);
}