# Define the parameters
N=(8192 16384 32768)
NUM_PROCESSES=(1 2 4 16 32 64)
RUNS=4

# Output CSV file
OUTPUT_CSV="data2.csv"

# Create or overwrite the CSV file with a header row
echo "N,Processes,Average Init Time (s),Average Scatter Time (s),Average Reduce Time (s),Average Calculation Time (s)" > $OUTPUT_CSV

# Loop through the combinations of grid sizes and process counts
for n in "${N[@]}"; do
    for PROCESSES in "${NUM_PROCESSES[@]}"; do
        echo "Running with grid size ${n}x${n} and ${PROCESSES} processes..."
        TOTAL_INIT_TIME=0
        TOTAL_SCATTER_TIME=0
        TOTAL_REDUCE_TIME=0
        TOTAL_CALC_TIME=0

        for RUN in $(seq 1 $RUNS); do
            echo "  Run $RUN..."
            if [ $PROCESSES -eq 1 ]; then
                # Run the serial program and capture the output
                OUTPUT=$(./3_2s $n)
                echo "Serial Output: $OUTPUT"
                INIT_TIME=$(echo "$OUTPUT" | awk '{print $1}')
                CALC_TIME=$(echo "$OUTPUT" | awk '{print $2}')
                TOTAL_INIT_TIME=$(echo "$TOTAL_INIT_TIME + $INIT_TIME" | bc -l)
                TOTAL_CALC_TIME=$(echo "$TOTAL_CALC_TIME + $CALC_TIME" | bc -l)
                echo "    Init Time for run $RUN: $INIT_TIME seconds"
                echo "    Calculation Time for run $RUN: $CALC_TIME seconds"
            else
                # Run the MPI program and capture the output
                OUTPUT=$(mpirun -np $PROCESSES -f machines ./ex2 $n)
                echo "MPI Output: $OUTPUT"

                # Extract the average times from the output
                AVG_INIT_TIME=$(echo "$OUTPUT" | grep "Init Time:" | awk -F', ' '{print $1}' | awk '{print $3}')
                AVG_SCATTER_TIME=$(echo "$OUTPUT" | grep "Max Scatter Time:" | awk -F', ' '{print $3}' | awk '{print $4}')
                AVG_REDUCE_TIME=$(echo "$OUTPUT" | grep "Max Reduce Time:" | awk -F', ' '{print $4}' | awk '{print $4}')
                AVG_TOTAL_TIME=$(echo "$OUTPUT" | grep "Max Time:" | awk -F', ' '{print $5}' | awk '{print $3}')

                # Add the average times to the total times
                TOTAL_INIT_TIME=$(echo "$TOTAL_INIT_TIME + $AVG_INIT_TIME" | bc -l)
                TOTAL_SCATTER_TIME=$(echo "$TOTAL_SCATTER_TIME + $AVG_SCATTER_TIME" | bc -l)
                TOTAL_REDUCE_TIME=$(echo "$TOTAL_REDUCE_TIME + $AVG_REDUCE_TIME" | bc -l)
                TOTAL_CALC_TIME=$(echo "$TOTAL_CALC_TIME + $AVG_TOTAL_TIME" | bc -l)
                echo "    Avg Init Time for run $RUN: $AVG_INIT_TIME seconds"
                echo "    Avg Scatter Time for run $RUN: $AVG_SCATTER_TIME seconds"
                echo "    Avg Reduce Time for run $RUN: $AVG_REDUCE_TIME seconds"
                echo "    Avg Total Time for run $RUN: $AVG_TOTAL_TIME seconds"
            fi
        done

        # Calculate average times
        AVG_INIT_TIME=$(echo "$TOTAL_INIT_TIME / $RUNS" | bc -l)
        AVG_SCATTER_TIME=$(echo "$TOTAL_SCATTER_TIME / $RUNS" | bc -l)
        AVG_REDUCE_TIME=$(echo "$TOTAL_REDUCE_TIME / $RUNS" | bc -l)
        AVG_CALC_TIME=$(echo "$TOTAL_CALC_TIME / $RUNS" | bc -l)
        echo "Average Init Time for grid size ${n}x${n} with ${PROCESSES} processes: $AVG_INIT_TIME seconds"
        echo "Average Scatter Time for grid size ${n}x${n} with ${PROCESSES} processes: $AVG_SCATTER_TIME seconds"
        echo "Average Reduce Time for grid size ${n}x${n} with ${PROCESSES} processes: $AVG_REDUCE_TIME seconds"
        echo "Average Total Time for grid size ${n}x${n} with ${PROCESSES} processes: $AVG_CALC_TIME seconds"

        # Append the results to the CSV file
        echo "${n}x${n},${PROCESSES},${AVG_INIT_TIME},${AVG_SCATTER_TIME},${AVG_REDUCE_TIME},${AVG_CALC_TIME}" >> $OUTPUT_CSV
    done
done