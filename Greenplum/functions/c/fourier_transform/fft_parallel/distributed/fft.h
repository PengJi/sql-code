#ifndef __FFT_H__
#define __FFT_H__

#include "postgres.h"
#include "funcapi.h"
#include "cdb/cdbvars.h"

#define MAX_N 4096
#define PI 3.1415926535897932
#define EPS 10E-8
#define V_TAG 99
#define P_TAG 100
#define Q_TAG 101
#define R_TAG 102
#define S_TAG 103
#define S_TAG2 104
#define MAX_LINE 16384

typedef int BOOL; 

typedef struct 
{
    double r;
    double i;
}complex_t;

complex_t p[MAX_N],s[2*MAX_N],r[2*MAX_N];
complex_t w[2*MAX_N];
uint64 variableNum;
double transTime=0,totalTime=0,beginTime;
// MPI_Status status;

void comp_add(complex_t* result,const complex_t* c1,const complex_t* c2);
void comp_multiply(complex_t* result,const complex_t* c1,const complex_t* c2);
void shuffle(complex_t* f, int beginPos, int endPos);
void evaluate(complex_t* f, int beginPos, int endPos,const complex_t* x, 
	complex_t* y,int leftPos, int rightPos, int totalLength);
void print_res(const complex_t* f,int fLength);
void addTransTime(double toAdd);
void sendOrigData(int size);
void recvOrigData();
int read_data();

#endif   /* __FFT_H__ */
