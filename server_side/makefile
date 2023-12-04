CC = gcc
CFLAGS = -Wall -Wextra

# Source files
SRCS = server.c error_handling.c client_handling.c request_handling.c utils.c

# Object files
OBJS = $(SRCS:.c=.o)

# Executable name
TARGET = my_server

all: $(TARGET)

$(TARGET): $(OBJS)
		$(CC) $(CFLAGS) $^ -o $@

%.o: %.c
		$(CC) $(CFLAGS) -c $< -o $@

clean:
		rm -f $(OBJS) $(TARGET)
