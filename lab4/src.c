
#include <stdio.h>
#include "mpi.h"

#define EXIT_NUM 0

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size < 2) {
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    if (rank == 0) {
        // root process is sender
        sender(rank, rank+1);
    } else if (rank == size-1) {
        // last process is receiver
        receiver(rank, rank-1);
    } else {
        // other procs are linkers
        linker(rank, rank-1, rank+1);
    }

    MPI_Finalize();

    return 0;
}

int sender(int rank, int dst) {
    int number;

    printf("sender [%d] starts\n", rank);

    while (1) {
        MPI_Barrier(MPI_COMM_WORLD);

        printf("[%d] enter number (%d to exit):\n", rank, EXIT_NUM);
        scanf("%d", &number);
        printf("PIPE INPUT: %d:\n", number);

        printf("[%d] sending %d to [%d]\n", rank, number, dst);
        MPI_Send(&number, 1, MPI_INT, dst, 0, MPI_COMM_WORLD);

        if (number == EXIT_NUM) {
            printf("[%d] received EXIT_NUM\n", rank);
            break;
        }
    }

    printf("sender [%d] stops\n", rank);

    return 0;
}

int linker(int rank, int src, int dst) {
    int number;

    printf("linker [%d] starts\n", rank);

    while (1) {
        MPI_Barrier(MPI_COMM_WORLD);

        MPI_Recv(&number, 1, MPI_INT, src, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("[%d] received %d from [%d]\n", rank, number, src);

        printf("[%d] sending %d to [%d]\n", rank, number, dst);
        MPI_Send(&number, 1, MPI_INT, dst, 0, MPI_COMM_WORLD);

        if (number == EXIT_NUM) {
            printf("[%d] received EXIT_NUM\n", rank);
            break;
        }
    }

    printf("linker [%d] stops\n", rank);

    return 0;
}

int receiver(int rank, int src) {
    int number;

    printf("receiver [%d] starts\n", rank);

    while (1) {
        MPI_Barrier(MPI_COMM_WORLD);

        MPI_Recv(&number, 1, MPI_INT, src, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("[%d] received %d from [%d]\n", rank, number, src);

        if (number == EXIT_NUM) {
            printf("[%d] received EXIT_NUM\n", rank);
            break;
        } else {
            printf("[%d] do something with %d\n", rank, number);
            doStuff(number);
        }
    }

    printf("receiver [%d] stops\n", rank);

    return 0;
}

int doStuff(int number) {
    return 0;
}

