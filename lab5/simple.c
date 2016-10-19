
#include <stdio.h>
#include <stdlib.h>

#define MATRIX_LENGTH 17

int main(int argc, char *argv[]) {
    int i, j, k;

    srand(time(0));

    int A[MATRIX_LENGTH][MATRIX_LENGTH] = {},
        B[MATRIX_LENGTH][MATRIX_LENGTH] = {},
        C[MATRIX_LENGTH][MATRIX_LENGTH] = {};

    for (i = 0; i < MATRIX_LENGTH; i++) {
        for (j = 0; j < MATRIX_LENGTH; j++) {
            A[i][j] = (int)rand() % 10;
            B[i][j] = (int)rand() % 10;
        }
    }

    for (i = 0; i < MATRIX_LENGTH; i++) {
        for (j = 0; j < MATRIX_LENGTH; j++) {
            for (k = 0; k < MATRIX_LENGTH; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }

    #if MATRIX_LENGTH <= 16

    for (i = 0; i < MATRIX_LENGTH; i++) {
        for (j = 0; j < MATRIX_LENGTH; j++) {
            printf("%4d ", A[i][j]);
        }
        printf("\n");
    }
    
    printf("\n");

    for (i = 0; i < MATRIX_LENGTH; i++) {
        for (j = 0; j < MATRIX_LENGTH; j++) {
            printf("%4d ", B[i][j]);
        }
        printf("\n");
    }

    printf("\n");

    for (i = 0; i < MATRIX_LENGTH; i++) {
        for (j = 0; j < MATRIX_LENGTH; j++) {
            printf("%4d ", C[i][j]);
        }
        printf("\n");
    }

    #endif

    return 0;
}