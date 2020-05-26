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
 


int mp3_getattr(const char *path, struct stat *stbuf){
	
	int res = 0;
	struct estructura_mis_datos* datos = (struct estructura_mis_datos *)fuse_get_context()-> private data;
	
	memset(stbuf, 0, sizeof(struct stat));
	if (strcmp(path, "/") == 0) {
		stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = 4; //numero de entradas en sistema de ficheros (directorios por defecto 2 -> . y el ..)
        stbuf->st_uid = mis_datos->st_uid; //propietario del fichero
        stbuf->st_gid = mis_datos->st_gid; // grupo propietario del fichero

        stbuf->st_atime = mis_datos->st_atime; //horas de modificación etc ...
        stbuf->st_mtime = mis_datos->st_mtime;
        stbuf->st_ctime = mis_datos->st_ctime;
        stbuf->st_size = 4096; //tamaño
        stbuf->st_blocks = 2; // tamaño divido entre 512
        
	} else if (strcmp(path, "/Artistas") == 0) {
        stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = 2;
        stbuf->st_uid = mis_datos->st_uid;
        stbuf->st_gid = mis_datos->st_gid;

        stbuf->st_atime = mis_datos->st_atime;
        stbuf->st_mtime = mis_datos->st_mtime;
        stbuf->st_ctime = mis_datos->st_ctime;
        stbuf->st_size = 4096;
        stbuf->st_blocks = 2;
	
	} else if (strcmp(path, "/Favoritos") == 0) {
        stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = 2;
        stbuf->st_uid = mis_datos->st_uid;
        stbuf->st_gid = mis_datos->st_gid;

        stbuf->st_atime = mis_datos->st_atime;
        stbuf->st_mtime = mis_datos->st_mtime;
        stbuf->st_ctime = mis_datos->st_ctime;
        stbuf->st_size = 4096;
        stbuf->st_blocks = 2;
        
    }	
	// Completar
	
}

int mp3_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
				off_t offset, struct fuse_file_info *fi)
{	int i=0;
	struct estructura_mis_datos* datos = (struct estructura_mis_datos *)fuse_get_context()-> private data;
	
	if (strcmp(path, "/") == 0){
        if(filler(buf, ".", NULL, 0)!=0) return -ENOMEM;
        if(filler(buf, "..", NULL, 0)!=0) return -ENOMEM;
        if(filler(buf, "Artistas", NULL, 0)!=0) return -ENOMEM;
		if(filler(buf, "Favoritos", NULL, 0)!=0) return -ENOMEM;
		
    }else if(strcmp(path, "/Artistas") == 0){
		if(filler(buf, ".", NULL, 0)!=0) return -ENOMEM;
        if(filler(buf, "..", NULL, 0)!=0) return -ENOMEM;
        
        
        for(i; i < datos->numero_ficheros; i++){
			char artist=tag_get_[Artist];	// tomo el nombre del artista del tag del fichero
			if(filler(buf, artist, NULL, 0)!=0) return -ENOMEM;		// creo un subdirectorio con el nombre del artista
			
			for(i; i<datos->numero_ficheros; i++){	// para cada fichero mp3, compruebo si el nombre del artista "artist" coincide con el tag artista de cada fichero
				if(strcmp(artist, datos->nombre_ficheros[i]=0){
					if(filler(buf, datos->nombre_ficheros[i], NULL, 0) 
						return -ENOMEM;	// si coincide, meto la canción en el directorio
				}
			}
		}
	}else if(strcmp(path, "/Favoritos") == 0){
		if (filler(buf, ".", NULL, 0) != 0) return -ENOMEM;
        if (filler(buf, "..", NULL, 0) != 0) return -ENOMEM;
        
        for(i; i<datos->numero_ficheros; i++){
			if(strcmp(tag_get_[Comment], "FAV")=0){		// si en el campo "comentario" esta FAV escrito,
				if(filler(buf, datos->nombre_ficheros[i], NULL, 0) 	// añadimos la canción al subdirectorio favoritos
				return -ENOMEM;
			}
		}
	}else
		return -ENOMEM;
	
	return 0;
}
	
	
	
