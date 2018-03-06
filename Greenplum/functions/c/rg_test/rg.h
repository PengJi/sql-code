#ifndef __FFT_H__
#define __FFT_H__

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
//mac下M_PI在math.h中有宏定义，所以这里我们选择行的宏定义
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#define SIZE 1024*16
#define VALUE_MAX 1000

////////////////////////////////////////////////////////////////////
//定义一个复数结构体
///////////////////////////////////////////////////////////////////
struct Complex_{
    double real;
    double imagin;
};
typedef struct Complex_ Complex;

void Add_Complex(Complex * src1,Complex *src2,Complex *dst);
void Sub_Complex(Complex * src1,Complex *src2,Complex *dst);
void Multy_Complex(Complex * src1,Complex *src2,Complex *dst);
void getWN(double n,double size_n,Complex * dst);
void setInput(double * data,int  n);
void DFT(double * src,Complex * dst,int size);
void IDFT(Complex *src,Complex *dst,int size);
int FFT_remap(double * src,int size_n);
void FFT(double * src,Complex * dst,int size_n);

#endif   /* __FFT_H__ */
