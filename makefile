CC = gcc
CFLAGS = -pthread -Wall -Wextra -g

# Add -DTEST flag if TEST=true is specified in make command
ifeq ($(TEST), true)
CFLAGS += -DTEST
endif

# Add THRESHOLD value if specified in make command
ifdef THRESHOLD
CFLAGS += -DTHRESHOLD=$(THRESHOLD)
else
CFLAGS += -DTHRESHOLD=10000
endif

# Add NUM_THREADS_COUNT value if specified, default to 4
ifdef NUM_THREADS_COUNT
CFLAGS += -DNUM_THREADS_COUNT=$(NUM_THREADS_COUNT)
else
CFLAGS += -DNUM_THREADS_COUNT=4
endif

# Target executable name
TARGET = main

# Source files
SRCS = main.c file_info.c parallel_merge_sort.c merge_sort.c parallel_count_sort.c count_sort.c

# Object files (replace .c extension with .o)
OBJS = $(SRCS:.c=.o)

# Default target
all: $(TARGET)

# Link object files to create the executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) -lm  # Link the math library

# Compile each .c file into .o object file
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Run the program
run: $(TARGET)
	./$(TARGET)

# Clean up object files and the executable
clean:
	rm -f $(OBJS) $(TARGET)
