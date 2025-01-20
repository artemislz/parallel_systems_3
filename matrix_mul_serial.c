#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
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
    clock_t start, end;
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <n>\n", argv[0]);
        return EXIT_FAILURE;
    }
    long int n = atol(argv[1]);
    char* A = malloc(n * n * sizeof(char));
    char* x = malloc(n * sizeof(char));
    long unsigned int* y = malloc(n * sizeof(long unsigned int));
    srand(17);
    // print_matrix(A, n, n);
    // print_vector(x, n, "x");
    start = clock();

    for (int i = 0; i < n; i++) {      // for each row
        for (int j = 0; j < n; j++) {          // for each column
            y[i] += A[i * n + j] * x[i];   // access A by columns
        }
    }
    end = clock();

    /*
    * [1 , 2 , 3 , 4 , 5 , 6 , 7, 8]
    */
    // print_vector(y, n, "y");
    printf("%f\n", (double)(end - start) / CLOCKS_PER_SEC);
    free(A);
    free(x);
    free(y);
    return 0;   
}