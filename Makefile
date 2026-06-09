#compiler
CC = gcc
#compilerflags
CFLAGS = -Wall -Wextra -g
#output
TARGET = programTrab
#sources
SRCS = main.c auxiliares.c func_Btree.c func_csv.c func_reg.c

#generate object file names from source files
OBJS = $(SRCS:.c=.o)

# default target
all: $(TARGET)

# link object files
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# compile .c files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# clean objecs
clean:
	rm -f $(OBJS) $(TARGET)

# run
run: all
	./programTrab

zip: clean
	zip trab *.c *.h Makefile -o