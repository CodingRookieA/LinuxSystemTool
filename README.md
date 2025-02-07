# LinuxSystemTool
A program created in C that allows users to monitor the real-time resource usage of their laptop using customizable sample size and time intervals.

## Command Line Arguments
The program accepts several command-line arguments:

**--memory**  
    to indicate that only the memory usage should be generated.

**--cpu**  
    to indicate that only the CPU usage should be generated.

**--cores**  
    to indicate that only the cores information should be generated.

**--samples=N**  
    If used, the value N will indicate how many times the statistics are going to be collected, and results will be averaged and reported based on the N number of repetitions.  
    If no value is indicated, the default value will be **20**.

**--tdelay=T**  
    To indicate how frequently to sample in microseconds (1 microsecond = 10⁻⁶ sec).  
    If no value is indicated, the default value will be **0.5 sec = 500 milliseconds = 500000 microseconds**.

These last two arguments can also be specified as positional arguments if no flag is indicated, in the corresponding order: `samples tdelay`.

In this case, these arguments should be the first ones passed to the program.

## CLA Syntax:
```
./myMonitoringTool  [samples [tdelay]] [--memory] [--cpu] [--cores] [--samples=N] [--tdelay=T]
```

## Default Behavior
If no arguments are passed, the program presents all the information about memory utilization, CPU utilization, and cores.  
Default values: **samples = 20**, **tdelay = 500000 microseconds**.

## Special Note
Since the length of the y-axis is dynamically allocated depending on the sample size provided, when the sample size is too big, it might cause the title(“Nbr of samples…..”) and part of the graph to be not present in the default-sized terminal window. In this case, expanding the terminal window would resolve the issue.
If the user inputs one positional argument, like “./  a.out 40”, then it will set the sample size
If the user sets the input to larger than 100, it will trigger a sanity test to remind the user of their choice
If repetitive flags are passed in, then the program will use the rightmost flag provided, for example, “./a.out 50 –samples=25” will the sample size to 25
Sample size and delay must be positive integers
When using positional arguments, they must be the first to be inputted. Numerical values without flags will not be allowed anywhere else in the arguments, for example:
, “./a. out 50 –core 50” is prohibited
Users can input the same flag multiple times
If none of –core, –cpu, –memory flags is provided, then the program will print all their information, regardless of the rest of the command line arguments for example: “./a.out 50 250000 –samples=98 –tdelay=2500 –samples=60” will lead the program to print Memory, CPU, and core information with sample size 60 and delay of 2500 microseconds
When only core information is printed, the program will NOT print the title “Nbr of samples — X microSecs delay”, as sample size and delay are unreasonable under this context.


