#include "postgres.h"
#include "fmgr.h"
#include "executor/spi.h"

PG_MODULE_MAGIC;

/*
 * 使用SPI_*()执行一个SQL查询
 */
PG_FUNCTION_INFO_V1(add_ab);
Datum
count_returned_rows(PG_FUNCTION_ROWS){
	char *command;
	int cnt;
	int ret;
	int proc;

	//get arguments, concert command to c string
	command = text_to_cstring(PG_GETARG_TEXT_P(0));
	cnt = PG_GETARG_INT32(1);

	//open internal connection
	SPI_connect();
	//run the SQL command
	ret = SPI_exec(command,cnt);
	//save the number of rows
	proc = SPI_processed;
	//if some rows were fetched, print them via elog(INFO)
	if(ret > 0 && SPI_tuptable != NULL){
		TupleDesc tupdesc = SPI_tuptable -> tupdesc;
		SPITupleTable *tuptable = SPI_tuptable;
		char buf[8192];
		int i,j;

		for(j = 0; j<proc; j++){
			HeapTuple tuple = tuptable->vals[j];
			//construct a string representing the tuple
			for(i = 1,buf[0]=0; i<= tupdesc->natts; i++){
				snprintf(buf + strlen(buf),
						sizeof(buf)-strlen(buf),
						"%s(%s::%s)%s",
						SPI_fname(tupdesc,i),
						SPI_getvalue(tuple,tupdesc,i),
						SPI_gettype(tupdesc,i),
						(i == tupdesc->natts)?" ":" |");
				ereport(INFO,(errmsg("ROW: %s",buf)));
			}
		}
	}
	SPI_finish();

	pfree(command);

	PG_RETURN_INT32(proc);
}
