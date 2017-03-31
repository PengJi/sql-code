-- 返回集合的函数

create or replace function installed_languages()
	returns setof pg_language
as $$
begin
	return query select * from pg_language;
end;
$$ language plpgsql;

-- select * from installed_languages();
