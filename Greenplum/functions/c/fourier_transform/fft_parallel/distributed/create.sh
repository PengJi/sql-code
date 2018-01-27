make clean;
make;

scp fft.so gpdba@node1:/home/gpdba/sql_prac/Greenplum/functions/c/fourier_transform/fft_parallel/distributed/;
scp fft.so gpdba@node2:/home/gpdba/sql_prac/Greenplum/functions/c/fourier_transform/fft_parallel/distributed/;
scp fft.so gpdba@node3:/home/gpdba/sql_prac/Greenplum/functions/c/fourier_transform/fft_parallel/distributed/;
scp fft.so gpdba@node5:/home/gpdba/sql_prac/Greenplum/functions/c/fourier_transform/fft_parallel/distributed/;


psql -d testDB -c "
create or replace function hello(integer)
returns int
as '/home/gpdba/sql_prac/Greenplum/functions/c/fourier_transform/fft_parallel/distributed/fft', 'hello'
language c strict;
"


