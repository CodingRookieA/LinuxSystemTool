#include "memory.h"

/*
This module prints all the information collected by memory and cpu module. Additionally, it also grabs and prints information
on the core. Since there is little to no helper function requried for this module
*/



// When the user inputs too large of a sample size, do a sanity check
//Returns nothing
void sanity_check();

// Return the maximum frequency of the cores in GHz
float get_Max_freq();

/*
Initialize and return a chart matrix with the given parameters
//height: The height of the matrix
//sample_size: The length of the matrix
*/
char **initialize_matrix(int height, int sample_size);

/*
Print the given CPU matrix
//to_print: Pointer to the target matrix
//ratio: the ratio of the usage of CPU and the total time
//height: length of the matrix
//sample_size: length of the matrix
//Returns nothing
*/
void print_CPU_matrix(char **to_print, float ratio, int height, int sample_size);

/*
Print the given CPU matrix
//to_print: Pointer to the target matrix
//current: the amount of memory currently in use
//total: total amount of memory
//height: length of the matrix
//sample_size: length of the matrix
//Returns nothing
*/
void print_MEMORY_matrix(char **to_print, float current, int total, int height, int sample_size);

/*
Print the dynamic graph of CPU usage
//sample: The total number of samples to print
//delay: The time interval between collecting samples measured in seconds
//fd_read: file descriptor for getting data from parent
//fd_write:  writing back to child process(to indicate the finishing of printing, needed to handle signals)
//Returns nothing
*/
void print_CPU(int sample, long delay, int fd_read, int fd_write);

/*
Print the dynamic graph of the memory usage
//sample: The total number of samples to print
//delay: The time interval between collecting samples measured in seconds
//fd_read: file descriptor for getting data from child
//fd_write: fd for writing back to child process(to indicate the finishing of printing, needed to handle signals)
//Returns nothing
*/
void print_MEMORY(int sample, long delay, int fd_read, int fd_write);

/*
Print the graphs for Memory and CPU asynchrnously
//sample: The total number of samples to print
//delay: The time interval between collecting samples measured in seconds
//fd_read_1: file descriptor for getting data from child(memory)
//fd_read_2: fd to read from child 2(CPU)
//cpu_fd: fd for  writing back to child process(to indicate the finishing of printing, needed to handle signals)
//Returns nothing
//mem_fd: fd for writing back to child process for collecting memory
*/
void print_MEMORY_AND_CPU(int sample, long delay, int fd_read_1, int fd_read_2, int cpu_fd, int mem_fd);

// Print the number of logical cores and the maximum frequncy of processors on the machine
//Returns nothing
void print_Core(int to_plot, float freq);