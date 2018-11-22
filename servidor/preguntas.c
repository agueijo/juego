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
#include <string.h>
#include <time.h>

#define MAXMENS 1024

//VARIABLES

int preguntas[10];

// FUNCIONES

int cant_preguntas(void)
{

    int  cant_preg;
    FILE *archivo;
    char aux[500];

    if ((archivo = fopen("preg.txt", "r")) == NULL) {

	printf("Error: no se encontro el archivo de preguntas (preg.txt)\n");

	exit(1);

    }

    cant_preg = 0;

    while (!feof(archivo)) {

	fgets(aux, 500, archivo);

	cant_preg++;

    }

    fclose(archivo);

    return (cant_preg / 5);

}

void ini_preguntas(void)
{
    int i;
    int j;

    double max;

    max = cant_preguntas();

    if (max < 10) {

	printf("Error: Al menos debe haber 10 preguntas en preg.txt\n");

	exit(1);

    }

    srandom(time(0));

    for (i = 0; i <= 9; i++) {

	// Genero un numero entre 1 y max (cantidad depreguntas)

	preguntas[i] = (1 + (int) (max * rand() / (RAND_MAX + 1.0)));

	// por las dudas me fijo que no este repetido

	j = i;

	while (j >= 0) {

	    j--;

	    if (preguntas[j] == preguntas[i]) {

		j = 0;

		i--;

	    }

	}

    }

}

void pregunta(int orden, char *preg, char *op1, char *op2, char *op3)
{
    int i;
    int id;
    char resp[5];
    FILE *archivo;


    id = preguntas[orden - 1];

    if ((archivo = fopen("preg.txt", "r")) == NULL) {

	printf("Error: no se encontro el archivo de preguntas (preg.txt)\n");

	exit(1);

    }

    for (i = 1; i <= id; i++) {

	fgets(preg, MAXMENS, archivo);

	fgets(op1, MAXMENS, archivo);

	fgets(op2, MAXMENS, archivo);

	fgets(op3, MAXMENS, archivo);

	fgets(resp, 5, archivo);

    }

    preg[strlen(preg) - 1] = '\0';

    op1[strlen(op1) - 1] = '\0';

    op2[strlen(op2) - 1] = '\0';

    op3[strlen(op3) - 1] = '\0';

    resp[strlen(resp) - 1] = '\0';

    fclose(archivo);
}

int respuesta(int orden)
{
    int  i;
    int  id;
    char aux[MAXMENS];
    char resp[5];
    int  respuesta;
    FILE *archivo;


    id = preguntas[orden - 1];

    if ((archivo = fopen("preg.txt", "r")) == NULL) {

	printf("Error: no se encontro el archivo de preguntas (preg.txt)\n");

	exit(1);

    }

    for (i = 1; i <= id; i++) {

	fgets(aux, MAXMENS, archivo);

	fgets(aux, MAXMENS, archivo);

	fgets(aux, MAXMENS, archivo);

	fgets(aux, MAXMENS, archivo);

	fgets(resp, 5, archivo);

    }

    fclose(archivo);

    resp[strlen(resp) - 1] = '\0';

    respuesta = atoi(resp);

    return (respuesta);

}


