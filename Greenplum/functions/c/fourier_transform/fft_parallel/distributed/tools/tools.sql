-- 建立外部表分析日志
nohup gpfdist -d /home/gpadmin/gpdata/gpseg-1/pg_log -p 8081 &

gpfdist -d /home/gpadmin/gpdata/gpseg-1/pg_log -p 8081 


CREATE READABLE EXTERNAL TABLE pg_log(rec_time varchar,username varchar,databasename varchar,proc_id varchar,
	th_id varchar,from_id varchar,no1 varchar,ora_time varchar,no2 varchar,no3 varchar,no4 varchar,master_id varchar,
	no5 varchar,no6 varchar,no7 varchar,no8 varchar,sql_type varchar,no9 varchar,statement varchar,no10 varchar,
	no11 varchar,no12 varchar,no13 varchar,no14 varchar,sql_text varchar,no15 varchar,no16 varchar,no17 varchar,
	no18 varchar,no19 varchar ) 
LOCATION ('gpfdist://192.168.1.112:8081/*.csv') FORMAT 'CSV' (DELIMITER AS ',') ENCODING 'utf-8'; 

-- 测试
select rec_time,username ,databasename,proc_id,th_id,from_id,ora_time,master_id,sql_type,statement,sql_text 
from pg_log 
where statement like 'statement: %insert%zht%' 
and statement not like '%select sql_text from pg_log where statement like%' order by rec_time;
