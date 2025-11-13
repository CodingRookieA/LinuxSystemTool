#include "present.h"

void sanity_check()
{
    printf("Unexpected large amount of sample input detected, first positional argument is sample size, not delay!\n");
    printf("Continue anyway? y/n: ");
    char answer;
    scanf("%c", &answer);
    bool valid = false;
    while (!valid)
    {
        if (answer == 'n')
        {
            exit(0);
        }
        else if (answer == 'y')
        {
            return;
        }
        else
        {
            printf("Invalid command, try again\n");
            printf("Continue anyway? y/n: ");
            scanf(" %c", &answer);
        }
    }
}

char **initialize_matrix(int height, int sample_size)
{

    char **to_print;

    // Allocate memory
    to_print = malloc(sizeof(char *) * height);

    // Check if successful
    if (to_print == NULL)
    {
        fprintf(stderr, "Initialization for outter array failed\n");
        exit(1);
    }

    for (int i = 0; i < height; i++)
    {
        to_print[i] = malloc(sizeof(char) * sample_size);
        // Check if successful
        if (to_print[i] == NULL)
        {
            fprintf(stderr, "Initialization for inner array failed\n");
            exit(1);
        }
    }
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < sample_size; j++)
        {
            to_print[i][j] = ' ';
        }
    }

    // The first column is the Y-axis;
    for (int i = 0; i < height; i++)
    {
        to_print[i][0] = '|';
    }

    // The last row is the X-axis;
    for (int i = 0; i < sample_size; i++)
    {
        to_print[height - 1][i] = '_';
    }

    return to_print;
}

void print_CPU_matrix(char **to_print, float ratio, int height, int sample_size)
{
    // Print the matrix
    printf("v CPU  %.2f%%\n", ratio * 100);
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < sample_size; j++)
        {
            // Print the upper limit of the graph
            if (i == 0 && j == 0)
            {
                printf(" 100%% ");
            }
            // Print the lower limit of the graph
            else if (i == height - 1 && j == 0)
            {
                printf("   0%% ");
            }
            // The rest of the graph must stay aligned
            else if (j == 0)
            {
                printf("      ");
            }
            printf("%c", to_print[i][j]);
        }
        printf("\n");
    }
}

void print_MEMORY_matrix(char **to_print, float current, int total, int height, int sample_size)
{
    // Print the matrix
    printf("v Memory  %.2f\n", current);
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < sample_size; j++)
        {
            // Print the upper limit of the graph
            if (i == 0 && j == 0)
            {
                if (total < 10)
                {
                    printf("  %dGB ", total);
                }
                else if (10 <= total && total < 100)
                {
                    printf(" %dGB ", total);
                }
            }
            // Print the lower limit of the graph
            else if (i == height - 1 && j == 0)
            {
                printf(" 0 GB ");
            }
            // The rest of the graph must stay aligned
            else if (j == 0)
            {
                printf("      ");
            }
            printf("%c", to_print[i][j]);
        }
        printf("\n");
    }
}

float get_Max_freq()
{
    FILE *to_read;

    // Open the file for reading
    to_read = fopen("/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_max_freq", "r");
    if (to_read == NULL)
    {
        fprintf(stderr, "Error opening file");
        exit(1);
    }

    float max_freq = 0;

    // Read the file for max freq
    if(fscanf(to_read, "%f", &max_freq) != 1){
        perror("Cannot collect core info!");
        exit(1);
    }

    if (fclose(to_read) != 0)
    {
        fprintf(stderr, "Error closing file");
        exit(1);
    }

    // Convert to GHz and return
    return max_freq / 1000000;
}

void print_CPU(int sample, long delay, int fd_read, int fd_write)
{
    int remain = sample;
    int sample_size = sample + 2;

    // The chart's height changes as the sample size does
    int height = 10 + sample / 20;

    char **to_print = initialize_matrix(height, sample_size);

    int count = 1;

    float ratio;

    while (count <= remain && read(fd_read, &ratio, sizeof(float)) != 0)
    {

        // Clean the previously printed matrix to update the graph
        printf("\033[2J\033[H");
        // Print the title
        printf("Nbr of samples: %d -- every %ld microSecs (%.3f secs)\n", sample, delay, (float)delay / 1000000);
        // Update the matrix
        int position = (int)((height * (1 - ratio))) - 1;
        if (position <= 0)
        {
            position = 0;
        }
        to_print[position][count] = ':';

        // Print the matrix
        print_CPU_matrix(to_print, ratio, height, sample_size);
        write(fd_write, &count, sizeof(int));
        count++;
    }

    printf("\n\n");
}

void print_MEMORY(int sample, long delay, int fd_read, int fd_write)
{
    int remain = sample;
    int sample_size = sample + 2;

    // The chart's height changes as the sample size does
    int height = 10 + sample / 20;
    char **to_print = initialize_matrix(height, sample_size);

    int count = 1;

    char buff[MAX_SIZE];
    while (count <= remain && read(fd_read, buff, MAX_SIZE) != 0)
    {
        // Clean the previously printed matrix to update the graph
        printf("\033[2J\033[H");

        float ratio;
        float current;
        int total;

        sscanf(buff, "%f %f %d", &ratio, &current, &total);

        // Print the title
        printf("Nbr of samples: %d -- every %ld microSecs (%.3f secs)\n", sample, delay, (float)delay / 1000000);

        // Update the matrix
        int position = (int)((height * (1 - ratio))) - 1;
        if (position <= 0)
        {
            position = 0;
        }
        to_print[position][count] = '#';

        // print the matrix
        print_MEMORY_matrix(to_print, current, total, height, sample_size);

        write(fd_write, &count, sizeof(int));

        count++;

        printf("\n\n");
    }
    printf("\n\n");
}

void print_MEMORY_AND_CPU(int sample, long delay, int fd_read_1, int fd_read_2, int cpu_fd, int mem_fd)
{
    int remain = sample;
    int sample_size = sample + 2;

    // The chart's height changes as the sample size does
    int height = 10 + sample / 20;

    char **MEMORY = initialize_matrix(height, sample_size);

    char **CPU = initialize_matrix(height, sample_size);

    int count = 1;

    char buff[MAX_SIZE];

    float CPU_ratio;

    while (count <= remain && read(fd_read_1, buff, MAX_SIZE) != 0 && read(fd_read_2, &CPU_ratio, sizeof(float)))
    {
        // Clean the previously printed matrix to update the graph
        printf("\033[2J\033[H");

        // Parse input to get the needed information for memory
        float Memory_ratio;
        float current;
        int total;
        sscanf(buff, "%f %f %d", &Memory_ratio, &current, &total);

        // Print the title
        printf("Nbr of samples: %d -- every %ld microSecs (%.3f secs)\n", sample, delay, (float)delay / 1000000);

        // Update the Memory matrix
        int position = (int)((height * (1 - Memory_ratio))) - 1;
        if (position <= 0)
        {
            position = 0;
        }
        MEMORY[position][count] = '#';

        // Update the CPU matrix
        position = (int)((height * (1 - CPU_ratio))) - 1;
        if (position <= 0)
        {
            position = 0;
        }
        CPU[position][count] = ':';

        // print the Memory matrix
        print_MEMORY_matrix(MEMORY, current, total, height, sample_size);

        printf("\n\n\n");
        ////print the CPU matrix
        print_CPU_matrix(CPU, CPU_ratio, height, sample_size);
        // Write to child processes to indicate the printing is finished
        write(cpu_fd, &count, sizeof(int));
        write(mem_fd, &count, sizeof(int));
        count++;
    }
}

void print_Core(int to_plot, float freq)
{
    int i, j;
    // Number of cores per row
    int per_row = 4;
    int num_of_rows = (to_plot + per_row - 1) / per_row;
    printf("\n\nv Number of Cores: %d @ %.2f GHz\n", to_plot, freq);
    // controls the number of rows
    for (int row = 0; row < num_of_rows; row++)
    {
        // If the number of remaining cores is smaller than that of the default, reduce the number of cores printed
        if (to_plot < per_row)
        {
            per_row = to_plot;
        }
        for (i = 0; i < 3; i++)
        {
            if (i == 0 || i == 2)
            {
                // controls the number of squares on each row
                for (j = 0; j < per_row; j++)
                {
                    for (int e = 0; e <= 5; e++)
                    {
                        if (e == 0 || e == 4)
                        {
                            printf("+");
                        }
                        else if (e == 5)
                        {
                            printf(" ");
                        }
                        else
                        {
                            printf("-");
                        }
                    }
                }
                printf("\n");
            }
            else
            {
                for (j = 0; j < per_row; j++)
                {
                    for (int e = 0; e <= 5; e++)
                    {
                        if (e == 0 || e == 4)
                        {
                            printf("|");
                        }
                        else if (e == 5)
                        {
                            printf(" ");
                        }
                        else
                        {
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
