/************************************************************************/
/*        Juego de Preguntas y Respuestas (cliente/servidor)            */
/*                        Internetworking                               */
/*                Universidad Tecnológica Nacional                      */
/*                   Facultad Regional La Plata                         */
/*                    Buenos Aires - Argentina                          */
/*                         Diciembre 2001                               */
/*                                                                      */
/*                          Agustín Eijo                                */
/*                       agu@frlp.utn.edu.ar                            */
/*                                                                      */
/*                          Andres Artola                               */
/*                     andres_artola@yahoo.com                          */
/************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include "../protocolo/protocolo.h"

void ingrese_nombre(void);

void mostrar_menu(int estado);

void mostrar_prompt(void);

void listar_jugadores(void);

int leer_aviso(int estado);

int crear_juego(void);

void unir_juego(void);

void empezar_juego(void);

void esperar_pregunta(int num_preg);

void enviar_respuesta(int num_preg);

void mostrar_resultado(void);

void esperar_resumen(void);

void solicitar_resumen(void);

void salir_juego(void);

int extern errno;
int sd;

int main(int argc, char *argv[])
{
    int num_preg;
    int estado;
    int opcion;
    int empe;
    int esperar;
    int puerto = 2222;
    char host[MAXMENS] = "localhost";
    char pantalla[MAXMENS];
    struct hostent *servidor;
    struct sockaddr_in server;
    fd_set set;

    switch (argc) {
	case 3:
		puerto = atoi(argv[2]);

	case 2:
		snprintf(host,MAXMENS,"%s",argv[1]);
		break;

	case 1:
		break;

	default:		
		printf("Ejecute: %s [servidor [puerto]]\n", argv[0]);		
		exit(1);		
    }

    server.sin_family = AF_INET;

    server.sin_port = htons(puerto);

    if (servidor = gethostbyname(host)) {

    	memcpy(&server.sin_addr, servidor->h_addr, servidor->h_length);    

    } else {	    
	    
	printf("No se encontro %s (%s)\n",host,strerror(errno));
	
	printf("Ejecute: %s [servidor [puerto]]\n", argv[0]);	
	
	exit(1);
	
    }

    sd = socket(PF_INET, SOCK_STREAM, 0);

    if (sd < 0) {

	perror("Error:");
	
	exit(1);

    }

    if (connect(sd, (struct sockaddr *) &server, sizeof(server)) != 0) {

	printf("Servidor no disponible en %s:%d (%s)\n",host,puerto,strerror(errno));

	printf("Ejecute: %s [servidor [puerto]]\n", argv[0]);

	exit(1);

    }

    ingrese_nombre();

    estado = 1;

    mostrar_menu(estado);

    empe = 0;

    while (!empe) {

	opcion = 0;

	FD_ZERO(&set);

	FD_SET(STDIN_FILENO, &set);

	FD_SET(sd, &set);

	select(FD_SETSIZE, &set, NULL, NULL, NULL);

	if (FD_ISSET(STDIN_FILENO, &set)) {

	    fgets(pantalla, MAXMENS, stdin);

	    opcion = atoi(pantalla);

	    if (opcion != 1 && opcion != 2) {

		mostrar_prompt();

	    }

	} else {

	    estado = leer_aviso(estado);

	    if (estado == 5) {

		empe = 1;

	    }

	}

	switch (opcion) {

	case 1:

	    listar_jugadores();

	    mostrar_prompt();

	    break;

	case 2:

	    switch (estado) {

	    case 1:

		if (crear_juego()) {

		    printf("\nEl juego se creo satisfactoriamente.\n");

		    printf("Usted decide cuando empezar el juego\n");

		    estado = 3;

		    mostrar_menu(estado);

		}

		break;

	    case 2:

		unir_juego();

		estado = 4;

		mostrar_menu(estado);

		break;

	    case 3:

		empezar_juego();

		estado = leer_aviso(estado);

		if (estado == 5) {

		    empe = 1;

		}

		break;

	    case 4:

		salir_juego();

		estado = 2;

		mostrar_menu(estado);

		break;

	    }

	}

    }

    //COMENZO EL JUEGO

    esperar = 5;

    while (esperar != 0) {

	printf("\tcomenzando el juego en %d segundos ...\n", esperar);

	sleep(1);
	
	esperar--;

    }

    printf("\nEmpezo el juego:\n\n");

    for (num_preg = 1; num_preg <= 10; num_preg++) {

	esperar_pregunta(num_preg);

	enviar_respuesta(num_preg);

	mostrar_resultado();

    }

    esperar_resumen();

    printf("\n\nJuego terminado.\n");

    close(sd);

    //FIN JUEGO

    exit(0);

}

void ingrese_nombre(void)
{
    int res;
    int cone;
    char mensaje[MAXMENS];
    char nombre[10];

    cone = 0;

    while (!cone) {

	res = 0;

	while (res == 0) {

	    printf("\nIngrese Nombre (8 caracteres): ");

	    fgets(nombre, 10, stdin);
	  
	    nombre[strlen(nombre)-1] = '\0';

	    res = strlen(nombre);

	}

	snprintf(mensaje, MAXMENS, "CONE");

	strcat(mensaje, nombre);

	enviar_mensaje(sd, mensaje);

	res = 0;

	while (!res) {

	    leer_mensaje( sd, mensaje );

	    if (strcmp(mensaje, "CONE OK") == 0) {

		printf("\nUsted ingreso al juego\n");

		cone = 1;

		res = 1;

	    }

	    if (strcmp(mensaje, "DUPL") == 0) {

		printf("\nYa existe un jugador con el nombre %s \n",
		       nombre);

		res = 1;

	    }

	}

    }

}

void mostrar_menu(int estado)
{
    if (estado == 1) {

	printf("\n1-Listar Jugadores\n2-Crear Juego\n");

    }

    if (estado == 2) {

	printf("\n1-Listar Jugadores\n2-Unirse al Juego\n");

    }

    if (estado == 3) {

	printf("\n1-Listar Jugadores\n2-Empezar Juego\n");

    }

    if (estado == 4) {

	printf("\n1-Listar Jugadores\n2-Desconectarce del Juego\n");

    }

    printf("\n(1,2)=> ");

    fflush(stdout);
}

void mostrar_prompt(void)
{
    printf("(1,2)=> ");

    fflush(stdout);
}

void listar_jugadores(void)
{
    int listfin;
    char mensaje[MAXMENS];

    printf("\nJugadores:\n");

    snprintf(mensaje, MAXMENS, "LIST");

    enviar_mensaje(sd, mensaje);

    listfin = 0;

    while (!listfin) {

	leer_mensaje(sd, mensaje);

	if (strcmp(mensaje, "LIST FIN") == 0) {

	    printf("\n");

	    listfin = 1;

	} else {

	    printf("\t%s\n", mensaje);

	}
    }
}

int leer_aviso(int estado)
{
    char mens_rec[MAXMENS];
    char mens_id[5];
    char mensaje[MAXMENS];

    leer_mensaje(sd, mens_rec);

    memcpy(mens_id, mens_rec, 4);

    mens_id[4] = '\0';

    memcpy(mensaje, mens_rec + 5, (strlen(mens_rec) - 4));

    mensaje[(strlen(mens_rec) - 4)] = '\0';

    if (strcmp(mens_id, "AVIS") == 0) {

	printf("\n\nAVISO: %s\n\n", mensaje);

	mostrar_prompt();

    }

    if (strcmp(mens_id, "JUEG") == 0) {

	printf("\n\nAVISO: %s\n", mensaje);

	estado = 2;

	mostrar_menu(estado);

    }

    if (strcmp(mens_id, "EMPE") == 0) {

	printf("\n\nTiene 30 segundos para responder cada pregunta\n\n");

	estado = 5;

    }

    if (strcmp(mens_id, "NOJU") == 0) {

	printf("\n\nAVISO: %s\n", mensaje);

	estado = 1;

	mostrar_menu(estado);

    }
    
    return (estado);
    
}

int crear_juego(void)
{
    int res;
    char mensaje[MAXMENS];

    snprintf(mensaje, MAXMENS, "CREA");

    enviar_mensaje(sd, mensaje);

    res = 2;

    while (res == 2) {

	leer_mensaje(sd, mensaje);

	if (strcmp(mensaje, "CREA OK") == 0) {

	    res = 1;

	}

	if (strcmp(mensaje, "CREA NO") == 0) {

	    res = 0;

	}

    }

    return (res);

}

void unir_juego(void)
{
    char mensaje[MAXMENS];

    snprintf(mensaje, MAXMENS, "UNIR");

    enviar_mensaje(sd, mensaje);
}

void empezar_juego(void)
{
    char mensaje[MAXMENS];

    snprintf(mensaje, MAXMENS, "EMPE");

    enviar_mensaje(sd, mensaje);
}

void salir_juego(void)
{
    char mensaje[MAXMENS];

    snprintf(mensaje, MAXMENS, "SALE");

    enviar_mensaje(sd, mensaje);
}

void esperar_pregunta(num_preg)
{
    char preg[MAXMENS];
    char op1[MAXMENS];
    char op2[MAXMENS];
    char op3[MAXMENS];

    leer_mensaje(sd, preg);

    printf("\nPREGUNTA N° %d \n\n ", num_preg);

    printf("%s\n\n", preg);

    leer_mensaje(sd, op1);

    printf("\t%s\n", op1);

    leer_mensaje(sd, op2);

    printf("\t%s\n", op2);

    leer_mensaje(sd, op3);

    printf("\t%s\n", op3);

    printf("\t4 - Siguiente Pregunta \n\n");

}

void enviar_respuesta(int num_preg)
{
    int opcion;
    char pantalla[1024];
    char respuesta[50];
    long empezo;
    struct timeval t;
    fd_set set;

    opcion = 0;

    while ((opcion != 1) && (opcion != 2) && (opcion != 3) && (opcion != 4)) {

	printf("¿Respuesta (1,2,3,4)? ==> ");

	fflush(stdout);

	empezo = time(NULL);

	t.tv_sec = 30;
	
	t.tv_usec = 0;

	FD_ZERO(&set);
	FD_SET(STDIN_FILENO, &set);

	if (select(1, &set, NULL, NULL, &t) != 0) {

	    fgets(pantalla, MAXMENS, stdin);

	    opcion = atoi(pantalla);

	} else {

	    printf
		("\n\nSe termino el tiempo establecido (30 segundos).\n");

	    opcion = 4;

	}

	if ((opcion == 1) || (opcion == 2) || (opcion == 3) || (opcion == 4)) {

	    snprintf(respuesta, MAXMENS, "RESP %2d %d %5d", num_preg,
		     opcion, (time(NULL) - empezo));

	    enviar_mensaje(sd, respuesta);

	}
	
    }
    
}

void mostrar_resultado(void)
{
    char mensaje[MAXMENS];
    char aux[MAXMENS];

    leer_mensaje(sd, mensaje);

    memcpy(aux, mensaje + 5, 1);

    aux[1] = '\0';

    if (strcmp(aux, "1") == 0) {

	printf("\nCORRECTO !!!\n");

    } else {

	printf("\nINCORRECTO !!!\n");

    }
}

void esperar_resumen(void)
{
    int resufin;
    char mensaje[MAXMENS];
    struct timeval t;
    fd_set set;

    printf ("\nEsperando el resumen de cada jugador ");

    printf ("(presione CTR-C si no desea esperar).\n\nResumen:\n");

    printf("\tJugador	  Contesto	 Correctas	 Tiempo\n");

    resufin = 0;

    while (!resufin) {

	mensaje[0] = '\0';

	FD_ZERO(&set);

	FD_SET(sd, &set);

	t.tv_sec = 300;
	t.tv_usec = 0;

	if (select(FD_SETSIZE, &set, NULL, NULL, &t) == 0) {

    	    printf("\nJuego Terminado.\n");

	    close(sd);

	    exit(0);

	}

	leer_mensaje(sd, mensaje);

	if (strcmp(mensaje, "RESU FIN") == 0) {

	    resufin = 1;

	} else {

	    printf("\t%s\n", mensaje);

	}

    }

}

void solicitar_resumen(void)
{
    char mensaje[MAXMENS];

    snprintf(mensaje, MAXMENS, "RESU");

    enviar_mensaje(sd, mensaje);
}
