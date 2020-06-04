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
#include <Listas.c>


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
	
	} else if(strcmp(path, "/Artistas/") == 0){
		DIR * d = opendir(path); // open the path
		if(d==NULL) return; // if was not able return
		struct dirent * dir; // for the directory entries
		while ((dir = readdir(d)) != NULL) // if we were able to read something from the directory
		{
			if(dir -> d_type == DT_DIR && strcmp(dir->d_name,".")!=0 && strcmp(dir->d_name,"..")!=0 ) // if it is a directory
			{
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
		}
		closedir(d); // finally close the directory

	}else if (strcmp(path, "/Favoritos") == 0) {
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
	
	ID3v2_tag* artist_tag;
	ID3v2_frame* artist_frame;
	ID3v3_frame_text_content* artist_content;
	
	if (strcmp(path, "/") == 0){
        if(filler(buf, ".", NULL, 0)!=0) return -ENOMEM;
        if(filler(buf, "..", NULL, 0)!=0) return -ENOMEM;
        if(filler(buf, "Artistas", NULL, 0)!=0) return -ENOMEM;
		if(filler(buf, "Favoritos", NULL, 0)!=0) return -ENOMEM;
		
    }else if(strcmp(path, "/Artistas") == 0){
		if(filler(buf, ".", NULL, 0)!=0) return -ENOMEM;
        if(filler(buf, "..", NULL, 0)!=0) return -ENOMEM;
        
        
       /* for(i; i < datos->numero_ficheros; i++){
			char artist[]=tag_get_[Artist];	// tomo el nombre del artista del tag del fichero
			if(filler(buf, artist, NULL, 0)!=0) return -ENOMEM;		// creo un subdirectorio con el nombre del artista
			
			for(i; i<datos->numero_ficheros; i++){	// para cada fichero mp3, compruebo si el nombre del artista "artist" coincide con el tag artista de cada fichero
				if(strcmp(artist, datos->nombre_ficheros[i])=0){
					if(filler(buf, datos->nombre_ficheros[i], NULL, 0) 
						return -ENOMEM;	// si coincide, meto la canción en el directorio
				}
			}*/
		while(lista_art->sig != NULL){
			if(filler, lista_art->tag_artista, NULL, 0) return -ENOMEM;
			for(i; i<= datos->numero_ficheros; i++){
				artist_tag = ID3v2_tag * load_tag(datos->nombre_ficheros[i]);
				if(artista_tag==NULL) artista_tag=new_tag();
				artist_frame = tag_get_artist(artist_tag);
				artist_content = parse_text_frame_content(artist_frame);
				if(strcmp(lista_art->tag_artista, artist_content->data) == 0){
					if(filler(buf, datos->nombre_ficheros[i], NULL, 0) return -ENOMEM;
				}
			}
			sig=lista_art->sig;
		}
			
	}else if(strcmp(path, "/Favoritos") == 0){
		if (filler(buf, ".", NULL, 0) != 0) return -ENOMEM;
        if (filler(buf, "..", NULL, 0) != 0) return -ENOMEM;
        
        for(i; i<datos->numero_ficheros; i++){
			if(strcmp(tag_get_[Comment], "FAV")==0){		// si en el campo "comentario" esta FAV escrito,
				if(filler(buf, datos->nombre_ficheros[i], NULL, 0) 	// añadimos la canción al subdirectorio favoritos
				return -ENOMEM;
			}
		}
	}else
		return -ENOMEM;
	
	return 0;
}


int mp3_open(const char* path, struct fuse_file_info* fi){
	
	//MODIFICAR
	
	int i;

    struct estructura_mis_datos* datos = (struct estructura_mis_datos*)fuse_get_context()->private_data; //usarla para acceder a la estructura
		
		/* open-only flags */
	//#define	O_RDONLY	0x0000		/* open for reading only */
	//#define	O_WRONLY	0x0001		/* open for writing only */
	//#define	O_RDWR		0x0002		/* open for reading and writing */
	//#define	O_ACCMODE	0x0003		/* mask for above modes */
    
    
	if ((i=buscar_fichero(path , datos)) >0)
	{
		if ((fi->flags & 3) != O_RDONLY) return -EACCES;
		
		fi->fh=i;
		return 0;
		
	} 

}

	
int mp3_read(const char* path, char* buf, size_t size, off_t offset, struct fuse_file_info* fi){
	
	return pread(fi->fh, buf, size, offset);
}	


struct fuse_operations basic_oper = {
    .getattr = mp3_getattr,
    .readdir = mp3_readdir,
    .open = mp3_open,
    .read = mp3_read,

};


int main(int argc, char *argv[])
{
    struct structura_mis_datos* mis_datos;
    mis_datos = malloc(sizeof(struct structura_mis_datos));
    Artista lista_art = rellenar_lista_artistas();
    FILE *f;
    struct stat fileStat;

    // análisis parámetros de entrada
    if ((argc < 3) || (argv[argc - 2][0] == '-') || (argv[argc - 1][0] == '-'))
    {
        perror("Parametros insuficientes");
        exit(-1);
    }

    mis_datos->fichero_inicial = strdup(argv[argc - 2]); // fichero original
    argv[argc - 2] = argv[argc - 1];
    argv[argc - 1] = NULL;
    argc--;

    // leer metadatos del fichero
    mis_datos->path = realpath(argv[argc-2], NULL);
    fprintf(stderr,"real path: %s\n",mis_datos->path);
    f=fopen(mis_datos->fichero_inicial,"rc");
    if (f==NULL) {
        perror ("error imposible abrir fichero");
        exit(-1);
    }
    fstat(fileno(f), &fileStat);
    mis_datos->st_uid= fileStat.st_uid;
    mis_datos->st_gid= fileStat.st_gid;
    mis_datos->st_atime = fileStat.st_atime;
    mis_datos->st_ctime = fileStat.st_ctime;
    mis_datos->st_mtime = fileStat.st_mtime;
    mis_datos->st_size = fileStat.st_size;
    fclose(f);
    return fuse_main(argc, argv, &ejemplo_oper, mis_datos);
}
