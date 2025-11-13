#include "print_con.h"

void sig_int_handler_parent(int code)
{
    char buff[20];
    while (1)
    {
        printf("Program paused, do you wish to terminate? yes/no\n");
        scanf("%s", buff);
        if (strcmp(buff, "yes") == 0)
        {
            kill(0, SIGTERM);
            printf("Terminating program, thanks for using!\n");
            exit(0);
        }
        else if (strcmp(buff, "no") == 0)
        {
            return; // Return the control to process
        }
        printf("Invalid Input, try again\n");
    }
}

void mem_driver(int sample, long delay)
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

    // When only Memory needs to be printed
    int count = 1;
    // Pipe used for writing to child one
    int fd_1[2], mem_fb[2];
    if (pipe(fd_1) < 0 || pipe(mem_fb) < 0)
    {
        perror("Cannot create pipe!");
        exit(1);
    }

    int child = fork();

    if (child < 0)
    {
        perror("Fork failed!");
        exit(1);
    }

    // Child process for collecting information
    if (child == 0)
    {
        // Install handler for child
        if (sigaction(SIGINT, &newact_c, NULL) < 0)
        {
            perror("signal handler cannot be installed!");
            // Kill the parent
            kill(0, SIGTERM);
            exit(1);
        }

        int sig;

        if (close(fd_1[0]) < 0 || close(mem_fb[1]) < 0)
        {
            perror("Cannot close pipe!\n");
            // Kill the parent
            kill(0, SIGTERM);
            exit(1);
        }
        while (count <= sample)
        {   
            sleep_microsecond(delay);
            float ratio = get_Memory_ratio();
            float current = get_current_Memory();
            int total = get_total_Memory();
            char to_send[MAX_SIZE];
            sprintf(to_send, "%f %f %d", ratio, current, total);
            write(fd_1[1], to_send, MAX_SIZE);
            // Wait until parent finish printing
            read(mem_fb[0], &sig, sizeof(int));
            count++;
        }
        if (close(fd_1[1]) < 0)
        {

            perror("Cannot close pipe!\n");
            // Kill the parent
            kill(0, SIGTERM);
            exit(1);
        }
        exit(0);
    }
    // Parent for printing information
    else
    {
        // install signal handler for parent process
        if (sigaction(SIGINT, &newact, NULL) < 0)
        {
            perror("Signal handle installation failed!\n");
            exit(1);
        }
        if (close(fd_1[1]) < 0 || close(mem_fb[0]) < 0)
        {
            perror("Cannot close pipe!\n");
            exit(1);
        }
        print_MEMORY(sample, delay, fd_1[0], mem_fb[1]);
        if (close(fd_1[0]) < 0 || close(mem_fb[1]) < 0)
        {
            perror("Cannot close pipe!\n");
            exit(1);
        }
        // Wait for child before exit
        waitpid(child, NULL, 0);
    }
}

void cpu_driver(int sample, long delay)
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

    // When only CPU is printed
    int count = 1;

    // Pipe used for communicating with child one
    int fd_1[2], cpu_fb[2];
    if (pipe(fd_1) < 0 || pipe(cpu_fb) < 0)
    {
        perror("Cannot create pipe!");
        exit(1);
    }

    int child = fork();

    if (child < 0)
    {
        perror("Fork failed!");
        exit(1);
    }

    // Child process for collecting information
    if (child == 0)
    {
        // Install handler for child
        if (sigaction(SIGINT, &newact_c, NULL) < 0)
        {
            perror("signal handler cannot be installed!");
            // Kill the parent
            kill(0, SIGTERM);
            exit(1);
        }
        int sig;
        if (close(fd_1[0]) < 0 || close(cpu_fb[1]) < 0)
        {
            perror("Cannot close pipe!\n");
            // Kill the parent
            kill(0, SIGTERM);
            exit(1);
        }
        while (count <= sample)
        {
            float to_send = get_cpu_utilization(delay);
            write(fd_1[1], &to_send, sizeof(float));
            // Wait for parent to finish printing
            read(cpu_fb[0], &sig, sizeof(int));
            count++;
        }
        if (close(fd_1[1]) < 0 || close(cpu_fb[0]) < 0)
        {
            perror("Cannot close pipe!\n");
            // Kill the parent
            kill(0, SIGTERM);
            exit(1);
        }
        exit(0);
    }
    // Parent for printing information
    else
    {
        // install signal handler for parent processr
        if (sigaction(SIGINT, &newact, NULL) < 0)
        {
            perror("Signal handle installation failed!\n");
            exit(1);
        }
        if (close(fd_1[1]) < 0 || close(cpu_fb[0]) < 0)
        {
            perror("Cannot close pipe!\n");
            exit(1);
        }
        print_CPU(sample, delay, fd_1[0], cpu_fb[1]);
        if (close(fd_1[0]) < 0 || close(cpu_fb[1]) < 0)
        {
            perror("Cannot close pipe!\n");
            exit(1);
        }

        // Wait for child before exit
        waitpid(child, NULL, 0);
    }
}

void mem_cpu_driver(int sample, long delay)
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

    // Case when both Memory and CPU will be printed
    int fd_mem[2], fd_cpu[2], fd_mem_fb[2], fd_cpu_fb[2];

    int count = 1;

    if (pipe(fd_mem) < 0 || pipe(fd_cpu) < 0 || pipe(fd_mem_fb) < 0 || pipe(fd_cpu_fb) < 0)
    {
        perror("Cannot create pipe!");
    }

    int mem = fork();

    if (mem < 0)
    {
        perror("Fork failed!");
        exit(1);
    }

    // Fetch memory related information
    if (mem == 0)
    { // Install handler for child
        if (sigaction(SIGINT, &newact_c, NULL) < 0)
        {
            perror("signal handler cannot be installed!");
            // Kill the parent
            kill(0, SIGTERM);
            exit(1);
        }
        int sig;
        if (close(fd_mem[0]) < 0 || close(fd_mem_fb[1]) < 0 || close(fd_cpu_fb[0]) < 0 || close(fd_cpu_fb[1]) < 0)
        {
            perror("Cannot close read pipe for memory!\n");
            // Kill the parent
            kill(0, SIGTERM);
            exit(1);
        }
        while (count <= sample)
        {
            sleep_microsecond(delay);
            float ratio = get_Memory_ratio();
            float current = get_current_Memory();
            int total = get_total_Memory();
            char to_send[MAX_SIZE];
            sprintf(to_send, "%f %f %d", ratio, current, total);
            write(fd_mem[1], to_send, MAX_SIZE);
            // Wait until parent finish printing
            read(fd_mem_fb[0], &sig, sizeof(int));
            count++;
        }
        if (close(fd_mem[1]) < 0 || close(fd_mem_fb[0]) < 0)
        {
            perror("Cannot close write pipe for memory!\n");
            // Kill the parent
            kill(0, SIGTERM);
            exit(1);
        }
        exit(0);
    }

    else
    {
        // Fork another for collcecting cpu information
        int cpu = fork();
        if (cpu == 0)
        {
            // Install handler for child
            if (sigaction(SIGINT, &newact_c, NULL) < 0)
            {
                perror("signal handler cannot be installed!");
                // Kill the parent
                kill(0, SIGTERM);
                exit(1);
            }
            int sig = 1;
            if (close(fd_cpu[0]) < 0 || close(fd_mem_fb[0]) < 0 || close(fd_mem_fb[1]) < 0 || close(fd_cpu_fb[1]) < 0)
            {
                perror("Cannot close read pipe for cpu!\n");
                // Kill the parent
                kill(0, SIGTERM);
                exit(1);
            }
            while (count <= sample)
            {
                float to_send = get_cpu_utilization(delay);
                write(fd_cpu[1], &to_send, sizeof(float));
                // Read from parent process after it finishes printing the current matrix
                read(fd_cpu_fb[0], &sig, sizeof(int));
                count++;
            }
            if (close(fd_cpu[1]) < 0 || close(fd_cpu_fb[0]) < 0)
            {
                perror("Cannot close write pipe for cpu!\n");
                // Kill the parent
                kill(0, SIGTERM);
                exit(1);
            }
            exit(0);
        }

        // Parent process for printing collected information
        else
        {
            // install signal handler for parent processr
            if (sigaction(SIGINT, &newact, NULL) < 0)
            {
                perror("Signal handle installation failed!\n");
                exit(1);
            }
            if (close(fd_cpu[1]) < 0 || close(fd_mem[1]) < 0 || close(fd_mem_fb[0]) < 0 || close(fd_cpu_fb[0]) < 0)
            {
                perror("Cannot close writing pipe!");
            }

            print_MEMORY_AND_CPU(sample, delay, fd_mem[0], fd_cpu[0], fd_cpu_fb[1], fd_mem_fb[1]);
            if (close(fd_mem[0]) < 0 || close(fd_cpu[0]) < 0 || close(fd_cpu_fb[1]) < 0)
            {
                perror("Cannot close reading pipe!");
                exit(1);
            }
            // Wait for child before exit
            waitpid(mem, NULL, 0);
            waitpid(cpu, NULL, 0);
        }
    }
}

void core_driver()
{
    // Prepare the signal handler for Ctrl + C of parent
    struct sigaction newact;
    newact.sa_handler = sig_int_handler_parent;
    newact.sa_flags = 0;
    sigemptyset(&newact.sa_mask);

    int core[2];

    if (pipe(core) < 0)
    {
        perror("Cannot create pipe!\n");
        exit(1);
    }

    // Fork, child for getting data
    int core_c = fork();

    if (core_c < 0)
    {
        perror("Fork failed!\n");
        exit(1);
    }

    if (core_c == 0)
    {
        // Install handler
        if (sigaction(SIGINT, &newact, NULL) < 0)
        {
            perror("Signal handle installation failed!\n");
            // Kill the parent
            kill(0, SIGTERM);
            exit(1);
        }
        if (close(core[0]) < 0)
        {
            perror("Cannot close pipe!\n");
            exit(1);
        }
        // obtain the number of processors on the machine
        int to_plot = get_nprocs_conf();

        // Write to parent for printing
        write(core[1], &to_plot, sizeof(int));

        if (close(core[1]) < 0)
        {
            perror("Cannot close reading pipe!");
            // Kill the parent
            kill(0, SIGTERM);
            exit(1);
        }
        exit(0);
    }
    else
    {
        int freq_p[2];

        if (pipe(freq_p) < 0)
        {
            perror("Cannot create pipe");
            exit(1);
        }
        // Create another child for collecting frequency
        int freq = fork();

        if (freq < 0)
        {
            perror("Fork failed");
            exit(1);
        }

        if (freq == 0)
        {
            if (sigaction(SIGINT, &newact, NULL) < 0)
            {
                perror("Signal handle installation failed!\n");
                // Kill the parent
                kill(0, SIGTERM);
                exit(1);
            }
            if (close(core[0]) < 0 || close(freq_p[0]) < 0 || close(core[1]) < 0)
            {
                perror("Cannot close pipe!\n");
                kill(0, SIGTERM);
                exit(1);
            }

            // Obtain the maximum frequency of the cores
            float freq = get_Max_freq();
            write(freq_p[1], &freq, sizeof(float));

            if (close(freq_p[1]) < 0)
            {
                perror("Cannot close pipe!\n");
                kill(0, SIGTERM);
                exit(1);
            }
            exit(0);
        }

        if (close(core[1]) < 0 || close(freq_p[1]) < 0)
        {
            perror("Cannot close pipe\n");
            exit(1);
        }
        int to_plot;
        float freq_max;

        if (read(core[0], &to_plot, sizeof(int)) < 0 || read(freq_p[0], &freq_max, sizeof(float)) < 0) 
        {
            perror("Cannot fetch core info\n");
            exit(1);
        }

        print_Core(to_plot, freq_max);

        if (close(core[0]) < 0 || close(freq_p[0]))
        {
            perror("Cannot close pipe!\n");
            exit(1);
        }

        waitpid(core_c, NULL, 0);
        waitpid(freq, NULL, 0);
    }
}

void core_mem_driver(int sample, long delay)
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

    // When only Memory needs to be printed
    int count = 1;
    // Pipe used for writing to child one
    int fd_1[2], mem_fb[2];
    if (pipe(fd_1) < 0 || pipe(mem_fb) < 0)
    {
        perror("Cannot create pipe!");
        exit(1);
    }

    int child = fork();

    if (child < 0)
    {
        perror("Fork failed!");
        exit(1);
    }

    // Child process for collecting information
    if (child == 0)
    {
        int sig;
        // Install handler for child
        if (sigaction(SIGINT, &newact_c, NULL) < 0)
        {
            perror("signal handler cannot be installed!");
            // Kill the parent
            kill(0, SIGTERM);
            exit(1);
        }
        if (close(fd_1[0]) < 0 || close(mem_fb[1]) < 0)
        {
            perror("Cannot close pipe!\n");
            // Kill the parent
            kill(0, SIGTERM);
            exit(1);
        }
        while (count <= sample)
        {
            sleep_microsecond(delay);
            float ratio = get_Memory_ratio();
            float current = get_current_Memory();
            int total = get_total_Memory();
            char to_send[MAX_SIZE];
            sprintf(to_send, "%f %f %d", ratio, current, total);
            write(fd_1[1], to_send, MAX_SIZE);
            // Wait until parent finish printing
            read(mem_fb[0], &sig, sizeof(int));
            count++;
        }
        if (close(fd_1[1]) < 0)
        {
            perror("Cannot close pipe!\n");
            // Kill the parent
            kill(0, SIGTERM);
            exit(1);
        }
    }
    // Parent for printing information
    else
    {
        // Create anothe pipe to be used with Core
        int core[2];

        if (pipe(core) < 0)
        {
            perror("Cannot create pipe!\n");
            exit(1);
        }

        int core_c = fork();

        if (core_c < 0)
        {
            perror("Fork failed!\n");
            exit(1);
        }

        if (core_c == 0)
        {
            // Install handler for child
            if (sigaction(SIGINT, &newact_c, NULL) < 0)
            {
                perror("signal handler cannot be installed!");
                // Kill the parent
                kill(0, SIGTERM);
                exit(1);
            }
            if (close(fd_1[1]) < 0 || close(fd_1[0]) < 0 || close(mem_fb[1]) < 0 || close(mem_fb[0]) < 0 || close(core[0]) < 0)
            {
                perror("Problem closing pipe!\n");
                // Kill the parent
                kill(0, SIGTERM);
                exit(1);
            }
            // obtain the number of processors on the machine
            int to_plot = get_nprocs_conf();

            // Write to parent for printing
            write(core[1], &to_plot, sizeof(int));

            if (close(core[1]) < 0)
            {
                perror("Cannot close reading pipe!");
                // Kill the parent
                kill(0, SIGTERM);
                exit(1);
            }
            exit(0);
        }


        int freq_p[2];

        if(pipe(freq_p) < 0){
            perror("Cannot create pipe!");
            exit(1);
        }

        //Create another child for frequency
        int freq = fork();

        if(freq < 0){
            perror("Fork failed!");
            exit(1);
        }

        if(freq == 0){
             // Install handler for child
             if (sigaction(SIGINT, &newact_c, NULL) < 0)
             {
                 perror("signal handler cannot be installed!");
                 // Kill the parent
                 kill(0, SIGTERM);
                 exit(1);
             }
             if (close(fd_1[1]) < 0 || close(fd_1[0]) < 0 || close(mem_fb[1]) < 0 || close(mem_fb[0]) < 0 || close(core[0]) < 0 || close(core[1]) < 0 || close(freq_p[0]) < 0)
             {
                 perror("Problem closing pipe!\n");
                 // Kill the parent
                 kill(0, SIGTERM);
                 exit(1);
             }
             
             // Obtain the maximum frequency of the cores
             float freq = get_Max_freq();
 
             // Write to parent for printing
             write(freq_p[1], &freq, sizeof(float));
 
             if (close(freq_p[1]) < 0)
             {
                 perror("Cannot close reading pipe!");
                 // Kill the parent
                 kill(0, SIGTERM);
                 exit(1);
             }
             exit(0);
        }



        // install signal handler for parent processr
        if (sigaction(SIGINT, &newact, NULL) < 0)
        {
            perror("Signal handle installation failed!\n");
            exit(1);
        }
        if (close(fd_1[1]) < 0 || close(mem_fb[0]) < 0 || close(core[1]) < 0 || close(freq_p[1]) < 0)
        {
            perror("Cannot close pipe!\n");
            exit(1);
        }
        print_MEMORY(sample, delay, fd_1[0], mem_fb[1]);

        int to_plot;
        float max_freq;
        // Grab core information from core child;
        if (read(core[0], &to_plot, sizeof(int)) < 0 || read(freq_p[0], &max_freq, sizeof(float)) < 0)
        {
            perror("Core reading failed!\n");
            exit(1);
        }

        // Print the core
        print_Core(to_plot, max_freq);

        if (close(fd_1[0]) < 0 || close(mem_fb[1]) < 0 || close(core[0]) < 0 || close(freq_p[0]) < 0)
        {
            perror("Cannot close pipe!\n");
            exit(1);
        }
        // Wait for child to finish before exit
        waitpid(child, NULL, 0);
        waitpid(core_c, NULL, 0);
        waitpid(freq, NULL, 0);
    }
}

void core_cpu_driver(int sample, long delay)
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

    // When only CPU is printed
    int count = 1;

    // Pipe used for communicating with child one and core child
    int fd_1[2], cpu_fb[2], core[2];

    if (pipe(fd_1) < 0 || pipe(cpu_fb) < 0 || pipe(core) < 0)
    {
        perror("Cannot create pipe!");
        exit(1);
    }

    int child = fork();

    if (child < 0)
    {
        perror("Fork failed!");
        exit(1);
    }

    // Child process for collecting information
    if (child == 0)
    {
        // Install handler for child
        if (sigaction(SIGINT, &newact_c, NULL) < 0)
        {
            perror("signal handler cannot be installed!");
            // Kill the parent
            kill(0, SIGTERM);
            exit(1);
        }
        int sig;
        if (close(fd_1[0]) < 0 || close(cpu_fb[1]) < 0 || close(core[1]) < 0 || close(core[0]) < 0)
        {
            perror("Cannot close pipe!\n");
            // Kill the parent
            kill(0, SIGTERM);
            exit(1);
        }
        while (count <= sample)
        {
            float to_send = get_cpu_utilization(delay);
            write(fd_1[1], &to_send, sizeof(float));
            // Wait for parent to finish printing
            read(cpu_fb[0], &sig, sizeof(int));
            count++;
        }
        if (close(fd_1[1]) < 0 || close(cpu_fb[0]) < 0)
        {
            perror("Cannot close pipe!\n");
            // Kill the parent
            kill(0, SIGTERM);
            exit(1);
        }
        exit(0);
    }
    // Parent for printing information
    else
    {
        // Fork another child for collecting cpu info
        int core_c = fork();

        if (core_c < 0)
        {
            perror("Fork failed!\n");
            exit(1);
        }

        if (core_c == 0)
        {
            if (close(fd_1[1]) < 0 || close(fd_1[0]) < 0 || close(cpu_fb[1]) < 0 || close(cpu_fb[0]) < 0 || close(core[0]) < 0)
            {
                perror("Cannot close pipe!\n ");
                // Kill the parent
                kill(0, SIGTERM);
                exit(1);
            }
            // obtain the number of processors on the machine
            int to_plot = get_nprocs_conf();
            // Obtain the maximum frequency of the cores
            float freq = get_Max_freq();

            // Write to parent for printing
            write(core[1], &to_plot, sizeof(int));
            write(core[1], &freq, sizeof(float));

            if (close(core[1]) < 0)
            {
                perror("Cannot close reading pipe!");
                // Kill the parent
                kill(0, SIGTERM);
                exit(1);
            }
            exit(0);
        }



        int freq_p[2];

        if(pipe(freq_p) < 0){
            perror("Cannot create pipe!");
            exit(1);
        }

        int freq = fork();

        if(freq < 0){
            perror("Cannot create child!\n");
            exit(1);
        }

        //Another child for collecting max_freq
        if(freq == 0){
            if (close(fd_1[1]) < 0 || close(fd_1[0]) < 0 || close(cpu_fb[1]) < 0 || close(cpu_fb[0]) < 0 || close(core[0]) < 0 || close(core[1]) < 0 || close(freq_p[0]) < 0)
            {
                perror("Cannot close pipe!\n ");
                // Kill the parent
                kill(0, SIGTERM);
                exit(1);
            }

            // Obtain the maximum frequency of the cores
            float freq = get_Max_freq();

            // Write to parent for printing
            write(freq_p[1], &freq, sizeof(float));

            if(close(freq_p[1]) < 0){
                perror("Cannot close pipe!\n ");
                // Kill the parent
                kill(0, SIGTERM);
                exit(1);
            }

            exit(0);

        }
        // install signal handler for parent processr
        if (sigaction(SIGINT, &newact, NULL) < 0)
        {
            perror("Signal handle installation failed!\n");
            exit(1);
        }
        if (close(fd_1[1]) < 0 || close(cpu_fb[0]) < 0 || close(core[1]) < 0 || close(freq_p[1]) < 0)
        {
            perror("Cannot close pipe!\n");
            exit(1);
        }
        print_CPU(sample, delay, fd_1[0], cpu_fb[1]);

        int to_plot;
        float max_freq;

        // Grab core information from core child;
        if (read(core[0], &to_plot, sizeof(int)) < 0 || read(freq_p[0], &max_freq, sizeof(float)) < 0)
        {
            perror("Core reading failed!\n");
            exit(1);
        }

        // Print the core
        print_Core(to_plot, max_freq);

        if (close(fd_1[0]) < 0 || close(cpu_fb[1]) < 0 || close(core[0]) < 0 || close(freq_p[0]) < 0)
        {
            perror("Cannot close pipe!\n");
            exit(1);
        }
        // Wait for child to finish before exit
        waitpid(child, NULL, 0);
        waitpid(core_c, NULL, 0);
        waitpid(freq, NULL, 0);
    }
}

void core_mem_cpu_driver(int sample, long delay)
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

    // Case when both Memory and CPU will be printed
    int fd_mem[2], fd_cpu[2], fd_mem_fb[2], fd_cpu_fb[2];

    int count = 1;

    if (pipe(fd_mem) < 0 || pipe(fd_cpu) < 0 || pipe(fd_mem_fb) < 0 || pipe(fd_cpu_fb) < 0)
    {
        perror("Cannot create pipe!");
    }

    int mem = fork();

    if (mem < 0)
    {
        perror("Fork failed!");
        exit(1);
    }

    // Fetch memory related information
    if (mem == 0)
    {
        // Install handler for child
        if (sigaction(SIGINT, &newact_c, NULL) < 0)
        {
            perror("signal handler cannot be installed!");
            // Kill the parent
            kill(0, SIGTERM);
            exit(1);
        }
        int sig;
        if (close(fd_mem[0]) < 0 || close(fd_mem_fb[1]) < 0 || close(fd_cpu_fb[0]) < 0 || close(fd_cpu_fb[1]) < 0)
        {
            perror("Cannot close read pipe for memory!\n");
            // Kill the parent
            kill(0, SIGTERM);
            exit(1);
        }
        while (count <= sample)
        {
            sleep_microsecond(delay);
            float ratio = get_Memory_ratio();
            float current = get_current_Memory();
            int total = get_total_Memory();
            char to_send[MAX_SIZE];
            sprintf(to_send, "%f %f %d", ratio, current, total);
            write(fd_mem[1], to_send, MAX_SIZE);
            // Wait until parent finish printing
            read(fd_mem_fb[0], &sig, sizeof(int));
            count++;
        }
        if (close(fd_mem[1]) < 0 || close(fd_mem_fb[0]) < 0)
        {
            perror("Cannot close write pipe for memory!\n");
            // Kill the parent
            kill(0, SIGTERM);
            exit(1);
        }
        exit(0);
    }

    else
    {
        // Fork another for collcecting cpu information
        int cpu = fork();
        if (cpu < 0)
        {
            perror("Fork failed!\n");
        }
        if (cpu == 0)
        {
            // Install handler for child
            if (sigaction(SIGINT, &newact_c, NULL) < 0)
            {
                perror("signal handler cannot be installed!");
                // Kill the parent
                kill(0, SIGTERM);
                exit(1);
            }
            int sig = 1;
            if (close(fd_cpu[0]) < 0 || close(fd_mem_fb[0]) < 0 || close(fd_mem_fb[1]) < 0 || close(fd_cpu_fb[1]) < 0)
            {
                perror("Cannot close read pipe for cpu!\n");
                // Kill the parent
                kill(0, SIGTERM);
                exit(1);
            }
            while (count <= sample)
            {
                float to_send = get_cpu_utilization(delay);
                write(fd_cpu[1], &to_send, sizeof(float));
                // Read from parent process after it finishes printing the current matrix
                read(fd_cpu_fb[0], &sig, sizeof(int));
                count++;
            }
            if (close(fd_cpu[1]) < 0 || close(fd_cpu_fb[0]) < 0)
            {
                perror("Cannot close write pipe for cpu!\n");
                // Kill the parent
                kill(0, SIGTERM);
                exit(1);
            }
            exit(0);
        }

        // Parent process for printing collected information
        else
        {
            // install signal handler for parent processr
            if (sigaction(SIGINT, &newact, NULL) < 0)
            {
                perror("Signal handle installation failed!\n");
                exit(1);
            }

            // Create an additonal pipe for collecting coreinformation
            int core_fd[2];

            if (pipe(core_fd) < 0)
            {
                perror("Cannot open pipe!\n");
                exit(1);
            }

            int core = fork();

            if (core < 0)
            {
                perror("Fork failed!\n");
            }

            // In the child process, collects information for core number and max frequency
            if (core == 0)
            {
                if (close(fd_cpu[1]) < 0 || close(fd_mem[1]) < 0 || close(fd_mem_fb[0]) < 0 || close(fd_cpu_fb[0]) < 0 || close(core_fd[0]) < 0 || close(fd_cpu[0]) < 0 || close(fd_mem[0]) < 0 || close(fd_cpu_fb[1]) < 0 || close(fd_mem_fb[1]) < 0)
                {
                    perror("Cannot close writing pipe!");
                    // Kill the parent
                    kill(0, SIGTERM);
                    exit(1);
                }
                // obtain the number of processors on the machine
                int to_plot = get_nprocs_conf();

                // Write to parent for printing
                write(core_fd[1], &to_plot, sizeof(int));

                if (close(core_fd[1]) < 0)
                {
                    perror("Cannot close reading pipe!");
                    // Kill the parent
                    kill(0, SIGTERM);
                    exit(1);
                }
                exit(0);
            }

            int freq_p[2];

            if (pipe(freq_p) < 0)
            {
                perror("Cannot create pipe!");
                kill(0, SIGTERM);
                exit(1);
            }

            int freq = fork();

            if (freq < 0)
            {
                perror("Cannot create child process!");
                kill(0, SIGTERM);
                exit(1);
            }

            // Child for collecting frequency of core
            if (freq == 0)
            {
                if (close(fd_cpu[1]) < 0 || close(fd_mem[1]) < 0 || close(fd_mem_fb[0]) < 0 || close(fd_cpu_fb[0]) < 0 || close(core_fd[0]) < 0 || close(fd_cpu[0]) < 0 || close(fd_mem[0]) < 0 || close(fd_cpu_fb[1]) < 0 || close(fd_mem_fb[1]) < 0 || close(freq_p[0]) < 0 || close(core_fd[1]) < 0)
                {
                    perror("Cannot close writing pipe!");
                    // Kill the parent
                    kill(0, SIGTERM);
                    exit(1);
                }

                // Obtain the maximum frequency of the cores
                float freq = get_Max_freq();

                write(freq_p[1], &freq, sizeof(float));

                if (close(freq_p[1]) < 0)
                {
                    perror("Cannot close pipe!");
                    kill(0, SIGTERM);
                    exit(1);
                }

                exit(0);
            }

            if (close(fd_cpu[1]) < 0 || close(fd_mem[1]) < 0 || close(fd_mem_fb[0]) < 0 || close(fd_cpu_fb[0]) < 0 || close(core_fd[1]) < 0 || close(freq_p[1]) < 0)
            {
                perror("Cannot close writing pipe!");
            }

            print_MEMORY_AND_CPU(sample, delay, fd_mem[0], fd_cpu[0], fd_cpu_fb[1], fd_mem_fb[1]);

            int to_plot;
            float max_freq;
            // Grab core information from core child;
            if (read(core_fd[0], &to_plot, sizeof(int)) < 0 || read(freq_p[0], &max_freq, sizeof(float)) < 0)
            {
                perror("Core reading failed!\n");
                kill(0, SIGTERM);
                exit(1);
            }

            // Print the core
            print_Core(to_plot, max_freq);

            if (close(fd_mem[0]) < 0 || close(fd_cpu[0]) < 0 || close(fd_cpu_fb[1]) < 0 || close(fd_mem_fb[1]))
            {
                perror("Cannot close reading pipe!");
                exit(1);
            }

            // Wait untill all child process finish
            waitpid(mem, NULL, 0);
            waitpid(cpu, NULL, 0);
            waitpid(core, NULL, 0);
            waitpid(freq, NULL, 0);
        }
    }
}