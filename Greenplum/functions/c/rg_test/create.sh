#-------------------------
#在数据库中创建自定义函数脚本

#------------------------

make clean;
make;

scp rg.so gpadmin@node1:/home/gpadmin/sql_prac/Greenplum/functions/c/rg_test;
scp rg.so gpadmin@node2:/home/gpadmin/sql_prac/Greenplum/functions/c/rg_test;
scp rg.so gpadmin@node3:/home/gpadmin/sql_prac/Greenplum/functions/c/rg_test;
scp rg.so gpadmin@node5:/home/gpadmin/sql_prac/Greenplum/functions/c/rg_test;

psql -d testDB -f rg.sql.in


