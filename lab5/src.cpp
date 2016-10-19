
#include <cmath>
#include <cstdlib>
#include <iostream>
#include "mpi.h"

#define MATRIX_LENGTH 16
#define BLOCK_LENGTH 2
#define BLOCK_COUNT MATRIX_LENGTH*MATRIX_LENGTH/BLOCK_LENGTH/BLOCK_LENGTH
#define TAG 1

using namespace std;

int main(int argc, char *argv[]) {
    
    int A[MATRIX_LENGTH][MATRIX_LENGTH] = {},
        B[MATRIX_LENGTH][MATRIX_LENGTH] = {};

    srand(time(0));

    for (int i = 0; i < MATRIX_LENGTH; i++) {
        for (int j = 0; j < MATRIX_LENGTH; j++) {
            A[i][j] = (int)rand() % 10 + 1;
            B[i][j] = (int)rand() % 10 + 1;
        }
    }

    MPI_Status  statRecv2;
    MPI_Request reqSend2, reqRecv2;
    MPI_Status  statRecv[2];
    MPI_Request reqSend[2], reqRecv[2];

    MPI_Init(&argc, &argv);

    int rank, numprocs;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);

    int pra[MATRIX_LENGTH / BLOCK_LENGTH][MATRIX_LENGTH / BLOCK_LENGTH] = {},
        psa[MATRIX_LENGTH / BLOCK_LENGTH][MATRIX_LENGTH / BLOCK_LENGTH] = {};
    int prb[MATRIX_LENGTH / BLOCK_LENGTH][MATRIX_LENGTH / BLOCK_LENGTH] = {},
        psb[MATRIX_LENGTH / BLOCK_LENGTH][MATRIX_LENGTH / BLOCK_LENGTH] = {};

    int C[MATRIX_LENGTH][MATRIX_LENGTH] = {};

    for (int i = 0; i < MATRIX_LENGTH / BLOCK_LENGTH; i++) {
        for (int j = 0; j < MATRIX_LENGTH / BLOCK_LENGTH; j++) {
            int row = rank / BLOCK_LENGTH * (MATRIX_LENGTH / BLOCK_LENGTH) + i;
            int col = rank % BLOCK_LENGTH * (MATRIX_LENGTH / BLOCK_LENGTH) + j;
            psa[i][j] = A[row][col];
            psb[i][j] = B[row][col];
        }
    }

    double np = numprocs;
    double kk = sqrt(np);
    int k = (int)kk;

    int right_proc, left_proc, top_proc, bottom_proc;

    if (rank < k) {
        left_proc = (rank + k - 1) % k;
        right_proc = (rank + k + 1) % k;
        top_proc = ((k - 1)*k) + rank;
    }

    if (rank == k) {
        left_proc = ((rank + k - 1) % k) + k;
        right_proc = ((rank + k + 1) % k) + k;
        top_proc = rank - k;
    }

    if (rank > k) {
        int x = rank / k;
        left_proc = ((rank + k - 1) % k) + x * k;
        right_proc = ((rank + k + 1) % k) + x * k;
        top_proc = rank - k;
    }

    if (rank == 0 || (rank / k) < (k - 1)) {
        bottom_proc = rank + k;
    }

    if ((rank / k) == (k - 1)) {
        bottom_proc = rank - ((k - 1)*k);
    }

    int p = 0;
    do {
        if (p < rank / BLOCK_LENGTH) {
            MPI_Irecv(pra, BLOCK_COUNT, MPI_FLOAT, right_proc, TAG,
                MPI_COMM_WORLD, &reqRecv2);

            MPI_Isend(psa, BLOCK_COUNT, MPI_FLOAT, left_proc, TAG,
                MPI_COMM_WORLD, &reqSend2);

            MPI_Wait(&reqRecv2, &statRecv2);

            for (int i = 0; i < MATRIX_LENGTH / BLOCK_LENGTH; i++) {
                for (int j = 0; j < MATRIX_LENGTH / BLOCK_LENGTH; j++) {
                    psa[i][j] = pra[i][j];
                }
            }
        }

        MPI_Barrier(MPI_COMM_WORLD);

        if (p < rank % BLOCK_LENGTH) {
            MPI_Irecv(prb, BLOCK_COUNT, MPI_FLOAT, bottom_proc, TAG,
                MPI_COMM_WORLD, &reqRecv2);

            MPI_Isend(psb, BLOCK_COUNT, MPI_FLOAT, top_proc, TAG,
                MPI_COMM_WORLD, &reqSend2);

            MPI_Wait(&reqRecv2, &statRecv2);

            for (int i = 0; i < MATRIX_LENGTH / BLOCK_LENGTH; i++) {
                for (int j = 0; j < MATRIX_LENGTH / BLOCK_LENGTH; j++) {
                    psb[i][j] = prb[i][j];
                }
            }
        }

        MPI_Barrier(MPI_COMM_WORLD);
        p++;

    } while (p < MATRIX_LENGTH);

    for (int kkk = 0; kkk < BLOCK_LENGTH; kkk++) {
        for (int i = 0; i < MATRIX_LENGTH / BLOCK_LENGTH; i++) {
            for (int j = 0; j < MATRIX_LENGTH / BLOCK_LENGTH; j++) {
                for (int k = 0; k < MATRIX_LENGTH / BLOCK_LENGTH; k++) {
                    C[i][j] += psa[i][k] * psb[k][j];
                }
            }
        }

        MPI_Irecv(pra, BLOCK_COUNT, MPI_FLOAT, right_proc, TAG,
            MPI_COMM_WORLD, reqRecv);

        MPI_Irecv(prb, BLOCK_COUNT, MPI_FLOAT, bottom_proc, TAG,
            MPI_COMM_WORLD, &reqRecv[1]);

        MPI_Isend(psa, BLOCK_COUNT, MPI_FLOAT, left_proc, TAG,
            MPI_COMM_WORLD,reqSend);

        MPI_Isend(psb, BLOCK_COUNT, MPI_FLOAT, top_proc, TAG,
            MPI_COMM_WORLD, &reqSend[1]);

        MPI_Wait(reqRecv, statRecv);
        MPI_Barrier(MPI_COMM_WORLD);

        for (int i = 0; i < MATRIX_LENGTH / BLOCK_LENGTH; i++) {
            for (int j = 0; j < MATRIX_LENGTH / BLOCK_LENGTH; j++) {
                psa[i][j] = pra[i][j];
            }
        }

        for (int i = 0; i < MATRIX_LENGTH / BLOCK_LENGTH; i++) {
            for (int j = 0; j < MATRIX_LENGTH / BLOCK_LENGTH; j++) {
                psb[i][j] = prb[i][j];
            }
        }
    }

    cout << "rank " << rank << ": ";

    for (int i = 0; i < MATRIX_LENGTH / BLOCK_LENGTH; i++) {
        for (int j = 0; j < MATRIX_LENGTH / BLOCK_LENGTH; j++) {
            cout << C[i][j] << " ";
        }
    }

    cout << endl;

    MPI_Finalize();

    return 0;
}
