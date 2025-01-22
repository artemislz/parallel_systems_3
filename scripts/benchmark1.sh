
# Define the parameters
GENERATIONS=1000
GRID_SIZES=(512)
NUM_PROCESSES=(1 4 16 64)
RUNS=4

# Output CSV file
OUTPUT_CSV="experiment_results.csv"

# Create or overwrite the CSV file with a header row
echo "Grid Size,Processes,Average Time (s)" > $OUTPUT_CSV

# Loop through the combinations of grid sizes and process counts
for GRID_SIZE in "${GRID_SIZES[@]}"; do
    for PROCESSES in "${NUM_PROCESSES[@]}"; do
        echo "Running with grid size ${GRID_SIZE}x${GRID_SIZE} and ${PROCESSES} processes..."
        TOTAL_TIME=0

        for RUN in $(seq 1 $RUNS); do
            echo "  Run $RUN..."
            if [ $PROCESSES -eq 1 ]; then
                # Run the serial program and capture the output
                TIME=$(./3_1s $GENERATIONS $GRID_SIZE)
                TOTAL_TIME=$(echo "$TOTAL_TIME + $TIME" | bc)
                echo "    Time for run $RUN: $TIME seconds"
            else
                # Run the MPI program and capture the output
                OUTPUT=$(mpirun -np $PROCESSES -f machines ./3_1 $GENERATIONS $GRID_SIZE)

                # Extract the slowest process time from the output
                SLOWEST_TIME=$(echo "$OUTPUT" | grep "The slowest process took" | awk '{print $5}')

                # Add the slowest process time to the total time
                TOTAL_TIME=$(echo "$TOTAL_TIME + $SLOWEST_TIME" | bc)
                echo "    Time for run $RUN: $SLOWEST_TIME seconds"
            fi
        done

        # Calculate average time
        AVG_TIME=$(echo "$TOTAL_TIME / $RUNS" | bc -l)
        echo "Average time for grid size ${GRID_SIZE}x${GRID_SIZE} with ${PROCESSES} processes: $AVG_TIME seconds"

        # Append the results to the CSV file
        echo "${GRID_SIZE}x${GRID_SIZE},${PROCESSES},${AVG_TIME}" >> $OUTPUT_CSV
    done
done

echo "All experiments completed. Results saved in $OUTPUT_CSV."