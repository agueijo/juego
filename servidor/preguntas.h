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
// TAD PREGUNTAS

#include <stdio.h>
#include <stdlib.h>
#define MAXMENS 1024

//VARIABLES

int preguntas[10];

// FUNCIONES

void ini_preguntas ( void );

void pregunta ( int orden , char *preg , char *op1, char *op2 , char *op3 );

int respuesta ( int orden );

double cant_preguntas ( void );


