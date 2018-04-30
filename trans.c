/*
 * 
 *
 *
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */
#include <stdio.h>
#include "cachelab.h"

#define BLKSIZE 8
#define HALFSIZE 4

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */

void transpose_align(int M, int N, int A[N][M], int B[M][N]);

void transpose_arbitrary(int M, int N, int A[N][M], int B[M][N]);


char transpose_submit_desc[] = "Transpose submission";

void transpose_submit(int M, int N, int A[N][M], int B[M][N]) {
    if (N % BLKSIZE == 0)
        transpose_align(M, N, A, B);
    else
        transpose_arbitrary(M, N, A, B);
}

void transpose_align(int M, int N, int A[N][M], int B[M][N]) {
    int i, j;
    int blk_i, blk_j;
    int tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;
    for (i = 0; i < N; i += BLKSIZE) {
        for (j = 0; j < M; j += BLKSIZE) {
            if (i != j || N != M) {//case 1: normal case, optimize for N = M = 64
                /* Block in B, 8 by 8
                 * -----
                 * |1|2|
                 * -----
                 * |3|4|
                 * -----
                 *
                 * 1) move 1, 2
                 * 2) transpose 1, 2 in B
                 * 3) move 3 while transposing to B, move 2 in B to 3 in B
                 * 4) transpose and move 4
                 */

                for (blk_i = 0; blk_i < HALFSIZE; blk_i++)
                    for (blk_j = 0; blk_j < BLKSIZE; blk_j++)
                        B[j + blk_i][i + blk_j] = A[i + blk_i][j + blk_j];

                for (blk_i = 0; blk_i < HALFSIZE; blk_i++)
                    for (blk_j = blk_i + 1; blk_j < HALFSIZE; blk_j++) {
                        tmp0 = B[j + blk_i][i + blk_j];
                        B[j + blk_i][i + blk_j] = B[j + blk_j][i + blk_i];
                        B[j + blk_j][i + blk_i] = tmp0;

                        tmp0 = B[j + blk_i][i + HALFSIZE + blk_j];
                        B[j + blk_i][i + HALFSIZE + blk_j] = B[j + blk_j][i + HALFSIZE + blk_i];
                        B[j + blk_j][i + HALFSIZE + blk_i] = tmp0;
                    }
                for (blk_i = 0; blk_i < HALFSIZE; blk_i++) {
                    tmp0 = A[i + HALFSIZE][j + blk_i];
                    tmp1 = A[i + HALFSIZE + 1][j + blk_i];
                    tmp2 = A[i + HALFSIZE + 2][j + blk_i];
                    tmp3 = A[i + HALFSIZE + 3][j + blk_i];

                    tmp4 = B[j + blk_i][i + HALFSIZE];
                    tmp5 = B[j + blk_i][i + HALFSIZE + 1];
                    tmp6 = B[j + blk_i][i + HALFSIZE + 2];
                    tmp7 = B[j + blk_i][i + HALFSIZE + 3];

                    B[j + blk_i][i + HALFSIZE] = tmp0;
                    B[j + blk_i][i + HALFSIZE + 1] = tmp1;
                    B[j + blk_i][i + HALFSIZE + 2] = tmp2;
                    B[j + blk_i][i + HALFSIZE + 3] = tmp3;

                    B[j + HALFSIZE + blk_i][i] = tmp4;
                    B[j + HALFSIZE + blk_i][i + 1] = tmp5;
                    B[j + HALFSIZE + blk_i][i + 2] = tmp6;
                    B[j + HALFSIZE + blk_i][i + 3] = tmp7;
                }
                for (blk_i = HALFSIZE; blk_i < BLKSIZE; blk_i++)
                    for (blk_j = HALFSIZE; blk_j < BLKSIZE; blk_j++)
                        B[j + blk_j][i + blk_i] = A[i + blk_i][j + blk_j];
            } else if (i + BLKSIZE < N) {
                //case 2: diagonal blocks
                //copy block to adjacent block in array B
                for (blk_i = 0; blk_i < BLKSIZE; blk_i++)
                    for (blk_j = 0; blk_j < BLKSIZE; blk_j++)
                        B[i + blk_i][j + blk_j + BLKSIZE] = A[i + blk_i][j + blk_j];
                //transpose
                for (blk_j = BLKSIZE - 1; blk_j >= 0; blk_j--)
                    for (blk_i = HALFSIZE; blk_i < BLKSIZE; blk_i++)
                        B[j + blk_j][i + blk_i] = B[i + blk_i][j + blk_j + BLKSIZE];
                //copy to target block
                for (blk_j = 0; blk_j < BLKSIZE; blk_j++)
                    for (blk_i = 0; blk_i < HALFSIZE; blk_i++)
                        B[j + blk_j][i + blk_i] = B[i + blk_i][j + blk_j + BLKSIZE];

            } else {
                //case 3: last diagonal block
                //copy to target block
                for (blk_i = 0; blk_i < BLKSIZE; blk_i++) {
                    tmp0 = A[i + blk_i][j];
                    tmp1 = A[i + blk_i][j + 1];
                    tmp2 = A[i + blk_i][j + 2];
                    tmp3 = A[i + blk_i][j + 3];
                    tmp4 = A[i + blk_i][j + 4];
                    tmp5 = A[i + blk_i][j + 5];
                    tmp6 = A[i + blk_i][j + 6];
                    tmp7 = A[i + blk_i][j + 7];
                    B[j][i + blk_i] = tmp0;
                    B[j + 1][i + blk_i] = tmp1;
                    B[j + 2][i + blk_i] = tmp2;
                    B[j + 3][i + blk_i] = tmp3;
                    B[j + 4][i + blk_i] = tmp4;
                    B[j + 5][i + blk_i] = tmp5;
                    B[j + 6][i + blk_i] = tmp6;
                    B[j + 7][i + blk_i] = tmp7;
                }
                //transpose
                for (blk_i = 0; blk_i < BLKSIZE; blk_i++)
                    for (blk_j = blk_i + 1; blk_j < BLKSIZE; blk_j++) {
                        tmp0 = A[i + blk_i][j + blk_j];
                        A[i + blk_i][j + blk_j] = B[j + blk_j][i + blk_i];
                        B[j + blk_j][i + blk_i] = tmp0;
                    }
            }
        }
    }
}

void transpose_arbitrary(int M, int N, int A[N][M], int B[M][N]) {
    int i, j;
    int blk_i, blk_j;
    for (j = 0; j < M + BLKSIZE; j += BLKSIZE) {
        for (i = 0; i < N; i += BLKSIZE) {
            for (blk_i = 0; (blk_i < BLKSIZE) && (i + blk_i < N); blk_i++) {
                blk_j = (j == 0) ? 0 : -(M * (blk_i + i) + j) % BLKSIZE;
                if (j + blk_j < M)
                    do {
                        B[j + blk_j][i + blk_i] = A[i + blk_i][j + blk_j];
                        blk_j++;
                    } while ((M * (blk_i + i) + blk_j) % BLKSIZE != 0 && j + blk_j < M);
            }
        }
    }

}

/*
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";

void trans(int M, int N, int A[N][M], int B[M][N]) {
    int i, j, tmp;
    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }

    }

}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions() {
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc);

    /* Register any additional transpose functions */

    registerTransFunction(trans, trans_desc);

}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N]) {
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}

