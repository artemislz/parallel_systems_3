#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>
#include <string.h>

#define ALIVE 1
#define DEAD 0

void initialize_grid(int *grid, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            grid[i * cols + j] = rand() % 2;
        }
    }
}

void print_grid(int *grid, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%c ", grid[i * cols + j] ? 'O' : '.');
        }
        printf("\n");
    }
    printf("\n");
}

void simulate_generation(int *current, int *next, int start, int end, int cols, int rows) {
    for (int i = start; i < end; i++) {
        for (int j = 0; j < cols; j++) {
            int live_neighbors = 0;
            int ii = i -1;  // self contained grid so no need to wrap around
            int jj = (j - 1 + cols) % cols;
            int iii = i +1;  // self contained grid so no need to wrap around
            int jjj = (j + 1 + cols) % cols;
            live_neighbors = 
                current[ii * cols + jj] +  // top left
                current[ii * cols + j] +   // top
                current[ii * cols + jjj] + // top right
                current[i * cols + jj] +   // left
                current[i * cols + jjj] +  // right
                current[iii * cols + jj] + // bottom left
                current[iii * cols + j] +  // bottom
                current[iii * cols + jjj]; // bottom right
            if (current[i * cols + j] == ALIVE && (live_neighbors < 2 || live_neighbors > 3)) {
                next[i * cols + j] = DEAD;
            } else if (current[i * cols + j] == DEAD && live_neighbors == 3) {
                next[i * cols + j] = ALIVE;
            } else {
                next[i * cols + j] = current[i * cols + j];
            }
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <generations> <grid_size>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int generations = atoi(argv[1]);
    int grid_size = atoi(argv[2]);
    
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
  
    // Assuming grid_size % size == 0
    int rows_per_process = grid_size / size;

    int start_row = rank * rows_per_process;
    int end_row = start_row + rows_per_process;

    int local_rows = end_row - start_row;
    int *current = malloc((local_rows + 2) * grid_size * sizeof(int)); // +2 for ghost rows
    int *next = malloc((local_rows + 2) * grid_size * sizeof(int));
    int* grid = NULL;

    if (rank == 0) {
        grid = malloc(grid_size * grid_size * sizeof(int));
        srand(time(NULL));
        initialize_grid(grid, grid_size, grid_size);
        //print_grid(grid, grid_size, grid_size);
    }
    // if(rank == 0){
    //     print_grid(grid, grid_size, grid_size);
    // }
       // Scatter the grid to all processes
    MPI_Scatter(grid, local_rows * grid_size, MPI_INT, current + grid_size, local_rows * grid_size, MPI_INT, 0, MPI_COMM_WORLD);

    // Start timing
    double start_time = MPI_Wtime();

    for (int gen = 0; gen < generations; gen++) {
        MPI_Request requests[4];
        int num_requests = 0;
        //first send your first line to previous process
        MPI_Isend(current + grid_size, grid_size, MPI_INT, (rank - 1 + size) % size, 0, MPI_COMM_WORLD, &requests[num_requests++]);
        //second send your last line to next process
        MPI_Isend(current + local_rows*grid_size, grid_size , MPI_INT, (rank + 1 + size) % size, 0, MPI_COMM_WORLD, &requests[num_requests++]);
        //recieve your last line form the next process (the other process's first line)
        MPI_Irecv(current+(local_rows+1)*grid_size, grid_size, MPI_INT, (rank+1 +size) %size, 0, MPI_COMM_WORLD, &requests[num_requests++]);
        //recieve your first line from the previous process (the other process's last line)
        MPI_Irecv(current, grid_size, MPI_INT, (rank - 1 + size) % size, 0, MPI_COMM_WORLD, &requests[num_requests++]);
        //wait all the requests to finish
        MPI_Waitall(num_requests, requests, MPI_STATUSES_IGNORE);

      
        simulate_generation(current, next, 1, local_rows + 1, grid_size, grid_size);

        int *temp = current;
        current = next;
        next = temp;
    }

    MPI_Gather(current + grid_size, local_rows * grid_size, MPI_INT, grid, local_rows * grid_size, MPI_INT, 0, MPI_COMM_WORLD);
    // End timing
    double end_time = MPI_Wtime();
    double elapsed_time = end_time - start_time;

    // Find the maximum time taken by any process
    double max_time;
    MPI_Reduce(&elapsed_time, &max_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        print_grid(grid, grid_size, grid_size);
        printf("The slowest process took %f seconds\n", max_time);
        free(grid);
    }

    free(current);
    free(next);
    MPI_Finalize();
    return 0;
}