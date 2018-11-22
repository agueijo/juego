SRCS = cliente.c ../protocolo/protocolo.c

DEBUG = -g

CC = gcc

all: cliente
	@echo "Ejecute: ./cliente [servidor [puerto]]"

cliente: $(SRCS)
	$(CC) -o cliente $(SRCS)

debug: $(SRCS)
	$(CC) $(DEBUG) -o cliente $(SRCS)

clean:
	rm -f cliente
