CREATE TABLE neighbors(
	objID bigint NOT NULL,
	NeighborObjID bigint NOT NULL,
	distance real NULL,
	type smallint NOT NULL,
	neighborType smallint NOT NULL,
	mode smallint NOT NULL,
	neighborMode SMALLINT NOT NULL,
	null1 char,
	null2 char,
	null3 char,
	null4 char
)
-- 普通表
-- appendonly表
-- with (appendonly=TRUE)
-- with (appendonly=TRUE,compresslevel=5)
distributed by(objID);

-- 删除表
drop table neighbors;

-- 导入表
copy Neighbors from '/home/gpadmin/load/comma/Neighbors.csv' with DELIMITER ',' csv header;
-- linux shell comma
/usr/bin/time -v -o runNei.txt psql -d astronomy -c "copy neighbors from '/home/gpadmin/load/comma/neighbors1000.csv' with DELIMITER ',' csv header;" 
-- linux shell tab
/usr/bin/time -v -o runNei.txt psql -d astronomy -c "copy neighbors from '/home/gpadmin/load/tab/10G/NeighborsRandom10.csv' with DELIMITER '\t';"  

-- SciDB
CREATE array neighbors<
	objID : int64 NOT NULL,
	NeighborObjID : int64 NOT NULL,
	distance : double NULL,
	type : int16 NOT NULL,
	neighborType : int16 NOT NULL,
	mode : int16 NOT NULL,
	neighborMode : int16 NOT NULL,
	null1 : char,
	null2 : char,
	null3 : char,
	null4 : char
>[i];