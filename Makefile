CC=gcc
CFLAGS=-g 
BIN=main
TARGET=threadpool.o condition.o main.o

.PHONY:all clean

$(BIN):$(TARGET)
	$(CC) $(CFLAGS) -o $@  $^ -lpthread -lrt

clean:
	rm -f $(BIN) *.o
