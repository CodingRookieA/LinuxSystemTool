#define _POSIX_C_SOURCE 200809L // Enable POSIX features, since the test of my file is conducted in WSL and POSIX is hidden
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/sysinfo.h>

#define MAX_SIZE 400


/*
This module collects information about CPU usage. Aditionally, the module contains a delay function which can be used by other modules
*/

//Pause the program for the given microsecond
//microsecond: Number of microseconds to sleep for
//Returns nothing
void sleep_microsecond(long microsecond);


//Return the ratio between the CPU ontime and total time of a given time interval
//delay: the time intervel between two samples in microseconds
float get_cpu_utilization(long delay);




