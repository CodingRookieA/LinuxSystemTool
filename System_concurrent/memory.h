#include "cpu.h"


/*
This module contains functions on obtaining the cpu usage
*/



//Calculate and return the used RAM and total RAM in terms of GB and return their ratio
//delay, the flag user inputs to control the gap between sampling
float get_Memory_ratio();


//Return the total ram and return it as an integer
int get_total_Memory();


//Return the amount of memory currently in use
float get_current_Memory();