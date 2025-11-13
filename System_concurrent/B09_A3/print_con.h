#include "present.h"

/*This module is a refactored module of present, which uses concurrency to collect and print data.
Additionally, this module enables the program to handle default signals differently*/

// Handler function for the main process. It will pause the program
// code: signal code to intercept
void sig_int_handler_parent(int code);

// Function for printing memory using concurrency
// sample: The total number of samples to print
// delay: The time interval between collecting samples measured in seconds
// Returns nothing
void mem_driver(int sample, long delay);

// Function for printing cpu using concurrency
// sample: The total number of samples to print
// delay: The time interval between collecting samples measured in seconds
// Returns nothing
void cpu_driver(int sample, long delay);

// Function for printing cpu and memory using concurrency
// sample: The total number of samples to print
// delay: The time interval between collecting samples measured in seconds
// Returns nothing
void mem_cpu_driver(int sample, long delay);

// Print the core-related information using concurrency
// Returns nothing
void core_driver();

// Function for printing memory and core using concurrency
// sample: The total number of samples to print
// delay: The time interval between collecting samples measured in seconds
// Returns nothing
void core_mem_driver(int sample, long delay);

// Function for printing cpu and core using concurrency
// sample: The total number of samples to print
// delay: The time interval between collecting samples measured in seconds
// Returns nothing
void core_cpu_driver(int sample, long delay);

// Function for printing cpu and memory and core using concurrency
// sample: The total number of samples to print
// delay: The time interval between collecting samples measured in seconds
// Returns nothing
void core_mem_cpu_driver(int sample, long delay);
