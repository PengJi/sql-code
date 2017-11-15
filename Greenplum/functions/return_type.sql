-- 简单函数
create or replace function add_one(i int)
	returns int 
as $$
	return i+1;
$$ language plpythonu;

-- 返回一个记录
create or replace function userinfo(
	inout username name,
	out user_id oid,
	out is_superuser boolean)
as $$
	class PGUser:
		def __init__(self,username,user_id,is_superuser):
			self.username = username
			self.user_id = user_id
			self.is_superuser = is_superuser
	
	u = plpy.execute("""select username,usesysid,usesuper
			from pg_user
			where username = '%s'""" % username)[0];

	user = PGUser(u['username'],u['usersysid'],u['usesuper'])
	return user
$$ language plpythonu;

-- 更简单的返回方式
create or replace function userinfo(
	inout username name,
	out user_id oid,
	out is_superuser boolean)
as $$
	u = plpy.execute("""
		select username,usesysid,usessuper
		from pg_user
		where usename = '%s'""" % username)[0]

	return {'username':u['username'],'user_id':u['usesysid'],'is_superuser':u['usesuper']}
$$ language plpythonu;

-- 使用三元组
