/*
 * FUSE: Filesystem in Userspace
 * DSO 2020
 * Prueba proyecto librería de música
*/

#define FUSE_USE_VERSION 26

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <malloc.h>
#include <unistd.h>
#include <stdlib.h>
#include <limits.h>
#include <ctype.h>
#include <sys/types.h>
#include <dirent.h>
#include <libgen.h>
#include <taglib/tag_c.h>

#include "structures.h"


// Si está ya en la lista, devuelvo 1, si no un 0
static int esta_artista(Artista art, char *tag_art) {
    Artista aux = art;

    while(aux) {
        //fprintf(stderr,"##%s##\n##%s##\n",aux->tag_artista,tag_artista);
        if(strcmp(aux->tag,tag_art)==0)	return 1;
        aux=aux->sig;
    }
    return 0;
}

static int esta_album(Album alb, char *tag_alb){
    Album aux = alb;
    while(aux){
        if(strcmp(aux->tag, tag_alb)==0) return 1;
        aux=aux->sig;
    }
    return 0;
}

static int esta_genero(Genero gen, char *tag_gen){
    Genero aux = gen;
    while(aux){
        if(strcmp(aux->tag, tag_gen)==0) return 1;
        aux=aux->sig;
    }
    return 0;
}
/*************************************************/
static int lista_size(Artista art) {
    Artista aux = art;
    int contador=0;
    while(aux) {
        contador++;
        aux=aux->sig;
    }
    return contador;
}


static int lista_album_size(Album alb) {
    Album aux = alb;
    int contador=0;
    while(aux) {
        contador++;
        aux=aux->sig;
    }
    return contador;
}


static int lista_genero_size(Genero gen) {
    Genero aux = gen;
    int contador=0;
    while(aux) {
        contador++;
        aux=aux->sig;
    }
    return contador;
}

/************************************************/

void limpia_etiqueta(char * tag)
{
    int len= strlen(tag);
    for(int i= 0; i<len; i++)
    {
        if (tag[i]=='/' || tag[i]==' ') tag[i]='_';
    }

}

/************************************************/

// Inserto cierto artista
void insertar_artista(Artista *lista_art, char *tag_art) {
    Artista nuevoNodo, ptr;

    if(esta_artista(*lista_art, tag_art)==0) {
        //	fprintf(stderr,"   NUEVO ARTISTA\n");
        nuevoNodo=malloc(sizeof(struct lista_artista));
        nuevoNodo->tag=tag_art;
        nuevoNodo->sig=NULL;
        if(*lista_art == NULL) {		// lista vacia

            *lista_art=nuevoNodo;
        } else {						// insertamos al final
            ptr=*lista_art;
            while(ptr->sig!=NULL) ptr=ptr->sig;

            ptr->sig=nuevoNodo;
        }
    }
}


// Inserto cierto album
void insertar_album(Album *lista_alb, char *tag_alb) {
    Album nuevoNodo, ptr;

    if(esta_album(*lista_alb, tag_alb)==0) {
        //  fprintf(stderr,"   NUEVO ARTISTA\n");
        nuevoNodo=malloc(sizeof(struct lista_album));
        nuevoNodo->tag=tag_alb;
        nuevoNodo->sig=NULL;
        if(*lista_alb == NULL) {        // lista vacia

            *lista_alb=nuevoNodo;
        } else {                        // insertamos al final
            ptr=*lista_alb;
            while(ptr->sig!=NULL) ptr=ptr->sig;

            ptr->sig=nuevoNodo;
        }
    }
}

// Inserto cierto genero
void insertar_genero(Genero *lista_gen, char *tag_gen) {
    Genero nuevoNodo, ptr;

    if(esta_genero(*lista_gen, tag_gen)==0) {
        //  fprintf(stderr,"   NUEVO ARTISTA\n");
        nuevoNodo=malloc(sizeof(struct lista_genero));
        nuevoNodo->tag=tag_gen;
        nuevoNodo->sig=NULL;
        if(*lista_gen == NULL) {        // lista vacia

            *lista_gen=nuevoNodo;
        } else {                        // insertamos al final
            ptr=*lista_gen;
            while(ptr->sig!=NULL) ptr=ptr->sig;

            ptr->sig=nuevoNodo;
        }
    }
}

/************************************************/

// Crea una lista de artistas
Artista rellenar_lista_artistas(const char * path) {
	//taglib_set_strings_unicode(0);
    Artista lista=NULL;
    char lpath[512];
    TagLib_File *file;
  	TagLib_Tag *tag;
    DIR *dir;

    struct dirent *dp;
    char * file_name;
    dir = opendir(path);
    while ((dp=readdir(dir)) != NULL)
    {
        strcpy(lpath,path);
        strcat(lpath,"/");
        strcat(lpath,dp->d_name);
        //fprintf(stderr,"  fichero: %s\n",lpath);
    	file = taglib_file_new(lpath);
        //tag=taglib_file_tag(file);
        //artist_tag = load_tag(lpath);
        if(file==NULL) continue;
        tag=taglib_file_tag(file);
        char * arti = taglib_tag_artist(tag);
        fprintf(stderr, "ARTISTA TAG:  %s\n",arti);
        limpia_etiqueta(arti);
      	insertar_artista(&lista, arti);
		fprintf(stderr,"  -> insertado OK\n");

    }
    return lista;
}



// Crea una lista de artistas
Album rellenar_lista_album(const char * path) {
    //taglib_set_strings_unicode(0);
    Album lista=NULL;
    char lpath[512];
    TagLib_File *file;
    TagLib_Tag *tag;
    DIR *dir;

    struct dirent *dp;
    char * file_name;
    dir = opendir(path);
    while ((dp=readdir(dir)) != NULL)
    {
        strcpy(lpath,path);
        strcat(lpath,"/");
        strcat(lpath,dp->d_name);
        file = taglib_file_new(lpath);
        if(file==NULL) continue;
        tag=taglib_file_tag(file);
        char * alb = taglib_tag_album(tag);
        fprintf(stderr, "ALBUM TAG:  %s\n",alb);
        limpia_etiqueta(alb);
        insertar_album(&lista, alb);
        fprintf(stderr,"  -> insertado OK\n");

    }
    return lista;
}


// Crea una lista de generos
Genero rellenar_lista_genero(const char * path) {
    //taglib_set_strings_unicode(0);
    Genero lista=NULL;
    char lpath[512];
    TagLib_File *file;
    TagLib_Tag *tag;
    DIR *dir;

    struct dirent *dp;
    char * file_name;
    dir = opendir(path);
    while ((dp=readdir(dir)) != NULL)
    {
        strcpy(lpath,path);
        strcat(lpath,"/");
        strcat(lpath,dp->d_name);
        file = taglib_file_new(lpath);
        if(file==NULL) continue;
        tag=taglib_file_tag(file);
        char * gen = taglib_tag_genre(tag);
        fprintf(stderr, "GENERO TAG:  %s\n",gen);
        limpia_etiqueta(gen);
        insertar_genero(&lista, gen);
        fprintf(stderr,"  -> insertado OK\n");

    }
    return lista;
}

/************************************************/

int existe_fichero(const char * nombre, const char *path)
{
    DIR *dir;
    struct dirent *dp;
    char * file_name;
    dir = opendir(path);
    while ((dp=readdir(dir)) != NULL)
        if ( !strcmp(dp->d_name, nombre))  {
            closedir(dir);
            return 1;
        }
    closedir(dir);
    return 0;

}
