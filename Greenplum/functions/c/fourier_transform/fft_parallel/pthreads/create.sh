make clean;

make;

scp fft_pthreads.so gpdba@node1:/home/gpdba/sql_prac/Greenplum/functions/c/fourier_transform/fft_parallel/pthreads;
scp fft_pthreads.so gpdba@node2:/home/gpdba/sql_prac/Greenplum/functions/c/fourier_transform/fft_parallel/pthreads;
scp fft_pthreads.so gpdba@node3:/home/gpdba/sql_prac/Greenplum/functions/c/fourier_transform/fft_parallel/pthreads;
scp fft_pthreads.so gpdba@node5:/home/gpdba/sql_prac/Greenplum/functions/c/fourier_transform/fft_parallel/pthreads;

psql -d testDB -f fft.sql.in
