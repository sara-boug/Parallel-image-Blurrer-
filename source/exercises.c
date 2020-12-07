#include <stdio.h>
#include <stdlib.h>

#include "mpi.h"

/**
 * Master reads a value and sends it to the others with the MPI_Send function.
 **/
void exercise1(int rank, int world_size) {
    printf("My node ID is %d \n", rank);
    int value;
    if (rank == 0) {
        do {
            scanf("%d", &value);
            for (int i = 1; i < world_size; i++) {
                MPI_Send(&value, 1, MPI_INT, i, 123, MPI_COMM_WORLD);
            }

        } while (value >= 0);
    } else {
        do {
            MPI_Status status;
            MPI_Recv(&value, 1, MPI_INT, 0, 123, MPI_COMM_WORLD, &status);
            printf("Node %d received %d\n", rank, value);
        } while (value >= 0);
    }
}

/**
 * Master broadcasts its message with the MPI_Bcast function.
 * Slaves recevie the message with the MPI_Bcast function as well. 
 * All nodes where the id is different from the sender (the fourth argument, here 0) will receive the data.
 **/
void exercise1_bcast(int rank, int world_size) {
    printf("My node ID is %d \n", rank);
    int value;
    if (rank == 0) {
        do {
            scanf("%d", &value);
            MPI_Bcast(&value, 1, MPI_INT, 0, MPI_COMM_WORLD);
        } while (value >= 0);
    } else {
        do {
            MPI_Status status;
            MPI_Bcast(&value, 1, MPI_INT, 0, MPI_COMM_WORLD);
            printf("Node %d received %d\n", rank, value);
        } while (value >= 0);
    }
}

/**
 * Master reads a value on stdin and each node adds one to the value, then
 * sends the value to the next node.
 * Master node waits for the loop to come back to him before finishing.
 **/
void exercise2(int rank, int world_size) {
    int value;
    const int tag = 213;
    if (rank == 0) {
        MPI_Status status;
        printf("Enter a value to send:\n");
        scanf("%d", &value);
        MPI_Send(&value, 1, MPI_INT, 1, tag, MPI_COMM_WORLD);
        MPI_Recv(&value, 1, MPI_INT, world_size - 1, tag, MPI_COMM_WORLD, &status);
        printf("Loop back: end value is %d\n", value);
    } else {
        MPI_Status status;
        MPI_Recv(&value, 1, MPI_INT, rank - 1, tag, MPI_COMM_WORLD, &status);
        value += 1;
        MPI_Send(&value, 1, MPI_INT, (rank + 1) % world_size, tag, MPI_COMM_WORLD);
        printf("Process %d got %d\n", rank, value);
    }
}

int dot_product(int vec1[5], int vec2[5]) {
    int res = 0;
    for (int i = 0; i < 5; ++i) {
        res += vec1[i] * vec2[i];
    }
    return res;
}

/**
 * Master sends the data to the other nodes, then computes its part and
 * gathers the data.
 **/
void exercice3_master(int world_size) {
    int A[4][5] = {
        {3, 5, 2, 6, 4},
        {4, 3, 7, 4, 5},
        {8, 9, 3, 3, 7},
        {1, 2, 1, 6, 6}};
    int B[5] = {2, 9, 1, 0, 7};
    // Step 1: Broadcast the matrix and the vector
    MPI_Bcast(A, 20, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(B, 5, MPI_INT, 0, MPI_COMM_WORLD);
    int C[4];
    C[0] = dot_product(A[0], B);
    MPI_Gather(&C[0], 1, MPI_INT, C, 1, MPI_INT, 0, MPI_COMM_WORLD);
    printf("The answer is : ");
    for (int i = 0; i < 4; ++i) {
        printf("%d ", C[i]);
    }
    printf("\n");
}

/**
 * The slave receive (by broadcast) the data to compute, then computes it and finally 
 * sends the data through the MPI_Gather function.
 **/
void exercise3_slave(int rank) {
    int A[4][5];
    int B[5];
    int C[4];
    MPI_Bcast(A, 20, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(B, 5, MPI_INT, 0, MPI_COMM_WORLD);
    int result = dot_product(A[rank], B);
    MPI_Gather(&result, 1, MPI_INT, NULL, 1, MPI_INT, 0, MPI_COMM_WORLD);
}

/**
 * Matrix multiplication. 
 * Master broadcasts the value of the matrix and of the vector, then gathers the result.
 **/
void exercise3(int rank, int world_size) {
    if (rank == 0) {
        exercice3_master(world_size);
    } else {
        exercise3_slave(rank);
    }
}

int main(int argc, char **argv) {
    int rank, world_size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    //exercise1(rank, world_size);
    //exercise1_bcast(rank, world_size);
    //exercise2(rank, world_size);
    exercise3(rank, world_size);

    MPI_Finalize();
    return 0;
}