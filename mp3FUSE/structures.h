/*
 * FUSE: Filesystem in Userspace
 * DSO 2020
 * Prueba proyecto librería de música
*/
#define _XOPEN_SOURCE 500

#ifndef __PRUEBA_MP3__
#define __PRUEBA_MP3__


#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ftw.h>
  
// Esta es la estructura de un artista

struct lista_artista
{
    //char * path;						  
    //char * fichero_inicial;  				
    //char nombre_fichero;
    char *tag;				// Nombre del artista
    //int numero_ficheros		
    //TagLib_Tag *tag;					// Nombre del artista	 
    struct lista_artista *  sig;				// Puntero que apunta al artista siguiente en la lista

};

typedef struct lista_artista * Artista;


struct lista_album{
	char *tag;
	struct lista_album * sig;
};

typedef struct lista_album * Album;


struct lista_genero{
    char *tag;
    struct lista_genero * sig;
};

typedef struct lista_genero * Genero;

// Esta es la estructura de los ficheros mp3

struct estructura_mis_datos
{  				
	//int numero_ficheros;     				/* número de ficheros encontrados */
	char *path;								/* path donde se encuentran los ficheros mp3 */
	//char *mp3_inicial;						/* fichero mp3 inicial */ 
	//const char *nombre_ficheros[];			/* nombre de los ficheros */
	struct timespec st_atim;  				/* fechas del fichero */
    struct timespec st_mtim; 
    struct timespec st_ctim;  
    uid_t     st_uid;        				/* El usuario y grupo */
    gid_t     st_gid;  

    Artista lista_art;
    Album lista_album;
    Genero lista_genero;

    char *txt_borradir;        // texto del fichero busca
    int borradir_lenght;        // tamaño de busca
    size_t borradir_fh;     // id de busca
    // Añadimos nuevos registros para controlar los TAGs del fichero mp3
   
	//~ char *titulo[];
	//~ char *artista[];
	//~ char *album[];
	//~ int year;
	//~ char *genero[];
	//~ int numero_pista;
	
};

#endif
