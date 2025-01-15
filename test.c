#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>

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
            int ii = i -1;//(i - 1 + rows) % rows;
            int jj = (j - 1 + cols) % cols;
            int iii = i +1;//(i + 1 + rows) % rows;
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
    int grid[10][10]= {
        {0, 0, 1, 0, 0, 1, 0, 0, 0, 1},
        {0, 1, 1, 0, 1, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 1, 0, 0, 0, 1, 1},
        {1, 1, 1, 0, 0, 1, 0, 0, 1, 0},
        {1, 1, 1, 1, 1, 0, 1, 0, 0, 1},
        {1, 1, 0, 0, 1, 1, 0, 0, 0, 1},
        {1, 1, 0, 0, 1, 0, 0, 1, 0, 1},
        {1, 0, 1, 0, 1, 0, 1, 0, 0, 1},
        {0, 0, 1, 0, 0, 0, 1, 0, 0, 1},
        {1, 1, 0, 1, 0, 0, 0, 0, 1, 0}
        };
    if (rank == 0) {
       // grid = malloc(grid_size * grid_size * sizeof(int));
        srand(time(NULL));
        //initialize_grid(grid, grid_size, grid_size);
      
        print_grid(grid, grid_size, grid_size);
    }
        // Print the initial grid for debugging
       // printf("Initial Grid (Process 0):\n");
      //  print_grid(grid, grid_size, grid_size);

       // Scatter the grid to all processes
    MPI_Scatter(grid, local_rows * grid_size, MPI_INT, current + grid_size, local_rows * grid_size, MPI_INT, 0, MPI_COMM_WORLD);
        
    if (rank == 0) {
    //    free(grid);
    }

    for (int gen = 0; gen < generations; gen++) {
        MPI_Request send_request[2], recv_request[2];
        int num_requests = 0;

        if (rank > 0) {
            // Send the first row of the local grid to the previous process
            MPI_Isend(current + grid_size, grid_size, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, &send_request[num_requests]);
            // Receive the ghost row from the previous process and put at the last ghost row
            MPI_Irecv(current + (local_rows + 1) * grid_size, grid_size, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, &recv_request[num_requests]);
            num_requests++;
        } else {
            // Send the first row of the local grid to the last process
            MPI_Isend(current + grid_size, grid_size, MPI_INT, size - 1, 0, MPI_COMM_WORLD, &send_request[num_requests]);
            // Receive the ghost row from the last process
            MPI_Irecv(current + (local_rows+1)*grid_size, grid_size, MPI_INT, size - 1, 0, MPI_COMM_WORLD, &recv_request[num_requests]);
            num_requests++;
        }

        if (rank < size - 1) {
            // Send the last row of the local grid to the next process
            MPI_Isend(current + local_rows * grid_size, grid_size, MPI_INT, rank + 1, 0, MPI_COMM_WORLD, &send_request[num_requests]);
            // Receive the ghost row from the next process
            MPI_Irecv(current, grid_size, MPI_INT, rank + 1, 0, MPI_COMM_WORLD, &recv_request[num_requests]);
            num_requests++;
        } else {
            // Send the last row of the local grid to the first process
            MPI_Isend(current + local_rows * grid_size, grid_size, MPI_INT, 0, 0, MPI_COMM_WORLD, &send_request[num_requests]);
            // Receive the ghost row from the first process
            MPI_Irecv(current, grid_size, MPI_INT, 0, 0, MPI_COMM_WORLD, &recv_request[num_requests]);
            num_requests++;
        }

        MPI_Waitall(num_requests, send_request, MPI_STATUSES_IGNORE);
        MPI_Waitall(num_requests, recv_request, MPI_STATUSES_IGNORE);

       
        if(rank == 0 && gen == 0){
            printf("rank 0 gen 0\n");
            print_grid(current, local_rows + 2, grid_size);
        }

        simulate_generation(current, next, 1, local_rows + 1, grid_size, grid_size);

        int *temp = current;
        current = next;
        next = temp;

        // Print the grid at each generation
        if (rank == 0) {
            int *final_grid = malloc(grid_size * grid_size * sizeof(int));
            memcpy(final_grid, current + grid_size, local_rows * grid_size * sizeof(int));
            for (int p = 1; p < size; p++) {
                int s_row = p * rows_per_process;
                int e_row = s_row + rows_per_process;
                MPI_Recv(final_grid + s_row * grid_size, (e_row - s_row) * grid_size, MPI_INT, p, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
            if(gen == 0||gen == 1 || gen == 2 || gen == 3 ){
                print_grid(final_grid, grid_size, grid_size);
            }
         //  printf("Generation %d:\n", gen);
         //  print_grid(final_grid, grid_size, grid_size);
            free(final_grid);
        } else {
            MPI_Send(current + grid_size, local_rows * grid_size, MPI_INT, 0, 0, MPI_COMM_WORLD);
        }
    }

    free(current);
    free(next);
    MPI_Finalize();
    return 0;
}