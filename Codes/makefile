# Rule to compile serial file
serial:
	g++ TSP_Serial.cpp -o TSP_S
	./TSP_S

# Rule to compile logic test serial file
serial_logic:
	g++ TSP_Serial_Logic.cpp -o TSP_SL
	./TSP_SL

# Rule to compile parallel file
parallel_omp:
	g++ -fopenmp TSP_Parallel_omp.cpp -o TSP_P_OMP

# Rule to compile logic test parallel file
parallel_omp_logic:
	g++ -fopenmp TSP_Parallel_Logic_omp.cpp -o TSP_PL_OMP

# Rule to compile MPI parallel C++ file
parallel_mpi:
	mpic++ TSP_Parallel_mpi.cpp -o TSP_P_MPI

# Rule to run compiled MPI parallel C++ file
# type this command in terminal - replace num with number of processes to be created
# make run_parallel_mpi ARG=<num> 
run_parallel_mpi:
	mpiexec --use-hwthread-cpus -np $(ARG) ./TSP_P_MPI 

# Rule to compile logic test MPI parallel C++ file
parallel_mpi_logic:
	mpic++ TSP_Parallel_Logic_mpi.cpp -o TSP_PL_MPI

# Rule to run compiled MPI parallel C++ file
# type this command in terminal - replace num with number of processes to be created
# make run_parallel_mpi ARG=<num> 
run_parallel_mpi_logic:
	mpiexec --use-hwthread-cpus -np $(ARG) ./TSP_PL_MPI

run_mpi_script:
	./mpi_script.sh

graphs_omp:
	python3 Graphs_omp.py

graphs_mpi:
	python3 Graphs_mpi.py

sort_omp:
	python3 Metrics_Sort_omp.py

sort_mpi:
	python3 Metrics_Sort_mpi.py

# Rule to remove the executable and object files created by g++ compilation
clean:
	@rm -f TSP_S TSP_SL TSP_P_OMP TSP_PL_OMP TSP_P_MPI *.o
