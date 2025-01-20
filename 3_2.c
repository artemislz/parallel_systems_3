#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>
#include <string.h>

void initialize(char*, char*, size_t);
void initialize_matrix(char*, size_t, size_t);
void initialize_matrix_column(char*, size_t, size_t);
void initialize_vector(char*, size_t);
void print_matrix(char*, size_t, size_t);
void print_vector(char*, size_t, char*);

int main(int argc, char* argv[]) {
    char* A;
    char* x;
    // printf("hi\n");
    if (argc > 3) {
        fprintf(stderr, "Usage: %s <n> <1/0>\n", argv[0]);
        return EXIT_FAILURE;
    }
    int rank, size;
    size_t n = atol(argv[1]);
    double start_time, end_time, init_start, init_end
    //, init_matr_start, init_matr_end
    ,scatter_start, scatter_end
    ,reduce_start, reduce_end;    
    clock_t start, end;
    start = clock();
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Datatype col, coltype;
    MPI_Type_vector(n, 1, n, MPI_CHAR, &col);
    MPI_Type_commit(&col);
    MPI_Type_create_resized(col, 0, 1 * sizeof(char), &coltype);
    MPI_Type_commit(&coltype);
    end = clock();
    init_start = MPI_Wtime();
    int local_n = n / size;
    char* local_a;
    char* local_x;
    // if (!local_a || !local_x) {
    // fprintf(stderr, "Failed to allocate local buffers\n");
    // fflush(stderr);
    // MPI_Abort(MPI_COMM_WORLD, 1);
    // }
    //εδω
    
    init_end = MPI_Wtime();
    // for(int i=0; i<size; i++) {
    //     if (rank == i) {
    //         printf("Rank %d: %ld elements\n", rank, local_n*n);
    //     }
    //     MPI_Barrier(MPI_COMM_WORLD);
    // }
    // printf("rank %d here\n", rank);
    // fflush(stdout);
    local_a = malloc(n * local_n * sizeof(char));
    local_x = malloc(local_n * sizeof(char));
    // if (!local_a || !local_x) {
    // fprintf(stderr, "Rank %d: Memory allocation failed\n", rank);
    // fflush(stderr);
    // MPI_Abort(MPI_COMM_WORLD, 1);
    // }
    // if (local_n * size != n) {
    // fprintf(stderr, "Rank %d: Matrix size not divisible by number of processes\n", rank);
    // fflush(stderr);
    // }
    if (rank == 0) {
        srand(17);
        // printf("rank 0 here\n");
        // fflush(stdout);
        A = malloc(n * n * sizeof(char));
        x = malloc(n * sizeof(char));
        // printf("rank %d here\n", rank);
        // fflush(stdout);
        init_start = MPI_Wtime();
        initialize(A, x, n);
        // initialize_matrix(A, n, n);
        // initialize_vector(x, n);
        init_end = MPI_Wtime();
    }
    MPI_Barrier(MPI_COMM_WORLD);
    scatter_start = MPI_Wtime();
    MPI_Scatter(A, local_n, coltype, local_a, n * local_n, MPI_CHAR, 0, MPI_COMM_WORLD);
    scatter_end = MPI_Wtime();    
    MPI_Scatter(x, local_n, MPI_CHAR, local_x, local_n, MPI_CHAR, 0, MPI_COMM_WORLD);//doesnt cost anything
    // MPI_Barrier(MPI_COMM_WORLD);
    // for(int i=0; i<size; i++) {
    //     if (rank == i) {
    //         printf("Rank %d: %f seconds\n", rank, scatter_end - scatter_start);
    //     }
    //     MPI_Barrier(MPI_COMM_WORLD);
    // }
    if(rank == 0){
        free(A);
        free(x);
        // if (!A || !x) {
        // fprintf(stderr, "Failed to allocate global buffers\n");
        // fflush(stderr);
        // MPI_Abort(MPI_COMM_WORLD, 1);
        // }
        // print_vector(x, n, "x");
        // putchar('\n');
        // print_matrix(A, n, n);
        // MPI_Scatter(A, local_n*n, MPI_CHAR, local_a, local_n*n, MPI_CHAR, 0, MPI_COMM_WORLD);
        // MPI_Bcast(x, n, MPI_CHAR, 0, MPI_COMM_WORLD);
    }
    // else {
    //     // MPI_Scatter(A, local_n*n, MPI_CHAR, local_a, local_n*n, MPI_CHAR, 0, MPI_COMM_WORLD);
    //     // MPI_Barrier(MPI_COMM_WORLD);
    //     MPI_Scatter(A, local_n, coltype, local_a, n * local_n, MPI_CHAR, 0, MPI_COMM_WORLD);
        
    //     // printf("rank n here\n");
    //     // fflush(stdout);
    //     MPI_Scatter(x, local_n, MPI_CHAR, local_x, local_n, MPI_CHAR, 0, MPI_COMM_WORLD);
    //     // printf("here\n");
    //     // fflush(stdout);
    //     // print_vector(x, n, "x'");
    // }
    // if(rank == 0){
        // print_matrix(local_a,  n,local_n);
        // print_vector(local_a, n*local_n, "A");
        // print_matrix(local_a,  local_n,n);
        // print_vector(local_x, local_n, "x'");
    // }
    // MPI_Bcast(x, n, MPI_CHAR, 0, MPI_COMM_WORLD);
    // MPI_Allgather(local_x, local_n, MPI_CHAR, x, local_n, MPI_CHAR, MPI_COMM_WORLD);
    // print_vector(x, n, "x'");
    // char* local_sum = malloc(local_n * sizeof(char));
    start_time = MPI_Wtime();
    long unsigned int* local_y = calloc(n, sizeof(long unsigned int));
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
    end_time = MPI_Wtime();
    long unsigned int* y;
    if (rank == 0) {
        y = malloc(n * sizeof(long unsigned int));
    }
    reduce_start = MPI_Wtime();
    MPI_Reduce(local_y, y, n, MPI_CHAR, MPI_SUM, 0, MPI_COMM_WORLD);
    reduce_end = MPI_Wtime();
    if (rank == 0) {
        // print_vector(y, n, "y");
        // fflush(stdout);
        free(y);
    }
    double elapsed_time, init_time, scatter_time, reduce_time;
    double max_time, max_init_time, max_scatter_time, max_reduce_time;
    elapsed_time = end_time - start_time;
    init_time = (double)(end - start) / (double) CLOCKS_PER_SEC;
    scatter_time = scatter_end - scatter_start;
    reduce_time = reduce_end - reduce_start;
    MPI_Reduce(&elapsed_time, &max_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    MPI_Reduce(&init_time, &max_init_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    MPI_Reduce(&scatter_time, &max_scatter_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    MPI_Reduce(&reduce_time, &max_reduce_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    if (rank == 0) {
        printf("%.4f %.4f %.4f %.4f %.4f\n", init_end - init_start, max_init_time, max_scatter_time, max_reduce_time, max_time);
        // printf("Matrix initialization time: %f seconds\n", (init_end - init_start));
        // printf("The slowest process took %f seconds to initialize\n", max_init_time);
        // printf("The slowest a process took %f seconds to scatter\n", max_scatter_time);
        // printf("The slowest a process took %f seconds to reduce\n", max_reduce_time);
        // printf("The slowest process took %f seconds to do calculations\n", max_time);
    }
    free(local_a);
    free(local_x);
    free(local_y);
    // free(x);
    MPI_Finalize();
    return 0;
}

void print_matrix(char* a, size_t rows, size_t cols) {
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

void print_vector(char* v, size_t n, char* name) {
    printf("%s: [", name);
    for (size_t i = 0; i < n; i++) {
        printf("%d ", v[i]);
    }
    printf("\b]\n");
}

void initialize(char* A, char* x, size_t n) {
    for (int i = 0; i < n; i++) {
        x[i] = rand() % 10;
        for (int j = 0; j < n; j++) {
            A[i * n + j] = rand() % 10;
        }
    }
}

void initialize_matrix(char* a, size_t rows, size_t cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            a[i * cols + j] = rand() % 25;
        }
    }
}

void initialize_matrix_column(char* a, size_t rows, size_t cols) {
    for (int j = 0; j < cols; j++) {
        for (int i = 0; i < rows; i++) {
            // Column-major indexing: j * rows + i
            a[j * rows + i] = rand() % 25;
        }
    }
}


void initialize_vector(char* a, size_t n) {
    for (int i = 0; i < n; i++) {
        a[i] = rand() % 10;
    }
}