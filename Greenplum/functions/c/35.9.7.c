#include "postgres.h"
#include "executor/executor.h"  /* for GetAttributeByName() */

/**
 * 复合类型参数
 * http://www.postgres.cn/docs/9.4/xfunc-c.html
 */

#ifdef PG_MODULE_MAGIC
PG_MODULE_MAGIC;
#endif

PG_FUNCTION_INFO_V1(c_overpaid);

Datum
c_overpaid(PG_FUNCTION_ARGS)
{
    HeapTupleHeader  t = PG_GETARG_HEAPTUPLEHEADER(0);
    int32            limit = PG_GETARG_INT32(1);
    bool isnull;
    Datum salary;

    salary = GetAttributeByName(t, "salary", &isnull);
    if (isnull)
        PG_RETURN_BOOL(false);

    /* 另外，可能更希望将PG_RETURN_NULL()用在null薪水上 */

    PG_RETURN_BOOL(DatumGetInt32(salary) > limit);
}

/**
 * GetAttributeByName 是 PostgreSQL系统函数，用来返回当前记录的字段。
 * 它有三个参数：
 * 类型为HeapTupleHeader的传入函数的参数、字段名称、一个确定字段是否为NULL的返回参数。
 * GetAttributeByName 函数返回一个Datum值，可以用对应的DatumGetXXX()宏把它转换成合适的数据类型。
 */

/*
申明该函数
create function c_overpaid(emp,integer) returns boolean
AS 'DIRECTORY/funcs', 'c_overpaid'
LANGUAGE C STRICT;
*/
