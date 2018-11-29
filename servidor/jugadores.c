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

// TAD JUGADORES

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

// VARIABLES

typedef struct {
    char nom[9];
    int listo;
    int numpreg;
    int correctas;
    int tiempo;
} jugadores;

jugadores j[FD_SETSIZE];

int cant_jug;

// FUNCIONES

void ini_jugadores(void)
{
    int id;

    cant_jug = 0;

    for (id = 1; id < FD_SETSIZE; id++) {

	j[id].nom[0] = '\0';

	j[id].numpreg = 0;

	j[id].listo = 0;
    }
}

void agregar_jugador(int id, char *nom)
{
    cant_jug++;

    sprintf(j[id].nom, nom);

    j[id].numpreg = 1;

    j[id].correctas = 0;

    j[id].tiempo = 0;

    j[id].listo = 0;
}

void borrar_jugador(int id)
{
    cant_jug--;

    j[id].nom[0] = '\0';
}

void jugador_listo(int id)
{
    j[id].listo = 1;
}

void jugador_sale(int id)
{
    j[id].listo = 0;
}

int cantidad_jugadores(void)
{
    return (cant_jug);
}

void
ver_jugador(int id, char *nom, int *numpreg, int *correctas, int *tiempo)
{
    sprintf(nom, j[id].nom);

    *numpreg = j[id].numpreg;

    *correctas = j[id].correctas;

    *tiempo = j[id].tiempo;
}

void ver_nombre(int id, char *nom)
{
    sprintf(nom, j[id].nom);
}


int ver_listo(int id)
{
    return (j[id].listo);
}

int es_jugador(int id)
{
    return (j[id].nom[0] != '\0');
}

int jugador_duplicado(char *nom)
{
    int dup;
    int id;

    id = 1;

    dup = 0;

    while ((!dup) && (id < FD_SETSIZE)) {

	if (j[id].numpreg != 0) {

	    if (strcmp(nom, j[id].nom) == 0) {
		dup = 1;
	    }
	}
	id++;

    }

    return (dup);

}

int avanzar_pregunta(int id)
{
    j[id].numpreg++;
    return j[id].numpreg;
}

void cargar_respuesta(int id, int bien, int tiempo)
{
    j[id].correctas = j[id].correctas + bien;
    j[id].tiempo = j[id].tiempo + tiempo;
}

int jugador_termino(int id)
{
    return (j[id].numpreg == 10);
}
