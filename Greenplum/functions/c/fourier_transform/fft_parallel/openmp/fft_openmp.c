#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <omp.h>

#include "postgres.h"
#include "funcapi.h"
#include "access/heapam.h"
#include "access/relscan.h"
#include "utils/fmgroids.h"
#include "utils/tqual.h"
#include "utils/builtins.h"
#include "executor/spi.h"

PG_MODULE_MAGIC;

int main( void );
void ccopy( int n, double x[], double y[] );
void cfft2( int n, double x[], double y[], double w[], double sgn );
void cffti( int n, double w[] );
double ggl( double *ds );
void step( int n, int mj, double a[], double b[], double c[], double d[], 
  double w[], double sgn );
void timestamp( void );

/*
  Purpose:
    CCOPY copies a complex vector.

  Discussion:
    The "complex" vector A[N] is actually stored as a double vector B[2*N].
    The "complex" vector entry A[I] is stored as:
      B[I*2+0], the real part,
      B[I*2+1], the imaginary part.

  Parameters:
    Input, int n, the length of the vector.
    Input, double X[2*N], the vector to be copied.
    Output, double Y[2*N], a copy of X.
*/
void ccopy( int n, double x[], double y[] )
{
  int i;

  for ( i = 0; i < n; i++ )
  {
    y[i*2+0] = x[i*2+0];
    y[i*2+1] = x[i*2+1];
   }
  return;
}

/*
  Purpose:
    CFFT2 performs a complex Fast Fourier Transform.

  Parameters:
    Input, int N, the size of the array to be transformed.
    Input/output, double X[2*N], the data to be transformed. On output, the contents of X have been overwritten by work information.
    Output, double Y[2*N], the forward or backward FFT of X.
    Input, double W[N], a table of sines and cosines.
    Input, double SGN, is +1 for a "forward" FFT and -1 for a "backward" FFT.
*/
void cfft2( int n, double x[], double y[], double w[], double sgn )
{
  int j;
  int m;
  int mj;
  int tgle;

  m = ( int ) ( log ( ( double ) n ) / log ( 1.99 ) );
  mj   = 1;

  //Toggling switch for work array.
  tgle = 1;
  step( n, mj, &x[0*2+0], &x[(n/2)*2+0], &y[0*2+0], &y[mj*2+0], w, sgn );

  if( n == 2 )
  {
    return;
  }

  for( j = 0; j < m - 2; j++ )
  {
    mj = mj * 2;
    if( tgle )
    {
      step( n, mj, &y[0*2+0], &y[(n/2)*2+0], &x[0*2+0], &x[mj*2+0], w, sgn );
      tgle = 0;
    }
    else
    {
      step ( n, mj, &x[0*2+0], &x[(n/2)*2+0], &y[0*2+0], &y[mj*2+0], w, sgn );
      tgle = 1;
    }
  }

  //Last pass through data: move Y to X if needed.
  if( tgle ) 
  {
    ccopy( n, y, x );
  }

  mj = n / 2;
  step( n, mj, &x[0*2+0], &x[(n/2)*2+0], &y[0*2+0], &y[mj*2+0], w, sgn );

  return;
}

/*
  Purpose:
    CFFTI sets up sine and cosine tables needed for the FFT calculation.

  Parameters:
    Input, int N, the size of the array to be transformed.
    Output, double W[N], a table of sines and cosines.
*/
void cffti( int n, double w[] )
{
  double arg;
  double aw;
  int i;
  int n2;
  const double pi = 3.141592653589793;

  n2 = n / 2;
  aw = 2.0 * pi / ( ( double ) n );

#pragma omp parallel \
    shared ( aw, n, w ) \
    private ( arg, i )
#pragma omp for nowait

  for( i = 0; i < n2; i++ )
  {
    arg = aw * ( ( double ) i );
    w[i*2+0] = cos( arg );
    w[i*2+1] = sin( arg );
  }
  return;
}

/* 
  Purpose:
    GGL generates uniformly distributed pseudorandom real numbers in [0,1]. 

  Parameters:
    Input/output, double *SEED, used as a seed for the sequence.
    Output, double GGL, the next pseudorandom value.
*/
double ggl(double *seed)
{
  double d2 = 0.2147483647e10;
  double t;
  double value;

  t = ( double ) *seed;
  t = fmod( 16807.0 * t, d2 );
  *seed = ( double ) t;
  value = ( double ) ( ( t - 1.0 ) / ( d2 - 1.0 ) );

  return value;
}

/*
  Purpose:
    STEP carries out one step of the workspace version of CFFT2.

  Parameters:

*/
void step( int n, int mj, double a[], double b[], double c[],
  double d[], double w[], double sgn )
{
  double ambr;
  double ambu;
  int j;
  int ja;
  int jb;
  int jc;
  int jd;
  int jw;
  int k;
  int lj;
  int mj2;
  double wjw[2];

  mj2 = 2 * mj;
  lj  = n / mj2;

#pragma omp parallel \
    shared ( a, b, c, d, lj, mj, mj2, sgn, w ) \
    private ( ambr, ambu, j, ja, jb, jc, jd, jw, k, wjw )
#pragma omp for nowait

  for( j = 0; j < lj; j++ )
  {
    jw = j * mj;
    ja  = jw;
    jb  = ja;
    jc  = j * mj2;
    jd  = jc;

    wjw[0] = w[jw*2+0]; 
    wjw[1] = w[jw*2+1];

    if( sgn < 0.0 ) 
    {
      wjw[1] = - wjw[1];
    }

    for( k = 0; k < mj; k++ )
    {
      c[(jc+k)*2+0] = a[(ja+k)*2+0] + b[(jb+k)*2+0];
      c[(jc+k)*2+1] = a[(ja+k)*2+1] + b[(jb+k)*2+1];

      ambr = a[(ja+k)*2+0] - b[(jb+k)*2+0];
      ambu = a[(ja+k)*2+1] - b[(jb+k)*2+1];

      d[(jd+k)*2+0] = wjw[0] * ambr - wjw[1] * ambu;
      d[(jd+k)*2+1] = wjw[1] * ambr + wjw[0] * ambu;
    }
  }
  return;
}

/*
  Purpose:
    TIMESTAMP prints the current YMDHMS date as a time stamp.

  Parameters:
    None
*/
void timestamp( void )
{
#define TIME_SIZE 40

  static char time_buffer[TIME_SIZE];
  const struct tm *tm;
  size_t len;
  time_t now;

  now = time ( NULL );
  tm = localtime ( &now );

  len = strftime ( time_buffer, TIME_SIZE, "%d %B %Y %I:%M:%S %p", tm );

  printf ( "%s\n", time_buffer );

  return;
#undef TIME_SIZE
}

/* 
  Purpose:
    fft的UDF实现

  Discussion:
    The "complex" vector A is actually stored as a double vector B.
    The "complex" vector entry A[I] is stored as:
      B[I*2+0], the real part,
      B[I*2+1], the imaginary part.

*/
PG_FUNCTION_INFO_V1(fft_main);
Datum 
fft_main(PG_FUNCTION_ARGS)
{
  int i,n;
  double sgn;
  double *w;
  double wtime;
  double *x,*y,*z;

  timestamp();

  ereport(INFO,(errmsg("  Number of processors available = %d\n", omp_get_num_procs())));
  ereport(INFO,(errmsg("  Number of threads =              %d\n", omp_get_max_threads())));

  //Prepare for tests.
  ereport(INFO,(errmsg("             N      Time\n")));

  n = 4;
  w = (double *) malloc(    n * sizeof(double));
  x = (double *) malloc(2 * n * sizeof(double));
  y = (double *) malloc(2 * n * sizeof(double));
  z = (double *) malloc(2 * n * sizeof(double));

  //初始化数据
  x[0]=1.0; x[1]=0.0;
  x[2]=2.0; x[3]=0.0;
  x[4]=4.0; x[5]=0.0;
  x[6]=3.0; x[7]=0.0;

  ereport(INFO,(errmsg("x=")));
  for(i=0; i<2*n; i++){
    ereport(INFO,(errmsg("%f,",x[i])));
  }

  //Initialize the sine and cosine tables.
  cffti(n, w);

  wtime = omp_get_wtime();

  //Transform forward
  sgn = + 1.0;

  //fft计算
  cfft2( n, x, y, w, sgn );
    
  //输出结果
  ereport(INFO,(errmsg("y=")));
  for(i=0; i<2*n; i++){
    ereport(INFO,(errmsg("%f,",y[i])));
  }

  //元素个数
  ereport(INFO,(errmsg("  %12d", n)));
  //运行时间
  wtime = omp_get_wtime() - wtime;
  ereport(INFO,(errmsg("  %12e\n", wtime)));

  free(w);
  free(x);
  free(y);

  //Terminate.
  ereport(INFO,(errmsg("  Normal end of execution.\n")));
  timestamp();

  PG_RETURN_VOID();
}
