
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "mpi.h"

#define ARR_SIZE 10

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size == 1) {
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    srand(time(NULL)*rank);

    float rand_nums[ARR_SIZE];
    int i;
    for (i = 0; i < ARR_SIZE; i++) {
        rand_nums[i] = rand()%100;
    }
    
    float local_sum = 0;
	
    for (i = 0; i < ARR_SIZE; i++) {
        local_sum += rand_nums[i];
    }

    printf("local sum = %f\n", local_sum);

    float global_sum;

    MPI_Reduce(&local_sum, &global_sum, 1, MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
    if (rank == 0) {
        printf("global sum = %f\n", global_sum);
    }

    return 0;


    MPI_Finalize();

    return 0;
}
