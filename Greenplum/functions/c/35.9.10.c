#include "postgres.h"
#include "executor/executor.h"  /* for GetAttributeByName() */
#include "funcapi.h"
#include "utils/array.h"

/**
 * 多态参数和返回类型
 * http://www.postgres.cn/docs/9.4/xfunc-c.html
 */

PG_FUNCTION_INFO_V1(make_array);
Datum
make_array(PG_FUNCTION_ARGS)
{
    ArrayType  *result;
    Oid         element_type = get_fn_expr_argtype(fcinfo->flinfo, 0);
    Datum       element;
    bool        isnull;
    int16       typlen;
    bool        typbyval;
    char        typalign;
    int         ndims;
    int         dims[MAXDIM];
    int         lbs[MAXDIM];

    if (!OidIsValid(element_type))
        elog(ERROR, "could not determine data type of input");


    /* 获取提供的元素(要小心其为NULL的情况) */
    isnull = PG_ARGISNULL(0);
    if (isnull)
        element = (Datum) 0;
    else
        element = PG_GETARG_DATUM(0);
    
    /* 维数是1 */
    ndims = 1;
    /* 有1个元素 */
    dims[0] = 1;
    /* 数组下界是1 */

    lbs[0] = 1;

    /* 获取有关元素类型需要的信息 */
    get_typlenbyvalalign(element_type, &typlen, &typbyval, &typalign);

    /* 然后制作数组 */ 
    result = construct_md_array(&element, &isnull, ndims, dims, lbs,
                                element_type, typlen, typbyval, typalign);

    PG_RETURN_ARRAYTYPE_P(result);
}

/*
申明函数
CREATE FUNCTION make_array(anyelement) RETURNS anyarray
    AS 'DIRECTORY/funcs', 'make_array'
    LANGUAGE C IMMUTABLE;
*/
