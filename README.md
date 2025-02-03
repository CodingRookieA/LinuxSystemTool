# LinuxSystemTool
A program created in C that allows users to monitor the real-time resource usage of their laptop using customizable sample size and time interval

The program accepts several command line arguments:

--memory
        to indicate that only the memory usage should be generated


--cpu

        to indicate that only the CPU usage should be generated

 

--cores

        to indicate that only the cores information should be generated

 

--samples=N

        if used the value N will indicate how many times the statistics are going to be collected and results will be average and reported based on the N number of repetitions.
If not value is indicated the default value will be 20.


--tdelay=T

        to indicate how frequently to sample in micro-seconds (1 microsec = 10 -6 sec.)
If not value is indicated the default value will be 0.5 sec = 500 milisec = 500000 microsec.

 

These last two arguments can also be specified as positional arguments if not flag is indicated, in the corresponding order: samples tdelay.

In this case, these argument should be the first ones to be passed to the program.

 

CLA Syntax:
./myMonitoringTool  [samples [tdelay]] [--memory] [--cpu] [--cores] [--samples=N] [--tdelay=T]

 

Default Behavior
If no arguments are passed the program presents all the information about memory utilization, CPU utilization and cores
Default values for samples=20 , tdelay=500000 microsec.
