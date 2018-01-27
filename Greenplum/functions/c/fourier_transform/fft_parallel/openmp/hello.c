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

	int num_procs = omp_get_num_procs();
    ereport(INFO,(errmsg("%d",num_procs)));

	int max_threads = omp_get_max_threads();
	ereport(INFO,(errmsg("%d",max_threads)));

#pragma omp task
    {
        ereport(INFO,(errmsg("task")));
    }

#pragma omp parallel
	{
		ereport(INFO,(errmsg("parallel")));
	}

#pragma omp task
    {
        ereport(INFO,(errmsg("task")));
    }

    PG_RETURN_INT32(arg);
}
