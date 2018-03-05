#-------------------------
#在数据库中创建自定义函数脚本

#------------------------


make clean;
make;

scp fft.so gpdba@node1:/home/gpdba/sql_prac/Greenplum/functions/c/fourier_transform/fft_parallel/distributed/;
scp fft.so gpdba@node2:/home/gpdba/sql_prac/Greenplum/functions/c/fourier_transform/fft_parallel/distributed/;
scp fft.so gpdba@node3:/home/gpdba/sql_prac/Greenplum/functions/c/fourier_transform/fft_parallel/distributed/;
scp fft.so gpdba@node5:/home/gpdba/sql_prac/Greenplum/functions/c/fourier_transform/fft_parallel/distributed/;

psql -d testDB -f fft.sql.in


