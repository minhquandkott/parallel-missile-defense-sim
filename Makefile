CC = mpicc
CFLAGS = -Xpreprocessor -fopenmp -I/opt/homebrew/opt/libomp/include -O2 -Wall
LDFLAGS = -L/opt/homebrew/opt/libomp/lib -lomp

SRC_DIR = src
OBJ_DIR = obj
BIN = sim

SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(BIN)

.PHONY: all clean
