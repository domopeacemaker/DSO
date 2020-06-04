/*
 * FUSE: Filesystem in Userspace
 * DSO 2020
 * Prueba proyecto librería de música
*/

#ifndef __PRUEBA_MP3__
#define __PRUEBA_MP3__

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

/***********************************
 * macros y estructura
 * */

#define for_each_item(item, list) \
    for(T * item = list->head; item != NULL; item = item->next)
    

// Esta es la estructura de los ficheros mp3

struct estructura_mis_datos
{  				
	int numero_ficheros;     				/* número de ficheros encontrados */
	char *path;								/* path donde se encuentran los ficheros mp3 */
	char *mp3_inicial;						/* fichero mp3 inicial */ 
	const char *nombre_ficheros[];			/* nombre de los ficheros */
	struct timespec st_atim;  				/* fechas del fichero */
    struct timespec st_mtim; 
    struct timespec st_ctim;  
    uid_t     st_uid;        				/* El usuario y grupo */
    gid_t     st_gid;  
    
    // Añadimos nuevos registros para controlar los TAGs del fichero mp3
   
	char *titulo[];
	char *artista[];
	char *album[];
	int year;
	char *genero[];
	int numero_pista;
	
	
};

typedef struct lista_artista * Artista;

// Esta es la estructura de un artista

struct lista_artista
{
    //char * path;						  
    //char * fichero_inicial;  				
    //char nombre_fichero;
    char *tag_artista[];				// Nombre del artista
    //int numero_ficheros			 
    Artista sig;				// Puntero que apunta al artista siguiente en la lista

};


/***********************************
 * Busca un fichero path en mi estructura de datos
 * */
int buscar_fichero(const char *path, struct estructura_mis_datos *mis_datos);

/***********************************
 * error en parametros de entrada
 * */
void error_parametros();

/***********************************
 * lee fichero con los capítulos y rellena estructura mis_datos
 * */
void leer_fichero(struct estructura_mis_datos *mis_datos);

#endif
