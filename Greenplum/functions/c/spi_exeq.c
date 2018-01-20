#include "postgres.h"
#include "fmgr.h"
#include "executor/spi.h"
#include "utils/builtins.h"

/*
 * spi examples
 * http://www.postgres.cn/docs/10/spi-examples.html
 *
 */

#ifdef PG_MODULE_MAGIC
PG_MODULE_MAGIC;
#endif


PG_FUNCTION_INFO_V1(execq);
Datum
execq(PG_FUNCTION_ARGS)
{
    char *command;
    int ret,cnt;
    uint64 proc;

    /* Convert given text object to a C string */
    command = text_to_cstring(PG_GETARG_TEXT_P(0));
	cnt = PG_GETARG_INT32(1);

    SPI_connect();

    ret = SPI_exec(command, cnt);

    proc = SPI_processed;
    /*
     * If some rows were fetched, print them via elog(INFO).
     */
    if (ret > 0 && SPI_tuptable != NULL)
    {
        TupleDesc tupdesc = SPI_tuptable->tupdesc;
        SPITupleTable *tuptable = SPI_tuptable;
        char buf[8192];
        uint64 j;

        for (j = 0; j < proc; j++)
        {
            HeapTuple tuple = tuptable->vals[j];
            int i;

            for (i = 1, buf[0] = 0; i <= tupdesc->natts; i++)
                snprintf(buf + strlen (buf), sizeof(buf) - strlen(buf), " %s%s",
                        SPI_getvalue(tuple, tupdesc, i),
                        (i == tupdesc->natts) ? " " : " |");
            //elog(INFO, "EXECQ: %s", buf);
			ereport(INFO,(errmsg("ROW: %s",buf)));
        }
    }

    SPI_finish();
    pfree(command);

	PG_RETURN_INT32(proc);
}

/*
编译
cc -fpic -c spi_exeq.c -I/home/gpdba/greenplum/include/postgresql/server;
cc -shared -o spi_exeq.so spi_exeq.o;

申明函数
CREATE or replace FUNCTION execq(text, integer) RETURNS int8
    AS '/home/gpdba/sql_prac/Greenplum/functions/c/spi_exeq','execq'
	    LANGUAGE C STRICT;
*/
