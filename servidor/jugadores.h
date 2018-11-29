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

#include <sys/types.h>

// VARIABLES

typedef struct {
        char    nom[9];
        int     listo;
        int     numpreg;
	int 	correctas;
	int	tiempo;
} jugadores;


jugadores j[1024];

int cant_jug;

// FUNCIONES

void ini_jugadores( void );

void agregar_jugador ( int id , char *nom );

int borrar_jugador ( int id );

void jugador_listo ( int id );

void jugador_sale ( int id );

int cantidad_jugadores ( void );

void ver_jugador ( int id , char *nom ,int *numpreg , int *correctas , int *tiempo );

void ver_nombre ( int id , char *nom );

int ver_listo ( int id );

int es_jugador ( int id );

int jugador_duplicado ( char *nom );

int avanzar_pregunta( int id );

void cargar_respuesta ( int id , int bien, int tiempo);

int jugador_termino ( int id );
