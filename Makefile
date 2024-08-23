# set the compiler
CC = gcc

# compiler flags
CFLAGS = -Wall -Wextra -std=c11

# soruce files
SRCS = $(wildcard src/*.c)

# header files
INCLUDES = -Iinclude

# libraries to link
LIBS = -lz -lssl -lcrypto

# executable file
TARGET = minigit-c

# dependency files
OBJS = $(SRCS:.c=.o)

# default target
all: $(TARGET)

# make the executable file
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET) $(LIBS)
 
# make each object file
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# clean
clean:
	rm -rf $(OBJS) $(TARGET) .minigit-c

# rebuild
rebuild: clean all
