-- 返回记录集

create or replace function permutations(inout a int,inout b int,inout c int)
returns setof record
as $$
begin
	return next;
	select b,c into c,b; return next;
	select a,b into b,a; return next;
	select b,c into c,b; return next;
	select a,b into b,a; return next;
	select b,c into c,b; return next;
end
$$ language plpgsql;

-- select * from permutations(1,2,3);
