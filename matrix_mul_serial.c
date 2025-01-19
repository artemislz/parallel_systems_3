#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
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

int main(int argc, char** argv){
    clock_t start, end;
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <n>\n", argv[0]);
        return EXIT_FAILURE;
    }
    int n = atoi(argv[1]);
    start = clock();
    double* A = malloc(n * n * sizeof(double));
    double* x = malloc(n * sizeof(double));
    double* y = malloc(n * sizeof(double));
    srand(17);
    for (int i = 0; i < n; i++) {
        x[i] = rand() % 10;
        y[i] = 0;
        for (int j = 0; j < n; j++) {
            A[i * n + j] = rand() % 10;
        }
    }
    end = clock();

    // print_matrix(A, n, n);
    // print_vector(x, n, "x");
    for (int i = 0; i < n; i++) {      // for each row
        for (int j = 0; j < n; j++) {          // for each column
            y[i] += A[i * n + j] * x[i];   // access A by columns
        }
    }
    /*
    * [1 , 2 , 3 , 4 , 5 , 6 , 7, 8]
    */
    // print_vector(y, n, "y");
    printf("Time: %f\n", (double)(end - start) / CLOCKS_PER_SEC);
    free(A);
    free(x);
    free(y);
    return 0;   
}