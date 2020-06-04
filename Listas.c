/*
 * FUSE: Filesystem in Userspace
 * DSO 2020
 * Prueba proyecto librería de música
*/

#define FUSE_USE_VERSION 26

#include <structures.h>
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
#include <id3v2lib.h>


/*
 *  Para usar los datos pasados a FUSE usar en las funciones:
 *
	struct structura_mis_datos *mis_datos= (struct structura_mis_datos *) fuse_get_context()->private_data;
 *
 * */
 

// Si está ya en la lista, devuelvo 1, si no un 0
static int esta(Artista art, char* tag_artista){
	Artista aux = art;
	
	while(aux->sig!=NULL){
		if(aux->tag_artista==tag_artista)
			return 1;
			
		aux=aux->sig;
	}
	return 0;
}

// Inserto cierto artista 
void insertar_artista(Artista *lista_art, char* tag_artista){
	Artista nuevoNodo, ptr;
	
	if(esta(art, tag_artista)==0){
		nuevoNodo=malloc(sizeof(struct lista_artista));
		nuevoNodo->tag_artista[]=tag_artista;
		
		if(*lista_art == NULL){		// lista vacia
			nuevoNodo->sig=NULL;
			*lista_art=nuevoNodo;
		}else{						// insertamos al final
			ptr=*lista_art;
			while(ptr->sig!=NULL)
				ptr=ptr->sig;
			
			ptr->sig=nuevoNodo;
		}
}


// Crea una lista de artistas
Artista rellenar_lista_artistas(){
	int i=0;
	struct estructura_mis_datos* datos = (struct estructura_mis_datos *)fuse_get_context()-> private data;
	
	Artista lista;
	ID3v2_tag* artist_tag;
	ID3v2_frame* artist_frame;
	ID3v3_frame_text_content* artist_content;
	
	for(i; i<= datos->numero_ficheros; i++){
		artist_tag = ID3v2_tag * load_tag(datos->nombre_ficheros[i]);
		if(artista_tag==NULL) artista_tag=new_tag();
		artist_frame = tag_get_artist(artist_tag);
		artist_content = parse_text_frame_content(artist_frame);
		insertar_artista(lista, artist_content->data);
	}
	return lista;
	
}

int buscar_fichero(const char *path, struct estructura_mis_datos *mis_datos)
{
	int i;
	
	for(i=0; i< mis_datos->numero_ficheros;i++)
	{
		if(strcmp(mis_datos->nombre_ficheros[i],path+1)==0)
		{
			return i;
		}
	}
	return -1;
}
