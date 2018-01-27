#include <omp.h>

#include "postgres.h"
#include "funcapi.h"

#ifdef PG_MODULE_MAGIC
PG_MODULE_MAGIC;
#endif

int test(){
#pragma omp parallel num_threads(2)
    {   
        ereport(INFO,(errmsg("test num_threads")));
        ereport(INFO,(errmsg("Thread ID: %d",omp_get_thread_num())));
    }

	return 0;
}

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

	omp_set_num_threads(10);
#pragma omp parallel
	{
		ereport(INFO,(errmsg("parallel")));
	}

#pragma omp task
    {
        ereport(INFO,(errmsg("task")));
    }

#pragma omp parallel num_threads(2)
    {
        ereport(INFO,(errmsg("num_threads")));
        ereport(INFO,(errmsg("Thread ID: %d",omp_get_thread_num())));
    }
	
	test();

    PG_RETURN_INT32(arg);
}
