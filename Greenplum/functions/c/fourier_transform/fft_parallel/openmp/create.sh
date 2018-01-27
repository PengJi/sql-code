make clean;

make;

scp fft_openmp.so gpdba@node1:/home/gpdba/sql_prac/Greenplum/functions/c/fourier_transform/fft_parallel/openmp;
scp fft_openmp.so gpdba@node2:/home/gpdba/sql_prac/Greenplum/functions/c/fourier_transform/fft_parallel/openmp;
scp fft_openmp.so gpdba@node3:/home/gpdba/sql_prac/Greenplum/functions/c/fourier_transform/fft_parallel/openmp;
scp fft_openmp.so gpdba@node5:/home/gpdba/sql_prac/Greenplum/functions/c/fourier_transform/fft_parallel/openmp;

psql -d testDB -f fft.sql.in
