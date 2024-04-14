SRC = pipe.c
OBJS = ${SRC:.c=.o} 

CC = gcc
CFLAGS = -std=c17 -Wpedantic -Wall -O2 -pipe -fno-plt
LDFLAGS = -Wl,-O1,--sort-common,--as-needed,-z,relro,-z,now

TARGET = pipe

all: ${TARGET}

# Rule to compile the source file into object file
$(OBJS): $(SRC)
	$(CC) $(CFLAGS) -c $< -o $@

# Rule to link object files into executable
$(TARGET): $(OBJS)
	$(CC) $^ -o $@

# Phony target for cleanup
.PHONY: clean
clean:
	rm -f $(OBJS) $(TARGET)
