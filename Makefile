MPICC = mpicc
CC = gcc
SRC = src
BIN = bin
.PHONY : all clean

3_1 : $(SRC)/3_1.c
	@mkdir -p $(BIN)
	$(MPICC) -o $(BIN)/3_1 $(SRC)/3_1.c
3_2 : $(SRC)/3_2.c
	@mkdir -p $(BIN)
	$(MPICC) -o $(BIN)/3_2 $(SRC)/3_2.c
3_1_serial : $(SRC)/3_1_serial.c
	@mkdir -p $(BIN)
	$(CC) -o $(BIN)/3_1_serial $(SRC)/3_1_serial.c
3_2_serial : $(SRC)/3_2_serial.c
	@mkdir -p $(BIN)
	$(CC) -o $(BIN)/3_2_serial $(SRC)/matrix_mul_serial.c

all : 3_1 3_2 3_1_serial 3_2_serial
	
clean:
	rm -rf $(BIN)