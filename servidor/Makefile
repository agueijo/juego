SRCS = servidor.c preguntas.c jugadores.c ../protocolo/protocolo.c

DEBUG = -g

CC = gcc 

all: servidor
	@echo "Ejecute ./servidor [puerto]"

servidor: $(SRCS)
	$(CC) -o servidor $(SRCS)

debug: $(SRCS)
	$(CC) $(DEBUG) -o servidor $(SRCS)

clean:
	rm -f servidor
