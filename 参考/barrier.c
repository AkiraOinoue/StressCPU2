#include "barrier.h"

void
barrier_init(barrier_t *barrier)
{
  int i, j, power;

  power = 1;

  for(i=0;i<L2Threads;i++){
    for(j=0;j<Threads;j++){
      barrier->intended[j][i]=(power+j)%Threads;
      barrier->ans_even[j][i] = TRUE;
      barrier->ans_odd[j][i] = FALSE;
      barrier->episode[j] = 0;
    }
    power = power*2;
  }
}

void
barrier(barrier_t *barrier, int thr_id)
{
  int i, signal, oddoreven;

  oddoreven = barrier->episode[thr_id]%2;
  signal = (barrier->episode[thr_id]/2)%2;
  
  for(i=0 ; i<L2Threads ; i++){
    if(oddoreven == 0){
      barrier->ans_even[barrier->intended[thr_id][i]][i] = signal;
      while (barrier->ans_even[thr_id][i] != signal)
        ;
    } else {
      barrier->ans_odd[barrier->intended[thr_id][i]][i] = signal;
      while (barrier->ans_odd[thr_id][i] != signal)
        ;
    }
  }
  barrier->episode[thr_id]++;
}
