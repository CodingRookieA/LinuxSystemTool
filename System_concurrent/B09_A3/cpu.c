#include "cpu.h"

void sleep_microsecond(long microsecond){
    struct timespec ts;

    //first, the whole seconds
    ts.tv_sec = (microsecond)/1000000;

    //Then, convert the remainder to nanoseconds
    ts.tv_nsec = (microsecond % 1000000) * 1000;

    nanosleep(&ts, NULL);

}


float get_cpu_utilization(long delay){
    FILE* to_read;

    char cpu_1[256];
    char cpu_2[256];

    unsigned long user1, nice1, sys1, idel1, iowait1, irq1, softirq1, steal1, guest1, guest_nice1;
    unsigned long user2, nice2, sys2, idel2, iowait2, irq2, softirq2, steal2, guest2, guest_nice2;

    //Read the file for the first sample
    to_read = fopen("/proc/stat", "r");
    if(to_read == NULL){
        fprintf(stderr, "Cannot open file\n");
    }

    fscanf(to_read, "%s  %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu", cpu_1, &user1, &nice1, &sys1, &idel1, &iowait1, &irq1, &softirq1, &steal1, &guest1, &guest_nice1);

    if(fclose(to_read)!=0){
        fprintf(stderr, "Cannot close file\n");
    }

    sleep_microsecond(delay);

    //Read the file for the second sample
    to_read = fopen("/proc/stat", "r");
    if(to_read == NULL){
        fprintf(stderr, "Cannot open file\n");
    }

    fscanf(to_read, "%s  %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu", cpu_2, &user2, &nice2, &sys2, &idel2, &iowait2, &irq2, &softirq2, &steal2, &guest2, &guest_nice2);

    if(fclose(to_read)!=0){
        fprintf(stderr, "Cannot close file\n");
    }

    //Calculate the the CPU usage in the time interval and return as a ratio
    unsigned long total_1, total_2, active_1, active_2;

    total_1 = user1 + nice1 + sys1 + idel1 + iowait1 + irq1 + softirq1 + steal1 + guest1 + guest_nice1;

    total_2 = user2 + nice2 + sys2 + idel2 + iowait2 + irq2 + softirq2 + steal2 + guest2 + guest_nice2;

    active_1 = total_1 - idel1 - iowait1;

    active_2 = total_2 - idel2 - iowait2;

    //printf("total1: %lu, nothing1: %lu, total2: %lu, nothing2: %lu\n", total_1, idel1 + iowait1, total_2, idel2 + iowait2);
    
    return (float)(active_2 - active_1)/(total_2 - total_1);
}