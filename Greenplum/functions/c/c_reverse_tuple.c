#include "postgres.h"
#include "fmgr.h"
#include "utils/array.h"
#include "catalog/pg_type.h"

/*
 * 返回复杂类型的单个元组
 *
 */
Datum
c_reverse_tuple(PG_FUNCTION_ARGS){
	HeapTupleHeader th;
	int32 a,b,c;
	bool aisnull,bisnull,cisnull;

	TupleDesc resultTupleDesc;
	oid resultTypeId;
	Datum retvals[4];
	bool retnulls[4];
	HeapTuple rettuple;

	//get the tuple header of 1st argument
	th = PG_GETARG_HEAPTUPLEHEADER(0);
	//get argument Datum's and convert them to int32
	a = DatumGetInt32(GetAttributeByName(th,"a",&aisnull));
	b = DatumGetInt32(GetAttributeByName(th,"b",&bisnull));
	c = DatumGetInt32(GetAttributeByName(th,"c",&cisnull));

	//debug: report the extracted field values
	ereport(INFO,(errmsg("arg: (a: %d,b: %d,c: %d)",a,b,c)));

	//set up tuple descriptos for result info
	get_call_result_type(fcinfo,&resultTypeId,&resultTupleDesc);
	//check that SQL function definition is set up to return arecord
	Assert(resultTypeId == TYPEFUNC_COMPOSITE);
	//make the tuple descriptor known to postgres as valid return type
	BlessTupleDesc(resultTupleDesc);

	retvals[0] = Int32GetDatum(c);
	retvals[1] = Int32GetDatum(b);
	retvals[2] = Int32GetDatum(a);
	retvals[3] = Int32GetDatum(retvals[0]*resvals[1]+retvals[2]);

	retnulls[0] = aisnull;
	retnulls[1] = bisnull;
	retnulls[2] = cisnull;
	retnulls[3] = aisnull || bisnull || cisnull;

	rettuple = heap_form_tuple(resultTupleDesc,retvals,retnulls);

	PG_RETURN_DATUM(HeapTupleGetDatum(rettuple));
}
