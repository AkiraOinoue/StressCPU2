#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#define Threads	    2
#define L2Threads   1  /* (int)log2(Threads) */
#define TRUE        1
#define FALSE       0

typedef struct {
  volatile int ans_even[Threads][L2Threads];
  volatile int ans_odd[Threads][L2Threads];
  int intended[Threads][L2Threads];
  int episode[Threads];
} barrier_t;

void barrier_init(barrier_t *barrier);
void barrier(barrier_t *barrier, int thr_id);

