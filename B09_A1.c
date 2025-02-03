#define _POSIX_C_SOURCE 200809L  // Enable POSIX features, since the test of my file is conducted in WSL and POSIX is hidden
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <sys/sysinfo.h>

//Pause the program for the given microsecond
void sleep_microsecond(long microsecond){
    struct timespec ts;

    //first, the whole seconds
    ts.tv_sec = (microsecond)/1000000;

    //Then, convert the remainder to nanoseconds
    ts.tv_nsec = (microsecond % 1000000) * 1000;

    nanosleep(&ts, NULL);

}


//Calculate the used RAM and total RAM in terms of GB and return their ratio
//@para delay, the flag user inputs to control the gap between sampling
float get_Memory_ratio(){
     struct sysinfo info;

      if (sysinfo(&info) != 0) {  // Call sysinfo() to populate the struct
        fprintf(stderr, "An error occured");
        return -1;  // Return -1 to indicate an error
    }

     float used = (float)(info.totalram - info.freeram)/(1024 * 1024 * 1024);

     float total = (float)(info.totalram)/(1024 * 1024 * 1024);
    
     return used/total;
}

//Get the total ram and return it as an integer
int get_total_Memory(){
    struct sysinfo info;

    if (sysinfo(&info) != 0) {  // Call sysinfo() to populate the struct
        perror("sysinfo failed");
        return -1;  // Return -1 to indicate an error
    }
    
    float total = (float)(info.totalram)/(1024 * 1024 * 1024);
    return (int)total;
}

//Return the amount of memory currently in use
float get_current_Memory(){
    struct sysinfo info;

    if (sysinfo(&info) != 0) {  // Call sysinfo() to populate the struct
        perror("sysinfo failed");
        return -1;  // Return -1 to indicate an error
    }
    
    float used = (float)(info.totalram - info.freeram)/(1024 * 1024 * 1024);
    return used;
}

//Return the ratio between the CPU ontime and total time of a given time interval
//@para delay: the time intervel between two samples in microseconds
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

/*
Initialize and return a chart matrix with the given parameters
@para height: The height of the matrix
@para sample_size: The length of the matrix
*/

char** initialize_matrix(int height, int sample_size){

    char** to_print;

    //Allocate memory
    to_print = malloc(sizeof(char*) * height);

    //Check if successful
    if(to_print == NULL){
        fprintf(stderr, "Initialization for outter array failed\n");
        exit(1);
    }

    for(int i = 0; i < height; i++){
        to_print[i] = malloc(sizeof(char) * sample_size);
            //Check if successful
            if(to_print[i] == NULL){
            fprintf(stderr, "Initialization for inner array failed\n");
            exit(1);
        }
    }
    for(int i = 0; i < height; i++){
        for(int j = 0; j < sample_size; j++){
            to_print[i][j] = ' ';
        }
    }

    //The first column is the Y-axis;
    for(int i = 0; i < height; i++){
        to_print[i][0] = '|';
    }

    //The last row is the X-axis;
    for(int i = 0; i < sample_size; i++){
        to_print[height-1][i] = '_';
    }

    return to_print;
}
    
/*
Print the given CPU matrix
@para to_print: Pointer to the target matrix
@para ratio: the ratio of the usage of CPU and the total time
@para height: length of the matrix
@para sample_size: length of the matrix
*/
void print_CPU_matrix(char** to_print, float ratio, int height, int sample_size){
    //Print the matrix
    printf("v CPU  %.2f%%\n", ratio * 100);
    for(int i = 0; i < height; i++){
        for(int j = 0; j < sample_size; j++){
                //Print the upper limit of the graph
                if(i==0 && j==0){
                   printf(" 100%% ");
                }
                //Print the lower limit of the graph
                else if(i==height - 1 && j==0){
                   printf("   0%% ");
                }
                //The rest of the graph must stay aligned
                else if(j==0){
                    printf("      ");
                }
                printf("%c", to_print[i][j]);
            }
            printf("\n");
        }
}

/*
Print the given CPU matrix
@para to_print: Pointer to the target matrix
@para current: the amount of memory currently in use
@para total: total amount of memory
@para height: length of the matrix
@para sample_size: length of the matrix
*/
void print_MEMORY_matrix(char** to_print, float current, int total, int height, int sample_size){
    //Print the matrix
        printf("v Memory  %.2f\n", current);
        for(int i = 0; i < height; i++){
            for(int j = 0; j < sample_size; j++){
                //Print the upper limit of the graph
                if(i==0 && j==0){
                    if(total < 10){
                    printf("  %dGB ", total);
                    }
                    else if(10<= total && total < 100){
                    printf(" %dGB ", total);
                    }
                }
                //Print the lower limit of the graph
                else if(i==height - 1 && j==0 ){
                    printf(" 0 GB ");
                }
                //The rest of the graph must stay aligned
                else if(j==0){
                    printf("      ");
                }
                printf("%c", to_print[i][j]);
            }
            printf("\n");
        }

}

//Return the maximum frequency of the cores in GHz
float get_Max_freq(){
    FILE* to_read;

    //Open the file for reading
    to_read = fopen("/proc/cpuinfo", "r");
    if(to_read == NULL){
        fprintf(stderr, "Error opening file");
        exit(1);
    }

    char line[256];
    float max_freq = 0;

    //Read the file until the last line is reached
    while(fgets(line, 256, to_read) != NULL){
        //Parse the line being read and obtain the maximum frequency
        float freq;
        if(sscanf(line, "cpu MHz : %f", &freq) == 1){
            if(freq > max_freq){
                max_freq = freq;
            }
        }
    }

    if(fclose(to_read) != 0){
        fprintf(stderr, "Error closing file");
        exit(1);
    }

    //Convert to GHz and return 
    return max_freq/1000;
}

//When the user inputs too large of a sample size, do a sanity check
void sanity_check(){
        printf("Unexpected large amount of sample input detected, first positional argument is sample size, not delay!\n");
        printf("Continue anyway? y/n: ");
        char answer;
        scanf("%c", &answer);
        bool valid = false;
        while(!valid){
            if(answer == 'n'){
                exit(0);
            }
            else if(answer == 'y'){
                return;
             }
            else{
                printf("Invalid command, try again\n");
                printf("Continue anyway? y/n: ");
                scanf(" %c", &answer);
            }
        }
}


/*
Print the dynamic graph of CPU usage
@para sample: The total number of samples to print
@para delay: The time interval between collecting samples measured in seconds
*/
void print_CPU(int sample, long delay){
    int remain = sample;
    int sample_size = sample + 2;
    
    //The chart's height changes as the sample size does
    int height = 10 + sample/20;

    char** to_print = initialize_matrix(height, sample_size);

    int count = 1;

    while(count <= remain){
         //Clean the previously printed matrix to update the graph
        printf("\033[2J\033[H");
        float ratio = get_cpu_utilization(delay);
        //Update the matrix
        int position = (int)((height * (1-ratio))) - 1;
        if(position <= 0){
            position = 0;
        }
        to_print[position][count] = ':';

        //Print the title
        printf("Nbr of samples: %d -- every %ld microSecs (%.3f secs)\n", sample, delay, (float)delay/1000000);

        //Print the matrix
        print_CPU_matrix(to_print, ratio, height, sample_size);
        count++;
        }
}

/*
Print the dynamic graph of the memory usage
@para sample: The total number of samples to print
@para delay: The time interval between collecting samples measured in seconds
*/
void print_MEMORY(int sample, long delay){
    int remain = sample;
    int sample_size = sample + 2;
    
    //The chart's height changes as the sample size does
    int height = 10 + sample/20;
    char** to_print = initialize_matrix(height, sample_size);

    int count = 1;
    
    while(count <= remain){
        //Clean the previously printed matrix to update the graph
        printf("\033[2J\033[H");

        //Delay
        sleep_microsecond(delay);

        float ratio = get_Memory_ratio();
        float current = get_current_Memory();
        int total = get_total_Memory();

        //Print the title
        printf("Nbr of samples: %d -- every %ld microSecs (%.3f secs)\n", sample, delay, (float)delay/1000000);

        //Update the matrix
        int position = (int)((height * (1-ratio))) - 1;
        if(position <= 0){
            position = 0;
        }
        to_print[position][count] = '#';

        //print the matrix
        print_MEMORY_matrix(to_print, current, total, height, sample_size);

        count++;
    }

}

/*
Print the graphs for Memory and CPU asynchrnously
@para sample: The total number of samples to print
@para delay: The time interval between collecting samples measured in seconds
*/
void print_MEMORY_AND_CPU(int sample, long delay){
    int remain = sample;
    int sample_size = sample + 2;
    
    //The chart's height changes as the sample size does
    int height = 10 + sample/20;

    char** MEMORY = initialize_matrix(height, sample_size);

    char** CPU = initialize_matrix(height, sample_size);

    int count = 1;
    
    while(count <= remain){
        //Clean the previously printed matrix to update the graph
        printf("\033[2J\033[H");

        //Print the title

        //Grab CPU usage
        float CPU_ratio = get_cpu_utilization(delay);


        //Grab Memory-relaed info
        float Memory_ratio = get_Memory_ratio();
        float current = get_current_Memory();
        int total = get_total_Memory();

        printf("Nbr of samples: %d -- every %ld microSecs (%.3f secs)\n", sample, delay, (float)delay/1000000);

        //Update the Memory matrix
        int position = (int)((height * (1-Memory_ratio))) - 1;
        if(position <= 0){
            position = 0;
        }
        MEMORY[position][count] = '#';

        //Update the CPU matrix
        position = (int)((height * (1-CPU_ratio))) - 1;
        if(position <= 0){
            position = 0;
        }
        CPU[position][count] = ':';

        //print the Memory matrix
        print_MEMORY_matrix(MEMORY, current, total, height, sample_size);
        printf("\n\n\n");
        ////print the CPU matrix
        print_CPU_matrix(CPU, CPU_ratio, height, sample_size);
        count++;
    }


}


//Print the number of logical cores and the maximum frequncy of processors on the machine
void print_Core(){
    int i, j;
    //obtain the number of processors on the machine
    int to_plot = get_nprocs_conf() ;
    //Obtain the maximum frequency of the cores
    float freq = get_Max_freq();
    //Number of cores per row
    int per_row = 4;
    int num_of_rows = (to_plot + per_row - 1)/per_row;
    printf("\n\nv Number of Cores: %d @ %.2f GHz\n", to_plot, freq);
    //controls the number of rows
    for(int row = 0; row < num_of_rows; row++){
        //If the number of remaining cores is smaller than that of the default, reduce the number of cores printed
        if(to_plot < per_row){
        per_row = to_plot;
        }
        for(i = 0; i < 3; i++){
            if(i == 0 || i == 2){
                //controls the number of squares on each row
                for(j = 0; j < per_row; j++){
                    for(int e  = 0; e <= 5; e++){
                     if(e == 0 || e == 4){
                        printf("+");
                    }else if(e == 5){
                        printf(" ");
                    }else{
                        printf("-");
                    }
                }
            }
            printf("\n");
        }else{
            for(j = 0; j < per_row; j++){
                for(int e  = 0; e <= 5; e++){
                    if(e == 0 || e == 4){
                        printf("|");
                    }else if(e == 5){
                        printf(" ");
                    }else{
                        printf(" ");
                    }
                }
            }
            printf("\n");
        }
    }
    to_plot = to_plot - per_row;
    }
}

int main(int argc, char** argv){
    //Default values of sample and delay
    int sample = 20;
    long delay = 500000;
    //Variables for determining which information to print
    bool Memory = false;
    bool CPU = false;
    bool Core = false;

    //No argument provided, default behaviour
    if(argc == 1){
        print_MEMORY_AND_CPU(sample, delay);
        print_Core();
        return 0;
    }

    //If only one argument is provided, then check which information the user wish to print
    if(argc == 2){
        int sample_input;
        long temp;
        char* remain;
        temp = strtol(argv[1], &remain, 10);
        if(*remain == '\0'){
            if(temp <= 0){
                fprintf(stderr, "Sample size must be positive integers!\n");
                return 1;
            }
            if(temp >= 100){
                sanity_check();
            }
            sample = (int)temp;
            print_MEMORY_AND_CPU(sample, delay);
            print_Core();
            return 0;

        }
        else if(strcmp(argv[1], "--memory") == 0){
            print_MEMORY(sample, delay);
            return 0;
        }
        else if(strcmp(argv[1], "--cpu") == 0){
            print_CPU(sample, delay);
            return 0;
        } 
        else if(strcmp(argv[1], "--core") == 0){
            print_Core();
            return 0;
        }
        else if(sscanf(argv[1], "--samples=%d", &sample_input) == 1){
            if(sample_input <= 0){
                    fprintf(stderr, "Sample size must be positive integers!\n");
                    return 1;
                 }
            if(sample_input >= 100){
                    sanity_check();
                }
            sample = sample_input;
            print_MEMORY_AND_CPU(sample, delay);
            print_Core();
            return 0;
        }
        else if(sscanf(argv[1],"--tdelay=%ld", &temp) == 1){
            if(temp <= 0){
                    fprintf(stderr, "delay must be positive integers!\n");
                    return 1;
                }
            delay = temp;
            print_MEMORY_AND_CPU(sample, delay);
            print_Core();
            return 0;
        }
        else{
            fprintf(stderr, "Invalid argument for the program: %s\n", argv[1]);
            return 1;
        }
    }

    //Now check for arguments in the case where there are more than 1 arguments
    
    //Check if user used positional argument
    long s, d;
    char *sam, *del;

    s = strtol(argv[1], &sam, 10);
    d = strtol(argv[2], &del, 10);

    //Case 1: The first two arguments are positional arguments
    if(*sam == '\0' && *del == '\0'){
        //Handle negative inputs
        if(s <= 0 || d <= 0){
            fprintf(stderr, "Positional arguments must be positive integers!\n");
            return 1;
        }

        //Sanity check
        if(s >= 100){
            sanity_check();
        }

        //Set the sample size and delay according to user input
        sample = s;
        delay = d;

        //check the remaining arguments
        int potential_sample;
        long potential_delay;

        for(int i = 3; i < argc; i++){

            if(strcmp(argv[i], "--memory") == 0){
                Memory = true;
            }
            else if(strcmp(argv[i], "--cpu") == 0){
                CPU = true;
            } 
            else if(strcmp(argv[i], "--core") == 0){
                Core = true;
            }
            else if(sscanf(argv[i], "--samples=%d",&potential_sample) == 1){
                    if(potential_sample <= 0){
                        fprintf(stderr, "Sample size must be positive integers!\n");
                        return 1;
                    }
                    if(potential_sample >= 100){
                        sanity_check();
                    }
                    sample = potential_sample;
                }
            else if(sscanf(argv[i], "--tdelay=%ld",&potential_delay) == 1){
                    if(potential_delay <= 0){
                        fprintf(stderr, "Delays must be positive integers!\n");
                        return 1;
                    }
                    delay = potential_delay;
            }
            else{
                fprintf(stderr, "Invalide argument: %s\n", argv[i]);
                return 1;
            }
            }
            printf("Memory: %d, CPU: %d, Core: %d\n", Memory, CPU, Core);
            //When user input only the positional arguments, print all information
            if(Memory == false && CPU == false && Core == false){
                print_MEMORY_AND_CPU(sample, delay);
                print_Core();
                return 0;
            }
        }

    //Case 2: The first two are not positional arguments
    else{
    int potential_sample;
    long temp;
    long potential_delay;
    char* remain;
    for(int i = 1; i < argc; i++){
        //The user can specify the number of inputs as first positional argument
        if(i==1){
        temp = strtol(argv[i], &remain, 10);
        if(*remain == '\0'){
                if(temp <= 0){
                    fprintf(stderr, "Sample size must be positive integers!\n");
                }
                if(temp >= 100){
                    sanity_check();
                }
                sample = (int)temp;
            }
        }
        else if(strcmp(argv[i], "--memory") == 0){
                Memory = true;
            }
            else if(strcmp(argv[i], "--cpu") == 0){
                CPU = true;
            } 
            else if(strcmp(argv[i], "--core") == 0){
                Core = true;
            }
            else if(sscanf(argv[i], "--samples=%d",&potential_sample) == 1){
                    if(potential_sample <= 0){
                        fprintf(stderr, "Sample size must be positive integers!\n");
                        return 1;
                    }
                    if(potential_sample >= 100){
                        sanity_check();
                    }
                    sample = potential_sample;
                }
            else if(sscanf(argv[i], "--tdelay=%ld",&potential_delay) == 1){
                    if(potential_delay <= 0){
                        fprintf(stderr, "Delays must be positive integers!\n");
                        return 1;
                    }
                    delay = potential_delay;
            }
            else{
                fprintf(stderr, "Invalide argument: %s\n", argv[i]);
                return 1;
            }
        }
    }
        
    //Now determine what information to display
    if(Memory == true && CPU == true){
        if(Core == true){
            print_MEMORY_AND_CPU(sample, delay);
            print_Core();
        }
        else{
            print_MEMORY_AND_CPU(sample, delay);
        }
    }

    else if(Memory == true){
        if(Core == true){
            print_MEMORY(sample, delay);
            print_Core();
        }else{
            print_MEMORY(sample,delay);
        }
    }

    else if(CPU == true){
        if(Core == true){
            print_CPU(sample, delay);
            print_Core();
        }else{
            print_CPU(sample, delay);
        }
    }

    else if(Core == true){
        print_Core();
    }

    else{
       print_MEMORY_AND_CPU(sample, delay);
       print_Core();
       return 0;
    }
    
    
    return 0;
}