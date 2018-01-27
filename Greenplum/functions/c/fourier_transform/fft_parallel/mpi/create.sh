make clean;

make;

scp fft_mpi.so gpdba@node1:/home/gpdba/sql_prac/Greenplum/functions/c/fourier_transform/fft_parallel/mpi;
scp fft_mpi.so gpdba@node2:/home/gpdba/sql_prac/Greenplum/functions/c/fourier_transform/fft_parallel/mpi;
scp fft_mpi.so gpdba@node3:/home/gpdba/sql_prac/Greenplum/functions/c/fourier_transform/fft_parallel/mpi;
scp fft_mpi.so gpdba@node5:/home/gpdba/sql_prac/Greenplum/functions/c/fourier_transform/fft_parallel/mpi;


psql -d testDB -f fft.sql.in
