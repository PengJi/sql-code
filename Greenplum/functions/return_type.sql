-- 创建扩展
create language plpythonu;

-- 简单函数
create or replace function add_one(i int)
	returns int 
as $$
	return i+1;
$$ language plpythonu;

-- 返回记录
-- 1.使用一个实例
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
	
	u = plpy.execute("""select usename,usesysid,usesuper
			from pg_user
			where usename = '%s'""" % username)[0]; 

	user = PGUser(u['usename'],u['usesysid'],u['usesuper'])
	return user
$$ language plpythonu;
--后面的[0]是为了提取结果中的第一行，因为plpy.execute会返回一张结果列表]

-- 2.使用字典
create or replace function userinfo(
	inout username name,
	out user_id oid,
	out is_superuser boolean)
as $$
	u = plpy.execute("""
		select usename,usesysid,usesuper
		from pg_user
		where usename = '%s'""" % username)[0]

	return {'username':u['usename'],'user_id':u['usesysid'],'is_superuser':u['usesuper']}
$$ language plpythonu;

-- 3.使用三元组
create or replace function userinfo(
	inout username name,
	out user_id oid,
	out is_superuser boolean)
as $$
	u = plpy.execute("""
		select usename,usesysid,usesuper
		from pg_user
		where usename = '%s'""" % username)[0]

	return (u['usename'],u['usesysid'],u['usesuper'])
$$ language plpythonu;

-- 调用 select * from userinfo('gpadmin');

-- 返回一个集合
-- 生成所有偶数
-- 1.返回一列整数
create or replace function even_number_from_list(up_to int)
	returns setof int
as $$
	return range(0,up_to,2);
$$ language plpythonu;

-- 2.返回表
create or replace function even_number_from_generator(up_to int)
	returns table(even int,odd int)
as $$
	return ((i,i+1) for i in xrange(0,up_to,2))
$$ language plpythonu;

-- 3.返回集合
create or replace function even_numbers_with_yield(up_to int, out even int,out odd int)
	returns setof record
as $$
	for i in xrange(0,up_to,2):
		yield i,i+1
$$ language plpythonu;

-- 3.对于集合中的任意一行，可以返回不同类型
create or replace function birthdates(out name text,out birthdate date)
	returns setof record
as $$
	return (
		{'name':'bob','birthdate':'1980-1-1'},
		{'name':'mary','birthdate':'1990-1-1'},
	);
$$ language plpythonu;

