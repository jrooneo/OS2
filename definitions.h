#ifndef DEFS
#define DEFS

//Important definitions for all files
#define BUFFERSIZE	128
#define BASESHMKEY 762354
#define BUFFERCOUNT 5
#define FLAGCOUNT 8

//Type definitions
typedef int bool;
enum { false, true };

//Standard set of includes needed in each program
#include <signal.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>

#endif
