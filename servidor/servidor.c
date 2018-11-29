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
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <sys/signal.h>
#include <unistd.h>
#include <errno.h>
#include "jugadores.h"
#include "preguntas.h"
#include "../protocolo/protocolo.h"

void deceso(int sig);

void ver_id(char *mens_rec, char *mens_id);

void establecer_conexion(char *mens_rec);

void avisar_salida(int sdJug, char *nombre);

void mostrar_jugadores(void);

void avisar_juego(int sdJug);

void rechazar_juego(int sdJug);

void avisar_sin_juego(int sdJug);

void avisar_listo(int sdJug);

void avisar_sale(int sdJug);

void atender_juego(void);

void enviar_pregunta(int id, int orden);

void ver_respuesta(char *mens_rec, int *orden, int *resp, int *tiempo);

void enviar_resultado(int sdJug, int bien);

void enviar_resumen(int sdJug);

int ver_ganador(char *nombre);


int sd;
int sdJug;
fd_set set;
fd_set copiaset;
extern int errno;

int main(int argc, char *argv[])
{
    int n;
    int pid;
    int puerto = 2222;
    int empe;
    int orden;
    int resp;
    int tiempo;
    int bien;
    int juego;
    int terminaron;
    int ganador;
    socklen_t clilen;
    char mens_id[5];
    char nombre[9];
    char mens_env[MAXMENS];
    char mens_rec[MAXMENS];
    struct sockaddr_in cli;
    struct sockaddr_in server;
   
    signal(SIGCHLD,deceso);

    if (argc == 2) {

	puerto = atoi(argv[1]);

	if (puerto == 0) {

		printf("Ejecute %s [puerto]\n",argv[0]);

		exit (1);

	}

    }

    server.sin_family = AF_INET;

    server.sin_port = htons(puerto);

    server.sin_addr.s_addr = INADDR_ANY;

    sd = socket(PF_INET, SOCK_STREAM, 0);

    if (bind(sd, (struct sockaddr *) &server, sizeof(server)) < 0) {

	perror("Error en bind");

	exit(1);

    }

    if ( listen(sd, 5) < 0 ) {

	perror("Error en listen");

	exit(1);

    }

    ini_jugadores();

    juego = 0;

    FD_ZERO(&set);

    FD_SET(sd, &set);

    while (1) {

	printf("\nEsperando Jugadores:\n\n");

	empe = 0;

	while (!empe) {

	    copiaset = set;

	    if ( (select(FD_SETSIZE, &copiaset, NULL, NULL, NULL) < 0) && (errno!=EINTR) ) {

          	perror("Error en select");

		exit(1);

	    }

	    if (FD_ISSET(sd, &copiaset)) {

		clilen = sizeof(cli);

		sdJug = accept(sd, (struct sockaddr *) &cli, &clilen);

		FD_SET(sdJug, &set);

	    } else {

		for (sdJug = 0; sdJug < FD_SETSIZE; sdJug++) {

		    if (FD_ISSET(sdJug, &copiaset)) {

			if ((n = leer_mensaje(sdJug, mens_rec)) == 0) {

			    close(sdJug);

			    FD_CLR(sdJug, &set);

			    ver_nombre(sdJug, nombre);

			    if (nombre[0] != '\0') {

				printf("%s salio del juego\n", nombre);

				avisar_salida(sdJug, nombre);

				borrar_jugador(sdJug);

				if (juego == sdJug) {

				    avisar_sin_juego(juego);

				    juego = 0;

				}

			    }

			} else {

			    ver_id(mens_rec, mens_id);

			    if (strcmp(mens_id, "CONE") == 0) {

				establecer_conexion(mens_rec);

				if (juego != 0) {

				    avisar_juego(juego);

				}

			    }

			    if (strcmp(mens_id, "LIST") == 0) {

				mostrar_jugadores();

			    }

			    if (strcmp(mens_id, "CREA") == 0) {

				jugador_listo(sdJug);

				if (juego == 0) {

				    juego = sdJug;

				    avisar_juego(sdJug);

				} else {

				    rechazar_juego(sdJug);

				}

			    }

			    if (strcmp(mens_id, "UNIR") == 0) {

				jugador_listo(sdJug);

				avisar_listo(sdJug);

			    }

			    if (strcmp(mens_id, "SALE") == 0) {

				jugador_sale(sdJug);

				avisar_sale(sdJug);

			    }

			    if (strcmp(mens_id, "EMPE") == 0) {

				empe = 1;

			    }

			} // if

		    } // if 

		} // for

	    } // if

	} // while

	/**************** FIN ETAPA DE CONEXION ********************/

	printf("\nComenzo un Juego\n");

	pid = fork();

	if (pid == 0) {

	    FD_CLR(sd, &set);

	    close(sd);

	    // COMIENZO DEL JUEGO

	    // ENVIAR 1er PREGUNTA A TODOS

	    ini_preguntas();

	    for (sdJug = 0; sdJug < FD_SETSIZE; sdJug++) {

		if (FD_ISSET(sdJug, &set)) {

		    if (ver_listo(sdJug)) {

			snprintf(mens_env, MAXMENS, "EMPE");

			enviar_mensaje(sdJug, mens_env);

			enviar_pregunta(sdJug, 1);

		    } else {

			borrar_jugador(sdJug);

			FD_CLR(sdJug, &set);

			close(sdJug);

		    }

		}

	    }

	    terminaron = 0;

	    while (terminaron != cantidad_jugadores()) {

		copiaset = set;

	    	if ( (select(FD_SETSIZE, &copiaset, NULL, NULL, NULL) < 0) && (errno!=EINTR) ) {

          		perror("Error en select");

			exit(1);

	    	}

		for (sdJug = 0; sdJug < FD_SETSIZE; sdJug++) {

		    if (FD_ISSET(sdJug, &copiaset)) {

			if ((n = leer_mensaje(sdJug, mens_rec)) == 0) {

			    FD_CLR(sdJug, &set);

			    close(sdJug);

			    if (!jugador_termino(sdJug)) {

				terminaron++;

				jugador_sale(sdJug);

			    }

			} else {

			    ver_id(mens_rec, mens_id);

			    if (strcmp(mens_id, "RESP") == 0) {

				ver_respuesta(mens_rec, &orden, &resp,
					      &tiempo);

				bien = (respuesta(orden) == resp);

				enviar_resultado(sdJug, bien);

				cargar_respuesta(sdJug, bien, tiempo);

				if (jugador_termino(sdJug)) {

				    terminaron++;

				    enviar_resumen(sdJug);

				} else {

				    enviar_pregunta(sdJug,
						    avanzar_pregunta
						    (sdJug));

				}

			    }

			} // if

		    } // if 

		} // for                        

	    } // while

	    ganador = ver_ganador(nombre);

	    for (sdJug = 0; sdJug < FD_SETSIZE; sdJug++) {

		if (FD_ISSET(sdJug, &set)) {

		    if (sdJug==ganador) {
			
       		    	snprintf(mens_env, MAXMENS,
			         "\nFelicitaciones %s ganaste el juego!!!",
			         nombre);

		    } else {
			    
	       		    snprintf(mens_env, MAXMENS,
			             "\nLo siento, %s gano el juego!!!",
			             nombre);

	    	    }

		    enviar_mensaje(sdJug, mens_env );

		    snprintf(mens_env, MAXMENS, "RESU FIN");

		    enviar_mensaje(sdJug, mens_env );

		    close(sdJug);

		}

	    }

	    exit(0);

	    /*************** FIN DEL JUEGO *********************/

	} else { // fork()

	    /******** CIERRO LOS CLIENTES QUE JUGARON ***********/

	    for (sdJug = 0; sdJug < FD_SETSIZE; sdJug++) {

		if (FD_ISSET(sdJug, &set)) {

		    if (sd != sdJug) {

			if (ver_listo(sdJug)) {

			    borrar_jugador(sdJug);

			    FD_CLR(sdJug, &set);

			    close(sdJug);

			}

		    }

		}

	    }

	    avisar_sin_juego(juego);

	    juego = 0;

	}

    } /***** while(1) VUELVO A LA ETAPA DE CONEXION *******/

    close(sd);

    exit(0);

}

void ver_id(char *mens_rec, char *mens_id)
{
    memcpy(mens_id, mens_rec, 4);

    mens_id[4] = '\0';
}

void establecer_conexion(char *mens_rec)
{
    int i;
    char mens_nombre[9];
    char nom[9];
    char mens_env[MAXMENS];

    memcpy(mens_nombre, mens_rec + 4, 8);

    mens_nombre[8] = '\0';

    if (!jugador_duplicado(mens_nombre)) {

	printf("%s ingreso al Juego\n", mens_nombre);

	agregar_jugador(sdJug, mens_nombre);

	snprintf(mens_env, MAXMENS, "CONE OK");

	enviar_mensaje(sdJug, mens_env );

	for (i = 0; i < FD_SETSIZE; i++) {

	    if (FD_ISSET(i, &set)) {

		if ((i != sd) && (i != sdJug)) {

		    ver_nombre(i, nom);

		    if (nom[0] != '\0') {

			snprintf(mens_env, MAXMENS,
				 "AVIS %s ingreso al juego", mens_nombre);

			enviar_mensaje(i, mens_env );

		    }

		}

	    }

	}


    } else {

	snprintf(mens_env, MAXMENS, "DUPL");

	enviar_mensaje(sdJug, mens_env );

    }

}

void avisar_salida(int sdJug, char *nombre)
{
    int i;
    char mens_env[MAXMENS];
    char nom[9];

    for (i = 0; i < FD_SETSIZE; i++) {

	if (FD_ISSET(i, &set)) {

	    if ((i != sd) && (i != sdJug)) {

		ver_nombre(i, nom);

		if (nom[0] != '\0') {

		    snprintf(mens_env, MAXMENS, "AVIS %s salio del juego",
			     nombre);

		    enviar_mensaje(i, mens_env );

		}

	    }

	}

    }

}

void mostrar_jugadores(void)
{
    int i;
    char nombre[9];
    char mens_env[MAXMENS];

    for (i = 0; i < FD_SETSIZE; i++) {

	if (FD_ISSET(i, &set)) {

	    if (i != sd) {

		ver_nombre(i, nombre);

		if (nombre[0] != '\0') {

		    if (ver_listo(i)) {

			snprintf(mens_env, MAXMENS,
				 "%s esta listo para jugar", nombre);

		    } else {

			snprintf(mens_env, MAXMENS, "%s esta conectado",
				 nombre);

		    }

		    enviar_mensaje(sdJug, mens_env );

		}
	
	    }

	}

    }


    snprintf(mens_env, MAXMENS, "LIST FIN");

    enviar_mensaje(sdJug, mens_env );

}

void avisar_juego(int sdJug)
{
    int i;
    char nombre[9];
    char mens_env[MAXMENS];

    snprintf(mens_env, MAXMENS, "CREA OK");

    enviar_mensaje(sdJug, mens_env );

    for (i = 0; i < FD_SETSIZE; i++) {

	if (FD_ISSET(i, &set)) {

	    if ((i != sd) && (i != sdJug)) {

		ver_nombre(sdJug, nombre);

		snprintf(mens_env, MAXMENS, "JUEG %s creo un Juego.",
			 nombre);

		enviar_mensaje(i, mens_env );

	    }

	}

    }

}

void rechazar_juego(int sdJug)
{
    char mens_env[MAXMENS];

    snprintf(mens_env, MAXMENS, "CREA NO");

    enviar_mensaje(sdJug, mens_env );
}

void avisar_sin_juego(int sdJug)
{
    int i;
    char nombre[9];
    char mens_env[MAXMENS];

    for (i = 0; i < FD_SETSIZE; i++) {

	if (FD_ISSET(i, &set)) {

	    if ((i != sd) && (i != sdJug) && (!ver_listo(i))) {

		ver_nombre(sdJug, nombre);

		snprintf(mens_env, MAXMENS, "NOJU %s ya comenzo el juego",
			 nombre);

		enviar_mensaje(i, mens_env );

	    }

	}

    }

}

void avisar_listo(int sdJug)
{
    int i;
    char nombre[9];
    char mens_env[MAXMENS];

    for (i = 0; i < FD_SETSIZE; i++) {

	if (FD_ISSET(i, &set)) {

	    if ((i != sd) && (i != sdJug)) {

		ver_nombre(sdJug, nombre);

		snprintf(mens_env, MAXMENS,
			 "AVIS %s esta listo para jugar", nombre);

		enviar_mensaje(i, mens_env );

	    }

	}

    }

}

void avisar_sale(int sdJug)
{
    int i;
    char nombre[9];
    char mens_env[MAXMENS];

    for (i = 0; i < FD_SETSIZE; i++) {

	if (FD_ISSET(i, &set)) {

	    if ((i != sd) && (i != sdJug)) {

		ver_nombre(sdJug, nombre);

		snprintf(mens_env, MAXMENS,
			 "AVIS %s no desea unirse al juego", nombre);

		enviar_mensaje(i, mens_env );

	    }

	}

    }

}

void enviar_pregunta(int id, int orden)
{
    char mensaje[MAXMENS];
    char preg[MAXMENS];
    char op1[MAXMENS];
    char op2[MAXMENS];
    char op3[MAXMENS];

    pregunta(orden, preg, op1, op2, op3);

    snprintf(mensaje, MAXMENS, "%s", preg);

    enviar_mensaje(sdJug, mensaje );

    snprintf(mensaje, MAXMENS, "%s", op1);

    enviar_mensaje(sdJug, mensaje );

    snprintf(mensaje, MAXMENS, "%s", op2);

    enviar_mensaje(sdJug, mensaje );

    snprintf(mensaje, MAXMENS, "%s", op3);

    enviar_mensaje(sdJug, mensaje );

}

void ver_respuesta(char *mens_rec, int *orden, int *resp, int *tiempo)
{
    char aux[MAXMENS];

    memcpy(aux, mens_rec + 5, 2);

    aux[2] = '\0';

    *orden = atoi(aux);

    memcpy(aux, mens_rec + 8, 1);

    aux[1] = '\0';

    *resp = atoi(aux);

    memcpy(aux, mens_rec + 10, 5);

    aux[5] = '\0';

    *tiempo = atoi(aux);

}

void enviar_resultado(int sdJug, int bien)
{
    char mensaje[MAXMENS];

    snprintf(mensaje, MAXMENS, "RESP %d", bien);

    enviar_mensaje(sdJug, mensaje );
}

void enviar_resumen(int sdJug)
{
    int i;
    char nombre[9];
    int respondidas;
    int correctas;
    int tiempo;
    char mensaje[MAXMENS];

    for (i = 0; i < FD_SETSIZE; i++) {

	if (es_jugador(i)) {

	    ver_jugador(i, nombre, &respondidas, &correctas, &tiempo);

	    if (jugador_termino(i) || !ver_listo(i)) {

		nombre[8] = '\0';

		snprintf(mensaje, MAXMENS,
			 "%-8s  %5d\t\t %5d\t\t %dm %dseg", nombre,
			 respondidas, correctas, abs(tiempo / 60),
			 (tiempo % 60));

		enviar_mensaje(sdJug, mensaje );

		if (i != sdJug && ver_listo(i)) {

		    ver_jugador(sdJug, nombre, &respondidas, &correctas,
				&tiempo);

		    nombre[8] = '\0';

		    snprintf(mensaje, MAXMENS,
			     "%-8s  %5d\t\t %5d\t\t %dm %dseg", nombre,
			     respondidas, correctas, abs(tiempo / 60),
			     (tiempo % 60));

		    enviar_mensaje(i, mensaje );

		}

	    }

	}

    }

}

int ver_ganador(char *nombre)
{
    int id;
    int g_id;
    int correctas;
    int g_correctas;
    int tiempo;
    int g_tiempo;
    int respondidas;
    char nom[9];

    g_correctas = -1;
    g_tiempo = -1;

    for (id = 1; id < FD_SETSIZE; id++) {

	if (es_jugador(id)) {

	    ver_jugador(id, nom, &respondidas, &correctas, &tiempo);

	    if (correctas > g_correctas) {

		g_id = id;

                g_correctas = correctas;

		g_tiempo = tiempo;

		snprintf(nombre, 8, "%s", nom);

	    } else {

		if (correctas == g_correctas) {

		    if (tiempo <= g_tiempo) {

			g_correctas = correctas;

			g_tiempo = tiempo;

			snprintf(nombre, 8, "%s", nom);

		    }

		}

	    }

	}

    }

    return (g_id);

}

void deceso(int sig) {
	int status;

	wait3(&status,WNOHANG,(struct rusage *)0);
	
	printf("\nFinalizo un juego\n");
	
}
