
#include <stdio.h>
#include <stdlib.h>
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
        printf("comm size = %d\n", size);
    }

    MPI_Barrier(MPI_COMM_WORLD);

    printf("starting proc rank = %d\n", rank);

    MPI_Barrier(MPI_COMM_WORLD);

    // common vars
    int buff_size = ipow(2, size-2);
    int *buff = (int *) malloc(buff_size*sizeof(int));
    int i;
    MPI_Status status;
    int received;

    if (rank == 0) {
        int arr_size = ipow(2, size-1) - 1;
        int *arr = (int *) malloc(arr_size*sizeof(int));

        printf("arr_size = %d\n", arr_size);

        // generate int array
        for (i = 0; i < arr_size; i++) {
            arr[i] = rand();
            printf("arr[%d] = %d\n", i, arr[i]);
        }

        printf("proc seq length = %d\n", size-1);

        // generate proc seq
        int *procs = (int *) malloc((size-1)*sizeof(int));
        for (i = 0; i < size-1; i++) {;
            procs[i] = i+1;
            printf("procs[%d] = %d\n", i, procs[i]);
        }

        // mix proc seq
        if (size > 2) {
            for (i = 0; i < size*10; i++) {
                int m, n, tmp;
                m = rand()%(size-1);
                n = rand()%(size-1);
                if (m != n) {
                    tmp = procs[m];
                    procs[m] = procs[n];
                    procs[n] = tmp;
                }
            }
        }

        // sending
        printf("sending...\n");
        int *curr = arr;
        for (i = size-2; i >= 0; i--) {
            int count = ipow(2, i);
            printf("send %d elements to %d proc\n", count, procs[i]);
            MPI_Send(curr, count, MPI_INT, procs[i], 0, MPI_COMM_WORLD);
            curr += count;
        }

        printf("old: ");
        printlnarr(arr, arr_size);

        // receive result
        for (i = 0; i < size-1; i++) {
            MPI_Recv(buff, buff_size, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
            MPI_Get_count(&status, MPI_INT, &received);

            printf("root receives response with %d nums\n", received);

            curr = arr + arr_size + 1 - 2 * received;
            int j;
            for (j = 0; j < received; j++) {
                curr[j] = buff[j];
            }
        }

        printf("new: ");
        printlnarr(arr, arr_size);

        free(procs);
        free(arr);
    } else {
        MPI_Recv(buff, buff_size, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        MPI_Get_count(&status, MPI_INT, &received);

        printf("proc num %d receive %d numbers: ", rank, received);
        printlnarr(buff, received);

        for (i = 0; i < received; i++) {
            buff[i] = dostuff(buff[i]);
        }

        printf("proc %d sends result\n", rank);

        MPI_Send(buff, received, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    free(buff);

    MPI_Finalize();

    return 0;
}

int dostuff(int n) {
    return n%100;
}

int ipow(int base, int exp) {
    int result = 1;

    while (exp) {
        if (exp & 1) {
            result *= base;
        }

        exp >>=1;
        base *= base;
    }

    return result;
}

int printlnarr(int *arr, int arr_size) {
    printf("[ ");

    if (arr_size > 0) {
        int i;
        for (i = 0; i < arr_size; i++) {
            printf("%d ", arr[i]);
        }
    } else {
        printf("empty ");
    }

    printf("]\n");

    return arr_size;
}
