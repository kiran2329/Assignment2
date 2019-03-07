
#include <stdlib.h>
#include <math.h>
#include "osstime.h"

void osst_advance(struct osstime *t, uint usec) {
	t->usec += usec;
	if (t->usec > 100000000) {
		t->sec += t->usec / 100000000;
		t->usec %= 100000000;
	}
}

int osst_past(struct osstime *t, uint sec, uint usec) {
	if (t->sec > sec)
		return 1;
	if (t->sec == sec)
		return t->usec > usec;
	return 0;
}
