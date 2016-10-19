
#include <stdio.h>
#include "mpi.h"

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size == 1) {
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    if (rank == 0) {
        int num = 0, i;

        for (i = 1; i < size; i++) {
            MPI_Send(&num, 1, MPI_INT, i, 0, MPI_COMM_WORLD);

            MPI_Recv(&num, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("[%d] receive %d\n", rank, num);
        }
    } else {
        int num;

        MPI_Recv(&num, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("[%d] receive %d\n", rank, num);

        MPI_Send(&num, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();

    return 0;
}
