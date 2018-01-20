#include "postgres.h"
#include "executor/executor.h"  /* for GetAttributeByName() */
#include "funcapi.h"
#include "fft.h"
#include "access/heapam.h"
#include "access/relscan.h"
#include "utils/fmgroids.h"
#include "utils/tqual.h"
#include "executor/spi.h"

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
	int i;
    char *command="select val from test order by id";
    int ret,cnt;
    uint64 proc;
    float r;

    //command = text_to_cstring(PG_GETARG_TEXT_P(0));
    cnt = PG_GETARG_INT32(1);

    SPI_connect();
    ret = SPI_exec(command, cnt);
    proc = SPI_processed;

    if (ret > 0 && SPI_tuptable != NULL){
        TupleDesc tupdesc = SPI_tuptable->tupdesc;
        SPITupleTable *tuptable = SPI_tuptable;
        char buf[8192];
        uint64 j;

        for (j = 0; j < proc; j++) //proc为表的行数
        {
            HeapTuple tuple = tuptable->vals[j];
            int i;

            for (i = 1, buf[0] = 0; i <= tupdesc->natts; i++){
                snprintf(buf + strlen (buf), sizeof(buf) - strlen(buf), " %s%s",
                        SPI_getvalue(tuple, tupdesc, i),
                        (i == tupdesc->natts) ? " " : " |");
            }

            ereport(INFO,(errmsg("ROW: %s",buf))); //输出一行数据
            sscanf(buf,"%f",&r);
            x[j].real = r;
            x[j].imag = 0.0f;
        }
    }

    fft_real(x,proc);
    for(i=0; i<proc; i++){
        ereport(INFO,(errmsg("%.5f %.5f\n",x[i].real, x[i].imag)));
    }

    /*
    //产生输入数据
    MakeInput(); 
    //fft 
    fft_real(x,SAMPLE_NODES);  
    for (i=0; i<SAMPLE_NODES; i++) {
        //ereport(INFO,(errmsg("%.5f %.5f\n",x[i].real, x[i].imag)));
    }
    */

	SPI_finish();
	//pfree(command);

    //PG_RETURN_NULL();
	PG_RETURN_INT32(proc);
}

PG_FUNCTION_INFO_V1(fft_main1);
Datum
fft_main1(PG_FUNCTION_ARGS)  
{  
    int i = 0;
    float8 real,imag;
    Relation reltb;
    HeapScanDesc scantb;
    HeapTuple tupletb;
    HeapTupleHeader thtb;
	ScanKeyData entry;
    bool aisnull,bisnull;

    Oid relid = PG_GETARG_OID(0);

	//ereport(INFO,(errmsg("read tuple: %d",relid)));

    reltb = heap_open(relid, AccessShareLock);

	ereport(INFO,(errmsg("rd_id: %d",reltb->rd_id)));
	ereport(INFO,(errmsg("natts: %d",reltb->rd_att->natts)));

	/*
	ScanKeyInit(&entry,
				Anum_pg_index_indisclustered,
				BTEqualStrategyNumber, F_BOOLEQ,
				BoolGetDatum(true));
	scantb = heap_beginscan(reltb, SnapshotNow, 1, &entry);
	*/
	scantb = heap_beginscan(reltb, SnapshotNow, 0, NULL);
	if(scantb != NULL){
		ereport(INFO,(errmsg("scantb is not NULL")));
	}else{
		ereport(INFO,(errmsg("scantb is NULL")));
	}

	ereport(INFO,(errmsg("rs_ntuples: %d",scantb->rs_ntuples)));
	ereport(INFO,(errmsg("rs_nblocks: %d",scantb->rs_nblocks)));
	ereport(INFO,(errmsg("rs_startblock: %d",scantb->rs_startblock)));

	/*
	if((tupletb = heap_getnext(scantb, ForwardScanDirection)) != NULL){
		ereport(INFO,(errmsg("tupletb is not NULL")));
	}else{
		ereport(INFO,(errmsg("tupletb is NULL")));
	}
	*/

	while ((tupletb = heap_getnext(scantb, ForwardScanDirection)) != NULL){
        thtb = tupletb->t_data;
        real = DatumGetFloat8(GetAttributeByName(thtb,"n1",&aisnull));
        imag = DatumGetFloat8(GetAttributeByName(thtb,"n2",&aisnull));
        ereport(INFO,(errmsg("read tuple: %.5f %.5f\n",real, imag)));
    }

	//问题：tupletb为空

    heap_endscan(scantb);
    heap_close(reltb, AccessShareLock);

    // float8 real = PG_GETARG_FLOAT8(1);
    // float8 imag = PG_GETARG_FLOAT8(2);

    //产生输入数据
    MakeInput();  
    fft_real(x,SAMPLE_NODES);  
    for (i=0; i<SAMPLE_NODES; i++) {
        //ereport(INFO,(errmsg("%.5f %.5f\n",x[i].real, x[i].imag)));
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

