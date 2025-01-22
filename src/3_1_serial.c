#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ALIVE 1 
#define DEAD 0

void initialize_grid(int **grid, int N) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            grid[i][j] = rand() % 2;
        }
    }
}

void update_grid(int **current, int **next, int N) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            int live_neighbors = 0;
            //ugly but werks
            int ii = (i - 1 + N) % N;
            int jj = (j - 1 + N) % N;
            int iii = (i + 1 + N) % N;
            int jjj = (j + 1 + N) % N;
            live_neighbors = 
                current[ii][jj] +  // top left
                current[ii][j] +      // top
                current[ii][jjj] +  // top right
                current[i][jj] +      // left
                current[i][jjj] +      // right
                current[iii][jj] +  // bottom left
                current[iii][j] +      // bottom
                current[iii][jjj];   // bottom right
            if (current[i][j] == ALIVE && (live_neighbors < 2 || live_neighbors > 3)) {
                next[i][j] = DEAD;
            } else if (current[i][j] == DEAD && live_neighbors == 3) {
                next[i][j] = ALIVE;
            } else {
                next[i][j] = current[i][j];
            }
        }
    }
}

void print_grid(int **grid, int N) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (grid[i][j] == ALIVE) {
                printf("A ");
            } else {
                printf(". ");
            }
        }
        printf("\n");
    }
    printf("\n");
}


int main(int argc, char *argv[]) {
  
    if (argc < 3) {
        printf("Usage: %s <generations> <grid_size>\n", argv[0]);
        return -1;
    }
    int generations = atoi(argv[1]);
    int N = atoi(argv[2]);

    int **current_grid = (int **)malloc(N * sizeof(int *));
    int **next_grid = (int **)malloc(N * sizeof(int *));
    for (int i = 0; i < N; i++) {
        current_grid[i] = (int *)malloc(N * sizeof(int));
        next_grid[i] = (int *)malloc(N * sizeof(int));
    }

    initialize_grid(current_grid, N);
    double start, end, elapsed;
    start = clock();
    
    for (int gen = 0; gen < generations; gen++) {
        update_grid(current_grid, next_grid, N);
        int **temp = current_grid;
        current_grid = next_grid;
        next_grid = temp;
    }
    
    end = clock();
    elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("%f\n", elapsed);
    for (int i = 0; i < N; i++) {
        free(current_grid[i]);
        free(next_grid[i]);
    }
    free(current_grid);
    free(next_grid);
    return 0;
}