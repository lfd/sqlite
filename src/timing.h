#ifndef INLINE_H
#define INLINE_H

#include<time.h>

typedef struct timespec tstamp_t;

#define NSEC_PER_SEC 1000000000
static inline unsigned long diff(tstamp_t start, tstamp_t end)  {
    tstamp_t temp;
    if ((end.tv_nsec-start.tv_nsec) < 0) {
      temp.tv_sec = end.tv_sec-start.tv_sec-1;
      temp.tv_nsec = NSEC_PER_SEC+end.tv_nsec-start.tv_nsec;
    } else {
      temp.tv_sec = end.tv_sec-start.tv_sec;
      temp.tv_nsec = end.tv_nsec-start.tv_nsec;
    }

    if (temp.tv_sec != 0) {
      return((unsigned long)(temp.tv_sec*NSEC_PER_SEC + temp.tv_nsec));
    }

    return ((unsigned long)temp.tv_nsec);
}

static inline tstamp_t get_tstamp(void) {
	tstamp_t val;
	clock_gettime(CLOCK_MONOTONIC, &val);
	return(val);
    }

#endif
