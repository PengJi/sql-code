create or replace function installed_languages()
	returns setof pg_language
as $$
begin
	return query select * from pg_language;
end;
$$ language plpgsql;
