
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include "common.h"

/* Global variables */

/* Arguments */
char *bin_name;
int total_limit = 4;
int concurrent_limit = 2;
int time_limit = 2;
char *input_file = "input.txt";
char *output_file = "output.txt";
FILE *input;
FILE *output;

/* Input file */
int have_input = 1;
uint increment;
uint next_sec, next_usec, next_duration;

/* State */

uint total_procs;
uint concurrent_procs;
pid_t *pids;

/* Global variables done */

/* Common functions */

/* Read next process start time */
void read_next_process() {
	if (!have_input)
		return;
	if (EOF == fscanf(input, "%d %d %d", &next_sec, &next_usec, &next_duration))
		have_input = 0;
}

/* Spawn a child and log it */
void spawn_process() {
	pid_t pid = fork();
	char dur_str[32];
	sprintf(dur_str, "%d", next_duration);

	if (pid == 0) {
		execlp("./user", "./user", dur_str, NULL);
	} else {
		pids[total_procs] = pid;
		fprintf(output, "[%6d %9d] Spawning Child With pid: %d at %d\n", shm->time.sec, shm->time.usec, pid, next_duration);
	}

	total_procs++;
	concurrent_procs++;
}

/* Log child termination */
void child_terminated(pid_t pid) {
	fprintf(output, "[%6d %9d] Terminated Child With pid: %d\n", shm->time.sec, shm->time.usec, pid);
	for (int i = 0; i < total_procs; i++) {
		if (pids[i] == pid) {
			pids[i] = 0;
		}
	}
	concurrent_procs--;
}

/* Terminate the OSS process */
void terminate() {
	fprintf(output, "[%6d %9d] Terminating\n", shm->time.sec, shm->time.usec);

	/* Terminate children */
	for (int i = 0; i < total_procs; i++) {
		if (pids[i] != 0) {
			kill(pids[i], 15);
		}
	}

	fclose(input);
	fclose(output);
	detach();
	exit(0);
}

/* Initialize OSS process */
void init() {
	allocate(1); /* Will call deallocate automatically */
	attach();
	shm->time.sec = 0;
	shm->time.usec = 0;

	input = fopen(input_file, "r");
	if (input == NULL)
		perror("fopen input");
	output = fopen(output_file, "w");
	if (output == NULL)
		perror("fopen output");

	pids = calloc(total_limit, sizeof(pid_t));

	signal(SIGALRM, terminate);
	signal(SIGINT, terminate);

	alarm(time_limit);
}

/* Main process */

void main_proc() {
	/* Reading initial data from the input file */
	fscanf(input, "%d", &increment);
	read_next_process();

	while(1) {
		/* Increment clock */
		osst_advance(&shm->time, increment);

		/* Check if time to spawn new process */
		if (osst_past(&shm->time, next_sec, next_usec) &&
				total_procs < total_limit &&
				concurrent_procs < concurrent_limit) {
			spawn_process();
			read_next_process();
		}

		/* Check if any children terminated */
		pid_t pid = waitpid(-1, NULL, WNOHANG);
		if (pid > 0) {
			child_terminated(pid);
		}

		/* Terminate? */
		if (!have_input && !concurrent_procs) {
			terminate();
			return;
		}
		if (!concurrent_procs && total_procs == total_limit) {
			terminate();
			return;
		}
	}

}

/* Pre-fork functions */

void show_usage() {
	printf("Usage: %s [options]\n", bin_name);
	printf("Options:\n");
	printf("	-h		Show this message\n");
	printf("	-n NUM		Set total process limit. Default: 4\n");
	printf("	-s NUM		Set concurrent process limit. Default: 2\n");
	printf("	-i INPUT	Set input filename. Default: input.dat\n");
	printf("	-o OUTPUT	Set output filename. Default: output.dat\n");
}

void do_args(int argc, char **argv) {
	int opt;
	while ((opt = getopt(argc, argv, "hi:o:s:n:")) != -1) {
		switch(opt) {
			case 'h':
				show_usage();
				exit(0);
			case 'n':
				total_limit = atoi(optarg);
				break;
			case 's':
				concurrent_limit = atoi(optarg);
				if (concurrent_limit > 20)
					concurrent_limit = 20;
				break;
			case 'i':
				input_file = optarg;
				break;
			case 'o':
				output_file = optarg;
		}
	}
}

int main(int argc, char **argv) {
	bin_name = argv[0];
	do_args(argc, argv);
	
	/* Dealing with shm */
	init();

	main_proc();
}
