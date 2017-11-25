#include "postgres.h"
#include "fmgr.h"
#include "utils/array.h"
#include "catalog/pg_type.h"

PG_MODULE_MAGIC;

/*
 * 基本加法
 */
PG_FUNCTION_INFO_V1(add_ab);
Datum 
add_ab(PG_FUNCTION_ARGS){
	int32 arg_a = PG_GETARG_INT32(0);
	int32 arg_b = PG_GETARG_INT32(1);

	PG_RETURN_INT32(arg_a + arg_b);
}

/*
 * 处理NULL参数
 * 1. 当出现任一参数为NULL时，确保参数仍被调用；
 * 2. 有效处理NULL参数，将NULL参数转换为0，当两个参数均为空时返回NULL
 */
PG_FUNCTION_INFO_V1(add_ab_null);
Datum
add_ab_null(PG_FUNCTION_ARGS){
	int32 not_null = 0;
	int32 sum = 0;

	if(!PG_ARGISNULL(0)){
		sum += PG_GETARG_INT32(0);
		not_null = 1;
	}
	if(!PG_ARGISNULL(1)){
		sum += PG_GETARG_INT32(1);
		not_null = 1;
	}
	if(not_null){
		PG_RETURN_INT32(sum);
	}
	PG_RETURN_NULL();
}

/*
 * 在参数数组中累加所有非空元素
 *
 */
PG_FUNCTION_INFO_V1(add_int32_array);
Datum
add_int32_array(PG_FUNCTION_ARGS){
	ArrayType *input_array;

	int32 sum = 0;
	bool not_null = false;
	Datum *datums;
	bool *nulls;
	int count;
	int i;

	input_array = PG_GETARG_ARRAYTYPE_P(0);
	Assert(ARR_ELEMTYPE(input_array) == INT4OID);

	if(ARR_NDIM(input_array) > 1)
		ereport(ERROR,
				(errcode(ERRCODE_ARRAY_SUBSCRIPT_ERROR),
				 errmsg("1-dimensional array needed")));

	deconstruct_array(input_array,
					INT4OID,
					4,
					true,
					'i',
					&datums,&nulls,&count);

	for(i=0;i<count;i++){
		if(nulls[i])
			continue;

		sum += DatumGetInt32(datums[i]);
		not_null = true;
	}

	if(not_null)
		PG_RETURN_INT32(sum);
	PG_RETURN_NULL();
}

