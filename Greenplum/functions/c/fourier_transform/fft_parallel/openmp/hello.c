#include <omp.h>

#include "postgres.h"
#include "funcapi.h"

#ifdef PG_MODULE_MAGIC
PG_MODULE_MAGIC;
#endif

PG_FUNCTION_INFO_V1(hello);
Datum 
hello(PG_FUNCTION_ARGS)
{
	int32 arg = PG_GETARG_INT32(0);
	int num = omp_get_num_procs();

//#pragma omp parallel  
    {
        ereport(INFO,(errmsg("hello")));
    }

    PG_RETURN_INT32(arg);
}
