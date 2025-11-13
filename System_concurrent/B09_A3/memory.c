#include "memory.h"


float get_Memory_ratio(){
    struct sysinfo info;

     if (sysinfo(&info) != 0) {  //Call sysinfo() to populate the struct
       fprintf(stderr, "An error occured");
       return -1;  //Return -1 to indicate an error
   }

    float used = (float)(info.totalram - info.freeram)/(1024 * 1024 * 1024);

    float total = (float)(info.totalram)/(1024 * 1024 * 1024);
   
    return used/total;
}


int get_total_Memory(){
    struct sysinfo info;

    if (sysinfo(&info) != 0) {  //Call sysinfo() to populate the struct
        perror("sysinfo failed");
        return -1;  //Return -1 to indicate an error
    }
    
    float total = (float)(info.totalram)/(1024 * 1024 * 1024);
    return (int)total;
}


float get_current_Memory(){
    struct sysinfo info;

    if (sysinfo(&info) != 0) {  //Call sysinfo() to populate the struct
        perror("sysinfo failed");
        return -1;  //Return -1 to indicate an error
    }
    
    float used = (float)(info.totalram - info.freeram)/(1024 * 1024 * 1024);
    return used;
}
