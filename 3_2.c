#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>
#include <string.h>

void initialize_matrix(double*, size_t, size_t);
void initialize_matrix_column(double*, size_t, size_t);
void initialize_vector(double*, size_t);
void print_matrix(double*, size_t, size_t);
void print_vector(double*, size_t, char*);

int main(int argc, char* argv[]) {
    double* A = NULL;
    double* x = NULL;
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <n>\n", argv[0]);
        return EXIT_FAILURE;
    }
    int rank, size;
    size_t n = atoi(argv[1]);
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int local_n = n / size;
    double* local_a = malloc(n * local_n * sizeof(double));
    double* local_x = malloc(n * sizeof(double));
    MPI_Datatype col, coltype;
    MPI_Type_vector(n, local_n, n, MPI_DOUBLE, &col);
    MPI_Type_commit(&col);
    MPI_Type_create_resized(col, 0, local_n * sizeof(double), &coltype);
    MPI_Type_commit(&coltype);
    if (rank == 0) {
        A = malloc(n * n * sizeof(double));
        x = malloc(n * sizeof(double));
        srand(17);
        initialize_matrix(A, n, n);
        initialize_vector(x, n);
        print_vector(x, n, "x");
        putchar('\n');
        print_matrix(A, n, n);
        MPI_Scatter(A, 1, coltype, local_a, n * local_n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        // MPI_Scatter(A, local_n*n, MPI_DOUBLE, local_a, local_n*n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        MPI_Scatter(x, local_n, MPI_DOUBLE, local_x, local_n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        // MPI_Bcast(x, n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        free(A);
        free(x);
    }
    else {
        // MPI_Scatter(A, local_n*n, MPI_DOUBLE, local_a, local_n*n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        MPI_Scatter(A, 1, coltype, local_a, n * local_n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        MPI_Scatter(x, local_n, MPI_DOUBLE, local_x, local_n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        // print_vector(x, n, "x'");
    }
    if(rank == 0){
        print_matrix(local_a,  n,local_n);
        // print_matrix(local_a,  local_n,n);
        // print_vector(local_x, local_n, "x'");
    }
    // MPI_Bcast(x, n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    // MPI_Allgather(local_x, local_n, MPI_DOUBLE, x, local_n, MPI_DOUBLE, MPI_COMM_WORLD);
    // print_vector(x, n, "x'");
    // double* local_sum = malloc(local_n * sizeof(double));
    double* local_y = calloc(n, sizeof(double));
    // for (size_t i = 0; i < n; i++) {
    //     for (size_t j = 0; j < local_n; j++) {
    //         local_y[i] += local_a[j * n + i] * local_x[j];
    //     }
    // }
    for (size_t j = 0; j < local_n; j++) {
        // local_y[j] = 0;
        for (size_t i = 0; i < n; i++) {
            local_y[i] += local_a[i * local_n + j] * local_x[j];
        }
    }
    // if(rank == 0){
    //     // printf("Rank %d: local_y[%zu] = %g\n", rank, 0, local_y[0]);
    //     print_vector(local_y, n, "y'");
    //     fflush(stdout);
    // }
    double* y;
    if (rank == 0) {
        y = malloc(n * n * sizeof(double));
        // MPI_Gather(local_y, local_n, MPI_DOUBLE, y, local_n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        MPI_Reduce(local_y, y, n, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
        print_vector(y, n, "y");
        fflush(stdout);
        free(y);
    }
    else {
        MPI_Reduce(local_y, y, n, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    }
    free(local_a);
    free(local_x);
    // free(local_y);
    // free(x);
    MPI_Finalize();
    return 0;
}

void print_matrix(double* a, size_t rows, size_t cols) {
    for (size_t i = 0; i < rows; i++) {
        if (i == rows / 2) {
            printf("A: ");
        }
        else {
            printf("   ");
        }
        for (size_t j = 0; j < cols; j++) {
            printf("%g ", a[i * cols + j]);
        }
        printf("\n");
    }
    printf("\n");
}

void print_vector(double* v, size_t n, char* name) {
    printf("%s: [", name);
    for (size_t i = 0; i < n; i++) {
        printf("%g ", v[i]);
    }
    printf("\b]\n");
}

void initialize_matrix(double* a, size_t rows, size_t cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            a[i * cols + j] = rand() % 10;
        }
    }
}

void initialize_matrix_column(double* a, size_t rows, size_t cols) {
    for (int j = 0; j < cols; j++) {
        for (int i = 0; i < rows; i++) {
            // Column-major indexing: j * rows + i
            a[j * rows + i] = rand() % 10;
        }
    }
}


void initialize_vector(double* a, size_t n) {
    for (int i = 0; i < n; i++) {
        a[i] = rand() % 10;
    }
}