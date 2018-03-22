#-------------------------
#在数据库中创建自定义函数脚本

#------------------------

make clean;
make;

scp rg.so gpdba@node1:/home/gpdba/sql_prac/Greenplum/functions/c/rg_test;
scp rg.so gpdba@node2:/home/gpdba/sql_prac/Greenplum/functions/c/rg_test;
scp rg.so gpdba@node3:/home/gpdba/sql_prac/Greenplum/functions/c/rg_test;
scp rg.so gpdba@node5:/home/gpdba/sql_prac/Greenplum/functions/c/rg_test;

psql -d testDB -f rg.sql.in


