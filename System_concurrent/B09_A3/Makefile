#default compiler
CC = gcc

# default flag for compilation
CFLAGS = -Werror -Wall -std=c99

#Dependencies for the driver file
NEEDED = driver.o cpu.o memory.o present.o print_con.o

#Target file
DIST = myMonitoringTool

.PHONY: clean

Target: ${NEEDED}
	${CC} ${CFLAGS} $^ -o ${DIST}

%.o: %.c
	${CC} ${CFLAGS} -c $^ -o $@

clean:
	rm -f ${NEEDED} ${DIST}