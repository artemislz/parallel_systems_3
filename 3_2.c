#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>
#include <string.h>

void initialize_matrix(int*, size_t, size_t);
void initialize_matrix_column(int*, size_t, size_t);
void initialize_vector(int*, size_t);
void print_matrix(int*, size_t, size_t);
void print_vector(int*, size_t, char*);

int main(int argc, char* argv[]) {
    int* A;
    int* x;
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <n>\n", argv[0]);
        return EXIT_FAILURE;
    }
    int rank, size;
    size_t n = atol(argv[1]);
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int local_n = n / size;
    int* local_a;
    int* local_x;
    
    // if (!local_a || !local_x) {
    // fprintf(stderr, "Failed to allocate local buffers\n");
    // fflush(stderr);
    // MPI_Abort(MPI_COMM_WORLD, 1);
    // }
    //εδω
    MPI_Datatype col, coltype;
    MPI_Type_vector(n, 1, n, MPI_INT, &col);
    MPI_Type_commit(&col);
    MPI_Type_create_resized(col, 0, 1 * sizeof(int), &coltype);
    MPI_Type_commit(&coltype);
    // printf("rank %d here\n", rank);
    // fflush(stdout);
    local_a = malloc(n * local_n * sizeof(int));
    local_x = malloc(local_n * sizeof(int));
    if (!local_a || !local_x) {
    fprintf(stderr, "Rank %d: Memory allocation failed\n", rank);
    fflush(stderr);
    MPI_Abort(MPI_COMM_WORLD, 1);
    }
    if (local_n * size != n) {
    fprintf(stderr, "Rank %d: Matrix size not divisible by number of processes\n", rank);
    fflush(stderr);
    }
    if (rank == 0) {
        srand(17);
        // printf("rank 0 here\n");
        // fflush(stdout);
        
        
        A = malloc(n * n * sizeof(int));
        // printf("rank %d here\n", rank);
        // fflush(stdout);
        initialize_matrix(A, n, n);
        // MPI_Scatter(A, local_n, coltype, local_a, n * local_n, MPI_INT, 0, MPI_COMM_WORLD);
        // free(A);
        x = malloc(n * sizeof(int));
        initialize_vector(x, n);
        // MPI_Scatter(x, local_n, MPI_INT, local_x, local_n, MPI_INT, 0, MPI_COMM_WORLD);
        // free(x);
        // if (!A || !x) {
        // fprintf(stderr, "Failed to allocate global buffers\n");
        // fflush(stderr);
        // MPI_Abort(MPI_COMM_WORLD, 1);
        // }
        // print_vector(x, n, "x");
        // putchar('\n');
        // print_matrix(A, n, n);
        // MPI_Scatter(A, local_n*n, MPI_INT, local_a, local_n*n, MPI_INT, 0, MPI_COMM_WORLD);
        // MPI_Bcast(x, n, MPI_INT, 0, MPI_COMM_WORLD);
    }
    // else {
        // MPI_Scatter(A, local_n*n, MPI_INT, local_a, local_n*n, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Barrier(MPI_COMM_WORLD);
        MPI_Scatter(A, local_n, coltype, local_a, n * local_n, MPI_INT, 0, MPI_COMM_WORLD);
        
        // printf("rank n here\n");
        // fflush(stdout);
        MPI_Scatter(x, local_n, MPI_INT, local_x, local_n, MPI_INT, 0, MPI_COMM_WORLD);
        // printf("here\n");
        // fflush(stdout);
        // print_vector(x, n, "x'");
    // }

    // if(rank == 0){
        // print_matrix(local_a,  n,local_n);
        // print_vector(local_a, n*local_n, "A");
        // print_matrix(local_a,  local_n,n);
        // print_vector(local_x, local_n, "x'");
    // }
    // MPI_Bcast(x, n, MPI_INT, 0, MPI_COMM_WORLD);
    // MPI_Allgather(local_x, local_n, MPI_INT, x, local_n, MPI_INT, MPI_COMM_WORLD);
    // print_vector(x, n, "x'");
    // int* local_sum = malloc(local_n * sizeof(int));
    int* local_y = calloc(n, sizeof(int));
    double start_time = MPI_Wtime();
    // for (size_t i = 0; i < n; i++) {
    //     for (size_t j = 0; j < local_n; j++) {
    //         local_y[i] += local_a[i * local_n + j] * local_x[j];
    //     }
    // }
    for (size_t j = 0; j < local_n; j++) {
        for (size_t i = 0; i < n; i++) {
            local_y[i] += local_a[j * n + i] * local_x[j];
        }
    }
    // for (size_t j = 0; j < local_n; j++) {
    //     // local_y[j] = 0;
    //     for (size_t i = 0; i < n; i++) {
    //         local_y[i] += local_a[i * local_n + j] * local_x[j];
    //     }
    // }
    // if(rank == 0){
    //     // printf("Rank %d: local_y[%zu] = %g\n", rank, 0, local_y[0]);
    //     print_vector(local_y, n, "y'");
    //     fflush(stdout);
    // }
    int* y;
    if (rank == 0) {
        y = malloc(n * n * sizeof(int));
        // MPI_Gather(local_y, local_n, MPI_INT, y, local_n, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Reduce(local_y, y, n, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
        // print_vector(y, n, "y");
        fflush(stdout);
        free(y);
    }
    else {
        MPI_Reduce(local_y, y, n, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    }
    double end_time = MPI_Wtime();
    double elapsed_time = end_time - start_time;
    double max_time;
    MPI_Reduce(&elapsed_time, &max_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    if (rank == 0) {
        printf("The slowest process took %f seconds\n", max_time);
    }
    free(local_a);
    free(local_x);
    free(local_y);
    // free(x);
    MPI_Finalize();
    return 0;
}

void print_matrix(int* a, size_t rows, size_t cols) {
    for (size_t i = 0; i < rows; i++) {
        if (i == rows / 2) {
            printf("A: ");
        }
        else {
            printf("   ");
        }
        for (size_t j = 0; j < cols; j++) {
            printf("%d ", a[i * cols + j]);
        }
        printf("\n");
    }
    printf("\n");
}

void print_vector(int* v, size_t n, char* name) {
    printf("%s: [", name);
    for (size_t i = 0; i < n; i++) {
        printf("%d ", v[i]);
    }
    printf("\b]\n");
}

void initialize_matrix(int* a, size_t rows, size_t cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            a[i * cols + j] = rand() % 10;
        }
    }
}

void initialize_matrix_column(int* a, size_t rows, size_t cols) {
    for (int j = 0; j < cols; j++) {
        for (int i = 0; i < rows; i++) {
            // Column-major indexing: j * rows + i
            a[j * rows + i] = rand() % 10;
        }
    }
}


void initialize_vector(int* a, size_t n) {
    for (int i = 0; i < n; i++) {
        a[i] = rand() % 10;
    }
}