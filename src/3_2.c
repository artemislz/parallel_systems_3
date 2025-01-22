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
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <n>\n", argv[0]);
        return EXIT_FAILURE;
    }
    int rank, size;
    size_t n = atol(argv[1]);
    double start_time = 0, init = 0,
        scatter = 0, reduce = 0;
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
    int local_n = n / size;
    char* local_a;
    char* local_x;
    local_a = malloc(n * local_n * sizeof(char));
    local_x = malloc(local_n * sizeof(char));
    clock_t matr_s, matr_e;
    // double matr_s, matr_e;
    if (rank == 0) {
        srand(17);
        A = malloc(n * n * sizeof(char));
        x = malloc(n * sizeof(char));
        // init -= MPI_Wtime();
        matr_s = clock();
        initialize(A, x, n);
        matr_e = clock();
        // init += MPI_Wtime();
    }
    MPI_Barrier(MPI_COMM_WORLD);
    scatter -= MPI_Wtime();
    MPI_Scatter(A, local_n, coltype, local_a, n * local_n, MPI_CHAR, 0, MPI_COMM_WORLD);
    scatter += MPI_Wtime();
    MPI_Type_free(&col);
    MPI_Type_free(&coltype);
    MPI_Scatter(x, local_n, MPI_CHAR, local_x, local_n, MPI_CHAR, 0, MPI_COMM_WORLD);//doesnt cost anything
    if (rank == 0) {
        free(A);
        free(x);
    }
    MPI_Barrier(MPI_COMM_WORLD);
    start_time -= MPI_Wtime();
    long unsigned int* local_y = calloc(n, sizeof(long unsigned int));
    for (size_t j = 0; j < local_n; j++) {
        for (size_t i = 0; i < n; i++) {
            local_y[i] += local_a[j * n + i] * local_x[j];
        }
    }
    start_time += MPI_Wtime();
    long unsigned int* y;
    if (rank == 0) {
        y = malloc(n * sizeof(long unsigned int));
    }
    MPI_Barrier(MPI_COMM_WORLD);
    reduce -= MPI_Wtime();
    MPI_Reduce(local_y, y, n, MPI_CHAR, MPI_SUM, 0, MPI_COMM_WORLD);
    reduce += MPI_Wtime();
    if (rank == 0) {
        free(y);
    }
    double elapsed_time, init_time, scatter_time, reduce_time;
    double max_time, max_init_time, max_scatter_time, max_reduce_time;
    //MPI initialization time
    init_time = (double)(end - start) / (double)CLOCKS_PER_SEC;
    MPI_Reduce(&start_time, &max_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    MPI_Reduce(&init_time, &max_init_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    MPI_Reduce(&scatter, &max_scatter_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    MPI_Reduce(&reduce, &max_reduce_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    if (rank == 0) {
        double elapsed_matr = (double)(matr_e - matr_s)/CLOCKS_PER_SEC;
        printf("Matr Init Time: %.4f, Max Init Time: %.4f, Max Scatter Time: %.4f, Max Reduce Time: %.4f, Max Comp Time: %.4f, Total Time: %.4f\n",
            // init, //matr init time, no need to find max only proc 0 computes it
            elapsed_matr, //matr init time, no need to find max only proc 0 computes it
            // matr_e - matr_s,
            max_init_time,
            max_scatter_time,
            max_reduce_time,
            max_time,
            elapsed_matr + max_time + max_init_time + max_scatter_time + max_reduce_time);
        //printf("%.4f %.4f %.4f %.4f %.4f\n", init_end - init_start, max_init_time, max_scatter_time, max_reduce_time, max_time);
        // printf("Matrix initialization time: %f seconds\n", (init_end - init_start));
        // printf("The slowest process took %f seconds to initialize\n", max_init_time);
        // printf("The slowest a process took %f seconds to scatter\n", max_scatter_time);
        // printf("The slowest a process took %f seconds to reduce\n", max_reduce_time);
        // printf("The slowest process took %f seconds to do calculations\n", max_time);
    }
    free(local_a);
    free(local_x);
    free(local_y);
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
        x[i] = rand() % 25;
        for (int j = 0; j < n; j++) {
            A[i * n + j] = rand() % 25;
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