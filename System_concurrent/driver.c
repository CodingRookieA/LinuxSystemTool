#include "print_con.h"



int main(int argc, char **argv)
{
    // Prepare the signal handler for Ctrl + C of parent
    struct sigaction newact;
    newact.sa_handler = sig_int_handler_parent;
    newact.sa_flags = 0;
    sigemptyset(&newact.sa_mask);

    // Prepare the signal handler for Ctrl + C of child
    struct sigaction newact_c;
    newact_c.sa_handler = SIG_IGN;
    newact_c.sa_flags = 0;
    sigemptyset(&newact_c.sa_mask);

    // Prepare and install hanlder for SIGSTP signal
    struct sigaction nothing;
    nothing.sa_handler = SIG_IGN;
    nothing.sa_flags = 0;
    sigemptyset(&nothing.sa_mask);
    sigaction(SIGTSTP, &nothing, NULL);

    // Default values of sample and delay
    int sample = 20;
    long delay = 500000;
    // Variables for determining which information to print
    bool Memory = false;
    bool CPU = false;
    bool Core = false;
    // No argument provided, default behaviour
    if (argc == 1)
    {
        core_mem_cpu_driver(sample, delay);
        return 0;
    }

    // If only one argument is provided, then check which information the user wish to print
    if (argc == 2)
    {
        int sample_input;
        long temp;
        char *remain;
        temp = strtol(argv[1], &remain, 10);
        if (*remain == '\0')
        {
            if (temp <= 0)
            {
                fprintf(stderr, "Sample size must be positive integers!\n");
                return 1;
            }
            if (temp >= 100)
            {
                sanity_check();
            }
            sample = (int)temp;
            core_mem_cpu_driver(sample, delay);
            return 0;
        }
        else if (strcmp(argv[1], "--memory") == 0)
        {
            mem_driver(sample, delay);
            return 0;
        }
        else if (strcmp(argv[1], "--cpu") == 0)
        {
            cpu_driver(sample, delay);
            return 0;
        }
        else if (strcmp(argv[1], "--cores") == 0)
        {
            core_driver();
            return 0;
        }
        else if (sscanf(argv[1], "--samples=%d", &sample_input) == 1)
        {
            if (sample_input <= 0)
            {
                fprintf(stderr, "Sample size must be positive integers!\n");
                return 1;
            }
            if (sample_input >= 100)
            {
                sanity_check();
            }
            sample = sample_input;
            core_mem_cpu_driver(sample, delay);
            return 0;
        }
        else if (sscanf(argv[1], "--tdelay=%ld", &temp) == 1)
        {
            if (temp <= 0)
            {
                fprintf(stderr, "delay must be positive integers!\n");
                return 1;
            }
            delay = temp;
            core_mem_cpu_driver(sample, delay);
            return 0;
        }
        else
        {
            fprintf(stderr, "Invalid argument for the program: %s\n", argv[1]);
            return 0;
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
            else if(strcmp(argv[i], "--cores") == 0){
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
            //When user input only the positional arguments, print all information
            if(Memory == false && CPU == false && Core == false){
                core_mem_cpu_driver(sample, delay);
                return 0;
            }
        }

    //Case 2: The first two are not positional arguments
    else{
    int potential_sample;
    long temp;
    long potential_delay;
    char* remain;
    temp = strtol(argv[1], &remain, 10);
    for(int i = 1; i < argc; i++){
     //The user can specify the number of inputs as first positional argument
    if(i==1 && *remain == '\0'){
            if(temp <= 0){
                fprintf(stderr, "Sample size must be positive integers!\n");
            }
            if(temp >= 100){
                    sanity_check();
            }
            sample = (int)temp;
            }
    else if(strcmp(argv[i], "--memory") == 0){
                Memory = true;
            }
    else if(strcmp(argv[i], "--cpu") == 0){
                CPU = true;
            } 
    else if(strcmp(argv[i], "--cores") == 0){
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

    // Now determine what information to display, case 1: No core will be printed
    if (!Core)
    {
        if (Memory == true && CPU == true)
        {
            mem_cpu_driver(sample, delay);
            return 0;
        }

        else if (Memory == true)
        {
            mem_driver(sample, delay);
            return 0;
        }

        else if (CPU == true)
        {
            cpu_driver(sample, delay);
            return 0;
        }

        //The only possibility is that no flag is specified, so default
        core_mem_cpu_driver(sample, delay);
        return 0;

    }

    // Case 2: Core needs to be printed
    else if (Core)
    {
        if (Memory == true && CPU == true)
        {
            core_mem_cpu_driver(sample, delay);
            return 0;
        }

        else if (Memory == true)
        {
            core_mem_driver(sample, delay);
            return 0;
        }

        else if (CPU == true)
        {
            core_cpu_driver(sample, delay);
            return 0;
        }

        //None is true, meaning only --cores is inputted
        core_driver();
        return 0;
    }

    return 0;
}