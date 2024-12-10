CC = gcc
CFLAGS = -Wall -I./Headers -std=c99
OBJ = list.o util.o
MAIN_OBJ = mmu.o
TEST_OBJ = test.o
EXEC_NAME = mmu
TEST_EXEC_NAME = test

# Build the main program
.PHONY: all
all: $(EXEC_NAME)

$(EXEC_NAME): $(OBJ) $(MAIN_OBJ)
	$(CC) $(CFLAGS) -o $(EXEC_NAME) $(OBJ) $(MAIN_OBJ)

# Build the test program
.PHONY: test
test: $(TEST_EXEC_NAME)

$(TEST_EXEC_NAME): $(OBJ) $(TEST_OBJ)
	$(CC) $(CFLAGS) -DTESTING -o $(TEST_EXEC_NAME) $(OBJ) $(TEST_OBJ) -std=c99

# Compile the object files
%.o: %.c
	$(CC) $(CFLAGS) -std=c99 -c $< -o $@

# Clean the build
.PHONY: clean
clean:
	rm -f *.o $(EXEC_NAME) $(TEST_EXEC_NAME)