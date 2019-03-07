
#ifndef OSSTIME_H
#define OSSTIME_H

#include "types.h"

struct osstime {
	ulong sec;
	ulong usec;
};

void osst_advance(struct osstime *time, uint usec);
int osst_past(struct osstime *time, uint sec, uint usec);

#endif
