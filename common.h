
#ifndef COMMON_H
#define COMMON_H

#include "types.h"
#include "osstime.h"

#define KEY 19283746

#define EXIT_ON_ERROR(VAR, STR) if (VAR < 0) { \
		perror(STR); \
		exit(1); \
	}

#define S_TERMINATED 0
#define S_ACTIVE 1

struct shm_t {
	struct osstime time;
};

int shmid;
struct shm_t *shm;

void allocate(int with_cleanup);
void deallocate();
void getExistingSharedMemoryId();
void attach();
void detach();

#endif
