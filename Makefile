# set the compiler
CC = gcc

# compiler flags
CFLAGS = -Wall -Wextra -std=c11

# soruce files
SRCS = src/*.c

# header files
INCLUDES = -Iinclude

# executable file
TARGET = minigit-c

# dependency files
OBJS = $(SRCS:.c=.o)

# default target
all: $(TARGET)

# make the executable file
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

# make each object file
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# clean
clean:
	rm -f $(OBJS) $(TARGET)

# rebuild
rebuild: clean all
