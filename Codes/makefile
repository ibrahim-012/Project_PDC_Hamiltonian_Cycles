# Rule to compile serial file
serial:
	g++ TSP_Serial.cpp -o TSP_S

# Rule to compile logic test serial file
serial_logic:
	g++ TSP_Serial_Logic.cpp -o TSP_SL

# Rule to compile parallel file
parallel_omp:
	g++ -fopenmp TSP_Parallel_omp.cpp -o TSP_P_OMP

# Rule to compile logic test parallel file
parallel_omp_logic:
	g++ -fopenmp TSP_Parallel_Logic_omp.cpp -o TSP_PL_OMP

graphs_omp:
	python3 Graphs_omp.py

sort_omp:
	python3 Metrics_Sort_omp.py

# Rule to remove the executable and object files created by g++ compilation
clean:
	@rm -f TSP_S TSP_SL TSP_P_OMP TSP_PL_OMP *.o
