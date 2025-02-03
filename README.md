# LinuxSystemTool
A program created in C that allows users to monitor the real-time resource usage of their laptop using customizable sample size and time interval.

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

