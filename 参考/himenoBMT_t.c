#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#ifdef SUNOS5
#include <thread.h>
#endif
#include <pthread.h>
#include "barrier.h"

#ifdef SMALL
#define MIMAX            65
#define MJMAX            65
#define MKMAX            129
#endif

#ifdef MIDDLE
#define MIMAX            257
#define MJMAX            129
#define MKMAX            129
#endif

#ifdef LARGE
#define MIMAX            513
#define MJMAX            257
#define MKMAX            257
#endif

#define NN               200
#define BLK              (MIMAX-1)/Threads

double second();
void jacobi();
void initmt();

pthread_mutex_t *mt;
barrier_t bt;

static float  p[MIMAX][MJMAX][MKMAX],
              a[MIMAX][MJMAX][MKMAX][4],
              b[MIMAX][MJMAX][MKMAX][3],
              c[MIMAX][MJMAX][MKMAX][3];

static float  bnd[MIMAX][MJMAX][MKMAX];
static float  wrk1[MIMAX][MJMAX][MKMAX],
              wrk2[MIMAX][MJMAX][MKMAX];

static int imax, jmax, kmax;
static float omega, gosa;

int
main()
{
  int i, j, k, ret, tt;
  double cpu0, cpu1, nflop, xmflops2, score;
  
  pthread_t  *tid;

  omega = 0.8;
  gosa = 0.0;
  imax = MIMAX-1;
  jmax = MJMAX-1;
  kmax = MKMAX-1;

  /*
   *    Initializing matrixes
   */

  tid = (pthread_t *) calloc(Threads, sizeof(pthread_t)); 
  barrier_init(&bt);
  initmt();

  mt = (pthread_mutex_t *) calloc(sizeof (*mt) , sizeof (double));
  ret = pthread_mutex_init(mt, NULL);
  if (ret != 0) {
    (void) printf(" mutex_init Failed\n");
    return (1);
  }

  printf("  %d Threads used. \n",Threads);
  printf("  mimax = %d mjmax = %d mkmax = %d\n",MIMAX, MJMAX, MKMAX);
  printf("  imax = %d jmax = %d kmax = %d\n",imax, jmax, kmax);

#ifdef SUNOS5
  thr_setconcurrency(Threads);
#elseif SGI
  pthread_setconcurrency(Threads);
#endif

  cpu0 = second();
  for (tt = 0; tt < Threads; tt++) {
#ifdef SUNOS5
    ret = thr_create(NULL,NULL,(void *(*)(void *))jacobi, (int *)tt,
		      THR_BOUND|THR_DAEMON, &tid[tt]);
#else
    ret = pthread_create(&tid[tt], NULL, (void *(*)(void *))jacobi,
                         (int *) tt);
#endif
    if (ret != 0) {
      (void) printf("pthread_create failed \n");
      return (1);
    }
  }
  
  for (i = 0; i < Threads; i++) {
#ifdef SUNOS5
    ret = thr_join(tid[i],NULL,NULL);
#else
    ret = pthread_join(tid[i], NULL);
#endif
    if (ret != 0) {
      (void) printf("thr join failed\n");
      return (1);
    }
  }
  cpu1 = second();

  nflop = (kmax-2)*(jmax-2)*(imax-2)*34;

  if(cpu1 != 0.0)
    xmflops2 = nflop/(cpu1-cpu0)*1.0e-6*(float)NN;

  score = xmflops2/32.27;
  
  printf("  cpu : %f sec.\n", cpu1-cpu0);
  printf("  Loop executed for %d times\n",NN);
  printf("  Gosa : %e \n",gosa);
  printf("  MFLOPS measured : %f\n",xmflops2);
  printf("  Score based on MMX Pentium 200MHz : %f\n",score);
  
  return 0;
}

void
initmt()
{
  int i,j,k;

  for(i=0 ; i<imax ; ++i)
    for(j=0 ; j<jmax ; ++j)
      for(k=0 ; k<kmax ; ++k){
        a[i][j][k][0]=0.0;
        a[i][j][k][1]=0.0;
        a[i][j][k][2]=0.0;
        a[i][j][k][3]=0.0;
        b[i][j][k][0]=0.0;
        b[i][j][k][1]=0.0;
        b[i][j][k][2]=0.0;
        c[i][j][k][0]=0.0;
        c[i][j][k][1]=0.0;
        c[i][j][k][2]=0.0;
        p[i][j][k]=0.0;
        wrk1[i][j][k]=0.0;
        bnd[i][j][k]=0.0;
      }

  for(i=0 ; i<imax ; ++i)
    for(j=0 ; j<jmax ; ++j)
      for(k=0 ; k<kmax ; ++k){
        a[i][j][k][0]=1.0;
        a[i][j][k][1]=1.0;
        a[i][j][k][2]=1.0;
        a[i][j][k][3]=1.0/6.0;
        b[i][j][k][0]=0.0;
        b[i][j][k][1]=0.0;
        b[i][j][k][2]=0.0;
        c[i][j][k][0]=1.0;
        c[i][j][k][1]=1.0;
        c[i][j][k][2]=1.0;
        p[i][j][k]=(float)(i*i)/(float)((imax-1)*(imax-1));
        wrk1[i][j][k]=0.0;
        bnd[i][j][k]=1.0;
      }
}

void
jacobi(int * tt)
{
  int i,j,k,n,tid,start,end;
  float gosa0, s0, ss;
	 
  tid = (int)tt;

  if(tid == 0)   start = tid * BLK + 1;
  else           start = tid * BLK;

  if(tid == Threads-1)  end = tid * BLK + BLK -1;
  else                  end = tid * BLK + BLK;

  for(n=0 ; n<NN ; ++n){
    gosa0 = 0.0;

    for(i=start ; i<end ; ++i)
      for(j=1 ; j<jmax-1 ; ++j)
        for(k=1 ; k<kmax-1 ; ++k){
          s0 = a[i][j][k][0] * p[i+1][j  ][k  ]
             + a[i][j][k][1] * p[i  ][j+1][k  ]
             + a[i][j][k][2] * p[i  ][j  ][k+1]
             + b[i][j][k][0] * ( p[i+1][j+1][k  ] - p[i+1][j-1][k  ]
                               - p[i-1][j+1][k  ] + p[i-1][j-1][k  ] )
             + b[i][j][k][1] * ( p[i  ][j+1][k+1] - p[i  ][j-1][k+1]
                               - p[i  ][j+1][k-1] + p[i  ][j-1][k-1] )
             + b[i][j][k][2] * ( p[i+1][j  ][k+1] - p[i-1][j  ][k+1]
                               - p[i+1][j  ][k-1] + p[i-1][j  ][k-1] )
             + c[i][j][k][0] * p[i-1][j  ][k  ]
             + c[i][j][k][1] * p[i  ][j-1][k  ]
             + c[i][j][k][2] * p[i  ][j  ][k-1]
             + wrk1[i][j][k];

          ss = ( s0 * a[i][j][k][3] - p[i][j][k] ) * bnd[i][j][k];

          gosa0 = gosa0 + ss*ss;

          wrk2[i][j][k] = p[i][j][k] + omega * ss;
        }
   
    barrier(&bt,tid);

    for(i=start ; i<end ; ++i)
      for(j=1 ; j<jmax-1 ; ++j)
        for(k=1 ; k<kmax-1 ; ++k)
          p[i][j][k] = wrk2[i][j][k];

  } /* end n loop */

  pthread_mutex_lock(mt);
  gosa += gosa0;
  pthread_mutex_unlock(mt);
}

double
second()
{
#include <sys/time.h>

  struct timeval tm;
  double t ;

  static int base_sec = 0,base_usec = 0;

  gettimeofday(&tm,NULL);
  if(base_sec == 0 && base_usec == 0)
    {
      base_sec = tm.tv_sec;
      base_usec = tm.tv_usec;
      t = 0.0;
  }
else
  {
    t = (double) (tm.tv_sec-base_sec) + 
      ((double) (tm.tv_usec-base_usec))/1.0e6 ;
  }

  return t ;
}
