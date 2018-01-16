#include "postgres.h"
#include "executor/executor.h"  /* for GetAttributeByName() */
#include "funcapi.h"
#include "fft.h"

PG_MODULE_MAGIC;

#define  SAMPLE_NODES              (128)  
COMPLEX x[SAMPLE_NODES];  

//产生输入数据
static void MakeInput()  
{  
    int i;  
  
    for (i=0;i<SAMPLE_NODES;i++)  
    {  
        x[i].real = sin(PI*8*i/SAMPLE_NODES);  
        x[i].imag  = 0.0f;  
    }  
}

PG_FUNCTION_INFO_V1(fft_main);
 
Datum
fft_main(PG_FUNCTION_ARGS)  
{  
    int i = 0;
    float8 real = PG_GETARG_FLOAT8(0);
    float8 imag = PG_GETARG_FLOAT8(1);

    MakeInput();  

    fft(x,SAMPLE_NODES);  
    for (i=0; i<SAMPLE_NODES; i++) {
		ereport(INFO,
            (errcode(ERRCODE_UNDEFINED_OBJECT),
            errmsg("%.5f %.5f\n",x[i].real, x[i].imag)));
    }
  
	//printf("=======================\n");

    ifft(x,SAMPLE_NODES);
    for (i=0; i<SAMPLE_NODES; i++) {
        ereport(INFO,
            (errcode(ERRCODE_UNDEFINED_OBJECT),
            errmsg("%.5f %.5f\n", x[i].real, x[i].imag)));
    }
	
    /* TEST FFT with REAL INPUTS */
	/*
    MakeInput();  
    fft_real(x,SAMPLE_NODES);  

    for (i=0; i<SAMPLE_NODES; i++) {
        printf("%.5f %.5f\n", x[i].real, x[i].imag);
    }
  
    ifft_real(x,SAMPLE_NODES);  

    for (i=0; i<SAMPLE_NODES; i++) {
        printf("%.5f %.5f\n", x[i].real, x[i].imag);
    }
	*/

	PG_RETURN_NULL();
}  

int main(void)  
{  
    int i = 0;

    /* TEST FFT */
    MakeInput();  
    fft(x,SAMPLE_NODES);  

    for (i=0; i<SAMPLE_NODES; i++) {
        printf("%.5f %.5f\n", x[i].real, x[i].imag);
    }
  
	printf("=======================\n");

    ifft(x,SAMPLE_NODES);  
    for (i=0; i<SAMPLE_NODES; i++) {
        printf("%.5f %.5f\n", x[i].real, x[i].imag);
    }
	
    /* TEST FFT with REAL INPUTS */
	/*
    MakeInput();  
    fft_real(x,SAMPLE_NODES);  

    for (i=0; i<SAMPLE_NODES; i++) {
        printf("%.5f %.5f\n", x[i].real, x[i].imag);
    }
  
    ifft_real(x,SAMPLE_NODES);  

    for (i=0; i<SAMPLE_NODES; i++) {
        printf("%.5f %.5f\n", x[i].real, x[i].imag);
    }
	*/

    return 0;
}  

