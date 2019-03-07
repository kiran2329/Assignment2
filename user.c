
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "common.h"
#include "osstime.h"

int main(int argc, char **argv) {
	uint duration = atoi(argv[1]);

	attach();

	struct osstime termination = shm->time;
	osst_advance(&termination, duration);

	while(1) {
		if (osst_past(&shm->time, termination.sec, termination.usec)) {
			printf("[%6d %9d] Child %d terminating\n", shm->time.sec, shm->time.usec, getpid());
			return 0;
		}
	}

	return 0;
}
