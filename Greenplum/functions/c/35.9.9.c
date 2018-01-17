#include "postgres.h"
#include "executor/executor.h"  /* for GetAttributeByName() */
#include "funcapi.h"

PG_MODULE_MAGIC;

/**
 * 返回集合(多行)
 * http://www.postgres.cn/docs/9.4/xfunc-c.html
 */

PG_FUNCTION_INFO_V1(retcomposite);

Datum
retcomposite(PG_FUNCTION_ARGS)
{
    FuncCallContext     *funcctx;
    int                  call_cntr;
    int                  max_calls;
    TupleDesc            tupdesc;
    AttInMetadata       *attinmeta;

    /* 只是在第一次调用函数的时候干的事情 */
    if (SRF_IS_FIRSTCALL())
    {
        MemoryContext   oldcontext;

        /* 创建一个函数环境，用于在调用间保持住 */
        funcctx = SRF_FIRSTCALL_INIT();

        /* 切换到适合多次函数调用的内存环境 */
        oldcontext = MemoryContextSwitchTo(funcctx->multi_call_memory_ctx);

        /* 要返回的行总数 */
        funcctx->max_calls = PG_GETARG_UINT32(0);

        /* 为了结果类型制作一个行描述 */
        if (get_call_result_type(fcinfo, NULL, &tupdesc) != TYPEFUNC_COMPOSITE)
            ereport(ERROR,
                    (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                     errmsg("function returning record called in context "
                            "that cannot accept type record")));
        
        /*
         * 生成稍后从裸 C 字符串生成行的属性元数据
         */
        attinmeta = TupleDescGetAttInMetadata(tupdesc);
        funcctx->attinmeta = attinmeta;

        MemoryContextSwitchTo(oldcontext);
    }

    /* 每次函数调用都要做的事情 */
    funcctx = SRF_PERCALL_SETUP();

    call_cntr = funcctx->call_cntr;
    max_calls = funcctx->max_calls;
    attinmeta = funcctx->attinmeta;
  
    if (call_cntr < max_calls)    /* 在还有需要发送的东西时继续处理 */
    {
        char       **values;
        HeapTuple    tuple;
        Datum        result;

        /*
         * 准备一个数值数组用于版本的返回行
         * 它应该是一个C字符串数组，稍后可以被合适的类型输入函数处理。
         */
        values = (char **) palloc(3 * sizeof(char *));
        values[0] = (char *) palloc(16 * sizeof(char));
        values[1] = (char *) palloc(16 * sizeof(char));
        values[2] = (char *) palloc(16 * sizeof(char));

        snprintf(values[0], 16, "%d", 1 * PG_GETARG_INT32(1));
        snprintf(values[1], 16, "%d", 2 * PG_GETARG_INT32(1));
        snprintf(values[2], 16, "%d", 3 * PG_GETARG_INT32(1));

        /* 制作一个行 */
        tuple = BuildTupleFromCStrings(attinmeta, values);

        /* 把行做成 datum */
        result = HeapTupleGetDatum(tuple);

        /* 清理(这些实际上并非必要) */
        pfree(values[0]);
        pfree(values[1]);
        pfree(values[2]);
        pfree(values);

        SRF_RETURN_NEXT(funcctx, result);
    }
    else    /* 在没有数据残留的时候干的事情 */
    {
        SRF_RETURN_DONE(funcctx);
    }
}

/*
cc -fpic -c 35.9.9.c -I/home/gpdba/greenplum/include/postgresql/server;
cc -shared -o 35.9.9.so 35.9.9.o;

申明函数
CREATE TYPE __retcomposite AS (f1 integer, f2 integer, f3 integer);

CREATE OR REPLACE FUNCTION retcomposite(integer, integer)
    RETURNS SETOF __retcomposite
    AS '/home/gpdba/sql_prac/Greenplum/functions/c/35.9.9', 'retcomposite'
    LANGUAGE C IMMUTABLE STRICT;

另一种方法
CREATE OR REPLACE FUNCTION retcomposite(IN integer, IN integer,
    OUT f1 integer, OUT f2 integer, OUT f3 integer)
    RETURNS SETOF record
    AS '/home/gpdba/sql_prac/Greenplum/functions/c/35.9.9', 'retcomposite'
    LANGUAGE C IMMUTABLE STRICT;
*/
