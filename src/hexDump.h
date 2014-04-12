#ifndef _HEXDUMP_H_
#define _HEXDUMP_H_

#include <unistd.h> // for STDOUT_FILENO constant
#include <stdbool.h>

void setDebug(bool on);
void hexdump(void * data, int length);

#endif // _HEXDUMP_H_
