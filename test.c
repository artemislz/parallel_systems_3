#include <stdio.h>
#include <string.h> /* Για τη strlen */
#include <mpi.h>    /* Για συναρτήσεις MPI, κ.λπ. */

const int MAX_STRING = 100;

int main(void) {
    char greeting[MAX_STRING];
    int comm_sz; /* Πλήθος διεργασιών */
    int my_rank; /* Αριθμός τρέχουσας διεργασίας */

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    if (my_rank != 0) {
        sprintf(greeting, "Χαιρετισμούς από τη διεργασία %d από %d!", my_rank, comm_sz);
        MPI_Send(greeting, strlen(greeting) + 1, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
    } else {
        printf("Χαιρετισμούς από τη διεργασία %d από %d!! \n", my_rank, comm_sz);
        for (int q = 1; q < comm_sz; q++) {
            MPI_Recv(greeting, MAX_STRING, MPI_CHAR, q, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("%s \n", greeting);
        }
    }

    MPI_Finalize();
    return 0;
}
