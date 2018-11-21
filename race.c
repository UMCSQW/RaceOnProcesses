/*use message passing functions to simulate 
	a race between two teams of processes.*/

#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

#define RACETAG 1
#define RETURNTAG 0

int main(int argc, char *argv[])
{
	int N = 0;
	if(argv[1] != NULL)
		N = atoi(argv[1]);
	else{
		printf("Input Error!\n ");
		return 0;
	}

	int my_rank;
    int num_procs;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank); 
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
	MPI_Status status;
	MPI_Request request;
	
	if(num_procs % 2 == 0){
		if(my_rank == 0)
			printf("number of processes should be odd!\n ");
		return 0;
	}

	double start_time;
    double stop_time_win;
	double stop_time_lose;

	int *temp = (int*)malloc(sizeof(int)*N);
	
	if(my_rank == 0){
		for(int i = 0; i < N; i++)
			temp[i] = 7;
		start_time = MPI_Wtime();
		
		int winner = 0;
		
		MPI_Isend (temp, N, MPI_INT, 1, RACETAG, MPI_COMM_WORLD,&request);
		MPI_Isend (temp, N, MPI_INT, 2, RACETAG, MPI_COMM_WORLD,&request);
		
		//waiting for winner.
		MPI_Recv(temp, N, MPI_INT, MPI_ANY_SOURCE, RETURNTAG, MPI_COMM_WORLD, &status);
		stop_time_win = MPI_Wtime();
		winner = status.MPI_SOURCE;
		
		MPI_Recv(temp, N, MPI_INT, MPI_ANY_SOURCE, RETURNTAG, MPI_COMM_WORLD, &status);
		stop_time_lose = MPI_Wtime();
		
		if(winner%2 == 0)
			printf("Even team time: %f sec\nOdd team time: %f sec\nEven team wins!",
					stop_time_win-start_time, stop_time_lose-start_time);
		else
			printf("Odd team time: %f sec\nEven team time: %f sec\nOdd team wins!",
					stop_time_win-start_time, stop_time_lose-start_time);
		
		free(temp);
	}

	if(my_rank == 1 || my_rank == 2){
		MPI_Recv(temp, N, MPI_INT, 0, RACETAG, MPI_COMM_WORLD,&status);
		MPI_Send(temp, N, MPI_INT, my_rank+2, RACETAG, MPI_COMM_WORLD);	
	}else if(my_rank == num_procs-1 || my_rank == num_procs-2){
		MPI_Recv(temp, N, MPI_INT, my_rank-2, RACETAG, MPI_COMM_WORLD, &status);
		MPI_Send(temp, N, MPI_INT, 0, RETURNTAG, MPI_COMM_WORLD);
	}else{
		MPI_Recv(temp, N, MPI_INT, my_rank-2, RACETAG, MPI_COMM_WORLD, &status);
		MPI_Send(temp, N, MPI_INT, my_rank+2, RACETAG, MPI_COMM_WORLD);	
	}

	MPI_Finalize();

    return EXIT_SUCCESS;
}