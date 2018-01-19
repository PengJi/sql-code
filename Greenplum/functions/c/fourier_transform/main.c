#include "postgres.h"
#include "executor/executor.h"  /* for GetAttributeByName() */
#include "funcapi.h"
#include "fft.h"
#include "catalog/heap.h"
#include "access/heapam.h"
#include "access/relscan.h"
#include "utils/tqual.h"

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

//fft主函数
Datum
fft_main(PG_FUNCTION_ARGS)  
{  
    int i = 0;
    float8 real,imag;
    Relation reltb;
    HeapScanDesc scantb;
    HeapTuple tupletb;
    HeapTupleHeader thtb;
    bool aisnull,bisnull;

    Oid relid = PG_GETARG_OID(0);
    reltb = heap_open(relid, AccessShareLock);
    scantb = heap_beginscan(reltb, SnapshotNow, 0, NULL);
	while ((tupletb = heap_getnext(scantb, ForwardScanDirection)) != NULL){
        thtb = tupletb->t_data;
        real = DatumGetFloat8(GetAttributeByName(thtb,"n1",&aisnull));
        imag = DatumGetFloat8(GetAttributeByName(thtb,"n2",&aisnull));
        ereport(INFO,(errmsg("read tuple: %.5f %.5f\n",real, imag)));
    }

    heap_endscan(scantb);
    heap_close(reltb, AccessShareLock);

    // float8 real = PG_GETARG_FLOAT8(1);
    // float8 imag = PG_GETARG_FLOAT8(2);

    //产生输入数据
    MakeInput();  
    fft_real(x,SAMPLE_NODES);  
    for (i=0; i<SAMPLE_NODES; i++) {
        ereport(INFO,(errmsg("%.5f %.5f\n",x[i].real, x[i].imag)));
    }

    /*
    fft(x,SAMPLE_NODES);  
    for (i=0; i<SAMPLE_NODES; i++) {
		ereport(INFO,(errmsg("%.5f %.5f\n",x[i].real, x[i].imag)));
    }

    ifft(x,SAMPLE_NODES);
    for (i=0; i<SAMPLE_NODES; i++) {
        ereport(INFO,(errmsg("%.5f %.5f\n", x[i].real, x[i].imag)));
    }
    */
	
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
    MakeInput();  
    fft_real(x,SAMPLE_NODES);  

    for (i=0; i<SAMPLE_NODES; i++) {
        printf("%.5f %.5f\n", x[i].real, x[i].imag);
    }
  
    ifft_real(x,SAMPLE_NODES);  

    for (i=0; i<SAMPLE_NODES; i++) {
        printf("%.5f %.5f\n", x[i].real, x[i].imag);
    }

    return 0;
}  

