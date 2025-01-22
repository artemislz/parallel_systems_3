#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "timer.h"
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

int main(int argc, char** argv){
    clock_t start, end, start_init, end_init;
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <n>\n", argv[0]);
        return EXIT_FAILURE;
    }
    long int n = atol(argv[1]);
    char* A = malloc(n * n * sizeof(char));
    char* x = malloc(n * sizeof(char));
    long unsigned int* y = malloc(n * sizeof(long unsigned int));
    srand(17);
    // GET_TIME(start_init);
    start_init = clock();
    for (int i = 0; i < n; i++) {
        x[i] = rand() % 25;
        y[i] = 0;
        for (int j = 0; j < n; j++) {
            A[i * n + j] = rand() % 25;
        }
    }
    // GET_TIME(end_init);
    end_init = clock();
    // print_matrix(A, n, n);
    // print_vector(x, n, "x");
    // GET_TIME(start);
    start = clock();

    for (int i = 0; i < n; i++) {      // for each row
        for (int j = 0; j < n; j++) {          // for each column
            y[i] += A[i * n + j] * x[i];   // access A by columns
        }
    }
    // GET_TIME(end);
    end = clock();

    /*
    * [1 , 2 , 3 , 4 , 5 , 6 , 7, 8]
    */
    // print_vector(y, n, "y");
    double elapsed1, elapsed2;
    elapsed1 = (double)(end_init - start_init)/CLOCKS_PER_SEC;
    elapsed2 = (double)(end - start)/CLOCKS_PER_SEC;
    printf("Matr Init Time: %.4f Comp Time: %.4f Total Time: %.4f\n", elapsed1, elapsed2, elapsed1+elapsed2);
    free(A);
    free(x);
    free(y);
    return 0;   
}