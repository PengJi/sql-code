-- 基于视图的函数

create view running_queries as
select 
	current_timestamp - query_start as runtime,
	-- pid, -- 8.3 disabled
	usename,
	waiting,
	current_query
from pg_stat_activity
order by 1 desc
limit 10;

create or replace function running_queries(rown int,qlen int)
returns setof running_queries 
as $$
begin
	return query select
		runtime,
		usename,
		waiting,
		(case when (usename = session_user)
			or (select usesuper from pg_user where usename = session_user)
			then
				substring(current_query,1,qlen)
			else
				substring(ltrim(current_query),1,6) || ' ***'
		end) as query
	from running_queries
	order by 1 desc
	limit rown;
end;
$$ language plpgsql;

-- select * from running_queries(5,10);
