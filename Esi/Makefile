TARGET=esi
LIBS= -lcommons -lpthread
CC= gcc
CFLAGS= -g -Wall

.PHONY: default all clean
default: $(TARGET)
all: default 



OBJECTS = $(patsubst %.c, %.o, $(wildcard *.c))
HEADERS = $(wildcard *.h)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS) 

$(TARGET): $(OBJECTS) 
	$(CC) $(OBJECTS) $(CFLAGS) $(LIBS) -o $@ 



clean: # Finalmente creamos una tarea para eliminar todos los .o y el binario
	-rm -f *.o
	-rm -f $(TARGET)