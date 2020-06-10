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
#include "listas_oper.c"


/*
 *  Para usar los datos pasados a FUSE usar en las funciones:
 *
	struct structura_mis_datos *mis_datos= (struct structura_mis_datos *) fuse_get_context()->private_data;
 *
 * 
 */



int mp3_getattr(const char *path, struct stat *stbuf) {

    TagLib_File *file;
    TagLib_Tag *tag;
    int res = 0;
    struct estructura_mis_datos* mis_datos = (struct estructura_mis_datos *)fuse_get_context()->private_data;

    memset(stbuf, 0, sizeof(struct stat));
    if (strcmp(path, "/") == 0) {
        stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = 7; //numero de entradas en sistema de ficheros (directorios por defecto 2 -> . y el ..)
        stbuf->st_uid = mis_datos->st_uid; //propietario del fichero
        stbuf->st_gid = mis_datos->st_gid; // grupo propietario del fichero

        stbuf->st_atime = mis_datos->st_atime; //horas de modificación etc ...
        stbuf->st_mtime = mis_datos->st_mtime;
        stbuf->st_ctime = mis_datos->st_ctime;
        stbuf->st_size = 4096; //tamaño
        stbuf->st_blocks = 2; // tamaño divido entre 512

    } else if((strcmp(path, "/borrar") == 0)){

      /*  char *token = strtok((char*)path, "/"); // busca primera parte del path
        token = strtok(NULL,"/");
        if(token!=NULL){
        if(existe_fichero(token, mis_datos->path)){
            struct stat  fileStat;
            char lpath[1024];
            strcpy(lpath,mis_datos->path);
            strcat(lpath,"/");
            strcat(lpath,token);
            stat(lpath, &fileStat);  //leemos datos del fichero original
        */
            stbuf->st_mode = S_IFREG | 0666;
            stbuf->st_nlink = 1; //al ser un fichero solo tiene 1 enlace
            stbuf->st_uid = mis_datos->st_uid; 
            stbuf->st_gid = mis_datos->st_gid; 

            stbuf->st_atime = mis_datos->st_atime; 
            stbuf->st_mtime = mis_datos->st_mtime;
            stbuf->st_ctime = mis_datos->st_ctime;
            stbuf->st_size = mis_datos->borradir_lenght; //tamaño
            stbuf->st_blocks = stbuf->st_size / 512 + (stbuf->st_size % 512) ? 1 : 0;
            
    
    }else if (strcmp(path, "/Artistas") == 0) {
        stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = 2 + lista_size(mis_datos->lista_art);
        stbuf->st_uid = mis_datos->st_uid;
        stbuf->st_gid = mis_datos->st_gid;

        stbuf->st_atime = mis_datos->st_atime;
        stbuf->st_mtime = mis_datos->st_mtime;
        stbuf->st_ctime = mis_datos->st_ctime;
        stbuf->st_size = 4096;
        stbuf->st_blocks = 2;

    }else if (strcmp(path, "/Albums") == 0) {
        stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = 2 + lista_album_size(mis_datos->lista_album);
        stbuf->st_uid = mis_datos->st_uid;
        stbuf->st_gid = mis_datos->st_gid;

        stbuf->st_atime = mis_datos->st_atime;
        stbuf->st_mtime = mis_datos->st_mtime;
        stbuf->st_ctime = mis_datos->st_ctime;
        stbuf->st_size = 4096;
        stbuf->st_blocks = 2;

    }else if (strcmp(path, "/Generos") == 0) {
        stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = 2 + lista_genero_size(mis_datos->lista_genero);
        stbuf->st_uid = mis_datos->st_uid;
        stbuf->st_gid = mis_datos->st_gid;

        stbuf->st_atime = mis_datos->st_atime;
        stbuf->st_mtime = mis_datos->st_mtime;
        stbuf->st_ctime = mis_datos->st_ctime;
        stbuf->st_size = 4096;
        stbuf->st_blocks = 2;

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

    } else if(strncmp(path, "/Artistas/",10) == 0) {

        char *token = strtok((char*)path, "/"); // busca primera parte del path
        token = strtok(NULL,"/");
        if(esta_artista(mis_datos->lista_art,token)) // es un directorio de artista
        {
            token = strtok(NULL,"/");
            if(token==NULL)
            {   // es el directorio
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
            else // es un fichero dentro del directorio
            {
                if(existe_fichero(token, mis_datos->path))
                {
                    struct stat  fileStat;
                    char lpath[1024];
                    strcpy(lpath,mis_datos->path);
                    strcat(lpath,"/");
                    strcat(lpath,token);
                    stat(lpath, &fileStat);  //leemos datos del fichero original

                    stbuf->st_mode = S_IFREG | 0444;
                    stbuf->st_nlink = 1; //numero de entradas en sistema de ficheros (directorios por defecto 2 -> . y el ..)

                    stbuf->st_uid = fileStat.st_uid; //propietario del fichero
                    stbuf->st_gid = fileStat.st_gid; // grupo propietario del fichero
                    stbuf->st_atime = fileStat.st_atime; //horas de modificación etc ...
                    stbuf->st_mtime = fileStat.st_mtime;
                    stbuf->st_ctime = fileStat.st_ctime;
                    stbuf->st_size = fileStat.st_size; //tamaño
                    stbuf->st_blocks = stbuf->st_size / 512 + (stbuf->st_size % 512) ? 1 : 0; // tamaño divido entre 512

                }
                else
                    return -ENOENT;
            }

        }

    }else if(strncmp(path, "/Albums/",8) == 0) {

        char *token = strtok((char*)path, "/"); // busca primera parte del path
        token = strtok(NULL,"/");
        if(esta_album(mis_datos->lista_album,token)) // es un directorio de artista
        {
            token = strtok(NULL,"/");
            if(token==NULL)
            {   // es el directorio
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
            else // es un fichero dentro del directorio
            {
                if(existe_fichero(token, mis_datos->path))
                {
                    struct stat  fileStat;
                    char lpath[1024];
                    strcpy(lpath,mis_datos->path);
                    strcat(lpath,"/");
                    strcat(lpath,token);
                    stat(lpath, &fileStat);  //leemos datos del fichero original

                    stbuf->st_mode = S_IFREG | 0444;
                    stbuf->st_nlink = 1; //numero de entradas en sistema de ficheros (directorios por defecto 2 -> . y el ..)

                    stbuf->st_uid = fileStat.st_uid; //propietario del fichero
                    stbuf->st_gid = fileStat.st_gid; // grupo propietario del fichero
                    stbuf->st_atime = fileStat.st_atime; //horas de modificación etc ...
                    stbuf->st_mtime = fileStat.st_mtime;
                    stbuf->st_ctime = fileStat.st_ctime;
                    stbuf->st_size = fileStat.st_size; //tamaño
                    stbuf->st_blocks = stbuf->st_size / 512 + (stbuf->st_size % 512) ? 1 : 0; // tamaño divido entre 512

                }
                else
                    return -ENOENT;
            }

        }

    }else if(strncmp(path, "/Generos/",9) == 0) {

        char *token = strtok((char*)path, "/"); // busca primera parte del path
        token = strtok(NULL,"/");
        if(esta_genero(mis_datos->lista_genero,token)) // es un directorio de artista
        {
            token = strtok(NULL,"/");
            if(token==NULL)
            {   // es el directorio
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
            else // es un fichero dentro del directorio
            {
                if(existe_fichero(token, mis_datos->path))
                {
                    struct stat  fileStat;
                    char lpath[1024];
                    strcpy(lpath,mis_datos->path);
                    strcat(lpath,"/");
                    strcat(lpath,token);
                    stat(lpath, &fileStat);  //leemos datos del fichero original

                    stbuf->st_mode = S_IFREG | 0444;
                    stbuf->st_nlink = 1; //numero de entradas en sistema de ficheros (directorios por defecto 2 -> . y el ..)

                    stbuf->st_uid = fileStat.st_uid; //propietario del fichero
                    stbuf->st_gid = fileStat.st_gid; // grupo propietario del fichero
                    stbuf->st_atime = fileStat.st_atime; //horas de modificación etc ...
                    stbuf->st_mtime = fileStat.st_mtime;
                    stbuf->st_ctime = fileStat.st_ctime;
                    stbuf->st_size = fileStat.st_size; //tamaño
                    stbuf->st_blocks = stbuf->st_size / 512 + (stbuf->st_size % 512) ? 1 : 0; // tamaño divido entre 512

                }
                else
                    return -ENOENT;
            }

        }

    }else if (strncmp(path, "/Favoritos/",11) == 0) {
        char *token = strtok((char*)path, "/"); // busca primera parte del path
        token = strtok(NULL,"/");
        if(token!=NULL){
            struct stat  fileStat;
            char lpath[1024];
            strcpy(lpath,mis_datos->path);
            strcat(lpath,"/");
            strcat(lpath,token);
            stat(lpath, &fileStat);  //leemos datos del fichero original
            stbuf->st_mode = S_IFREG | 0444;
            stbuf->st_nlink = 1; //numero de entradas en sistema de ficheros (directorios por defecto 2 -> . y el ..)

            stbuf->st_uid = fileStat.st_uid; //propietario del fichero
            stbuf->st_gid = fileStat.st_gid; // grupo propietario del fichero
            stbuf->st_atime = fileStat.st_atime; //horas de modificación etc ...
            stbuf->st_mtime = fileStat.st_mtime;
            stbuf->st_ctime = fileStat.st_ctime;
            stbuf->st_size = fileStat.st_size; //tamaño
            stbuf->st_blocks = stbuf->st_size / 512 + (stbuf->st_size % 512) ? 1 : 0; // tamaño divido entre 512
        }else
            return -ENOENT;
    }
    // Completar
    return 0;
}

int mp3_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                off_t offset, struct fuse_file_info *fi)
{   int i=0;
    struct estructura_mis_datos* datos = (struct estructura_mis_datos *)fuse_get_context()->private_data;


    if (strcmp(path, "/") == 0) {
        if(filler(buf, ".", NULL, 0)!=0) return -ENOMEM;
        if(filler(buf, "..", NULL, 0)!=0) return -ENOMEM;
        if(filler(buf, "Artistas", NULL, 0)!=0) return -ENOMEM;
        if(filler(buf, "Favoritos", NULL, 0)!=0) return -ENOMEM;
        if(filler(buf, "Albums", NULL, 0)!=0) return -ENOMEM;
        if(filler(buf, "Generos", NULL, 0)!=0) return -ENOMEM;
        if(filler(buf, "borrar", NULL, 0) != 0)  return -ENOMEM;
/*
        DIR *dir;
        struct dirent *dp;
        char * file_name;
        dir = opendir(datos->path);
        fprintf(stderr,"  buscando borradir****");
        while ((dp=readdir(dir)) != NULL) {
            if (strcmp(dp->d_name, "borradir") ==0 ){
                char lpath[1024]; // fichero en el path original
                file_name = dp->d_name; // use it
                strcpy(lpath,datos->path);
                strcat(lpath,"/");
                strcat(lpath,file_name);
                fprintf(stderr,"  file_name: %s\n",file_name);
                fprintf(stderr,"  fichero: %s\n",lpath);
                //if(existe_fichero(file_name, lpath)
                if(filler(buf, file_name, NULL, 0) != 0)  return -ENOMEM;
            } else {
                 
            }
        }
        closedir(dir);
*/
    } else if(strcmp(path, "/Artistas") == 0) {
        if(filler(buf, ".", NULL, 0)!=0) return -ENOMEM;
        if(filler(buf, "..", NULL, 0)!=0) return -ENOMEM;
        Artista aux=datos->lista_art;
        while(aux) {
            if(filler(buf, aux->tag, NULL, 0)!=0) return -ENOMEM;
            aux=aux->sig;
        }

    } else if(strcmp(path, "/Albums") == 0) {
        if(filler(buf, ".", NULL, 0)!=0) return -ENOMEM;
        if(filler(buf, "..", NULL, 0)!=0) return -ENOMEM;
        Album aux=datos->lista_album;
        while(aux) {
            if(filler(buf, aux->tag, NULL, 0)!=0) return -ENOMEM;
            aux=aux->sig;
        }

    } else if(strcmp(path, "/Generos") == 0) {
        if(filler(buf, ".", NULL, 0)!=0) return -ENOMEM;
        if(filler(buf, "..", NULL, 0)!=0) return -ENOMEM;
        Genero aux=datos->lista_genero;
        while(aux) {
            if(filler(buf, aux->tag, NULL, 0)!=0) return -ENOMEM;
            aux=aux->sig;
        }

    }else if(strncmp(path, "/Artistas/",10) == 0) {
        char *token = strtok((char*)path, "/"); // busca primera parte del path
        token = strtok(NULL,"/");
        if(esta_artista(datos->lista_art,token)) // es un directorio de artista
        {
			char *artista=strdup(token);
            token = strtok(NULL,"/");
            if(token==NULL)
            {   // es el directorio
                if(filler(buf, ".", NULL, 0)!=0) return -ENOMEM;
                if(filler(buf, "..", NULL, 0)!=0) return -ENOMEM;
                DIR *dir;
                struct dirent *dp;
                char * file_name;
                dir = opendir(datos->path);
                while ((dp=readdir(dir)) != NULL) {
                    if ( !strcmp(dp->d_name, ".") || !strcmp(dp->d_name, "..") )
                    {
                        // do nothing (straight logic)
                    } else {
                        char lpath[1024]; // fichero en el path original
                        file_name = dp->d_name; // use it
                        strcpy(lpath,datos->path);
                        strcat(lpath,"/");
                        strcat(lpath,file_name);
                        fprintf(stderr,"  fichero: %s\n",lpath);
                        TagLib_File *file = taglib_file_new(lpath);
                        if(file==NULL) continue;
                        
                        TagLib_Tag *tag = taglib_file_tag(file);
                        char * arti = taglib_tag_artist(tag);

                        fprintf(stderr,"artist  - \"%s\"\n", arti);
                         // fprintf(stderr,"  Artist TAG: %s\n",artist_content->data);
                        //limpia_etiqueta(artist_content->data);
                        limpia_etiqueta(arti);
                        fprintf(stderr,"  arti: %s\n",arti);
                        //if(tag!=NULL)
                        if(strcmp(arti, artista) == 0)   // tiene el artista
                            if(filler(buf, file_name, NULL, 0)!=0) return -ENOMEM;

                    }
                }
                closedir(dir);
                free(artista);
            }
            else return -ENOENT;
        }
        else return -ENOENT;

    }else if(strncmp(path, "/Albums/",8) == 0) {
        char *token = strtok((char*)path, "/"); // busca primera parte del path
        token = strtok(NULL,"/");
        if(esta_album(datos->lista_album,token)) // es un directorio de artista
        {
            char *album=strdup(token);
            token = strtok(NULL,"/");
            if(token==NULL)
            {   // es el directorio
                if(filler(buf, ".", NULL, 0)!=0) return -ENOMEM;
                if(filler(buf, "..", NULL, 0)!=0) return -ENOMEM;
                DIR *dir;
                struct dirent *dp;
                char * file_name;
                dir = opendir(datos->path);
                while ((dp=readdir(dir)) != NULL) {
                    if ( !strcmp(dp->d_name, ".") || !strcmp(dp->d_name, "..") )
                    {
                        // do nothing (straight logic)
                    } else {
                        char lpath[1024]; // fichero en el path original
                        file_name = dp->d_name; // use it
                        strcpy(lpath,datos->path);
                        strcat(lpath,"/");
                        strcat(lpath,file_name);
                        fprintf(stderr,"  fichero: %s\n",lpath);
                        TagLib_File *file = taglib_file_new(lpath);
                        if(file==NULL) continue;
                        
                        TagLib_Tag *tag = taglib_file_tag(file);
                        char * alb = taglib_tag_album(tag);
                        limpia_etiqueta(alb);
                        if(strcmp(alb, album) == 0)   // tiene el artista
                            if(filler(buf, file_name, NULL, 0)!=0) return -ENOMEM;

                    }
                }
                closedir(dir);
                free(album);
            }
            else return -ENOENT;
        }
        else return -ENOENT;

    }else if(strncmp(path, "/Generos/",9) == 0) {
        char *token = strtok((char*)path, "/"); // busca primera parte del path
        token = strtok(NULL,"/");
        if(esta_genero(datos->lista_genero,token)) // es un directorio de artista
        {
            char *genero=strdup(token);
            token = strtok(NULL,"/");
            if(token==NULL)
            {   // es el directorio
                if(filler(buf, ".", NULL, 0)!=0) return -ENOMEM;
                if(filler(buf, "..", NULL, 0)!=0) return -ENOMEM;
                DIR *dir;
                struct dirent *dp;
                char * file_name;
                dir = opendir(datos->path);
                while ((dp=readdir(dir)) != NULL) {
                    if ( !strcmp(dp->d_name, ".") || !strcmp(dp->d_name, "..") )
                    {
                        // do nothing (straight logic)
                    } else {
                        char lpath[1024]; // fichero en el path original
                        file_name = dp->d_name; // use it
                        strcpy(lpath,datos->path);
                        strcat(lpath,"/");
                        strcat(lpath,file_name);
                        fprintf(stderr,"  fichero: %s\n",lpath);
                        TagLib_File *file = taglib_file_new(lpath);
                        if(file==NULL) continue;
                        
                        TagLib_Tag *tag = taglib_file_tag(file);
                        char * gen = taglib_tag_genre(tag);
                        limpia_etiqueta(gen);
                        if(strcmp(gen, genero) == 0)   // tiene el artista
                            if(filler(buf, file_name, NULL, 0)!=0) return -ENOMEM;

                    }
                }
                closedir(dir);
                free(genero);
            }
            else return -ENOENT;
        }
        else return -ENOENT;

    }else if(strcmp(path, "/Favoritos")==0){
        //if(filler(buf, ".", NULL, 0)!=0) return -ENOMEM;
        //if(filler(buf, "..", NULL, 0)!=0) return -ENOMEM;
        char *token = strtok((char*)path, "/"); // busca primera parte del path
        token = strtok(NULL,"/");
        if(token == NULL){
            DIR *dir;
            struct dirent *dp;
            char * file_name;
            dir = opendir(datos->path);
            fprintf(stderr,"  he podido abrir el dir desde favoritos");
            while ((dp=readdir(dir)) != NULL) {
                if ( !strcmp(dp->d_name, ".") || !strcmp(dp->d_name, "..") )
                {
                    // do nothing (straight logic)
                } else {
                    char lpath[1024]; // fichero en el path original
                    file_name = dp->d_name; // use it
                    strcpy(lpath,datos->path);
                    strcat(lpath,"/");
                    strcat(lpath,file_name);
                    fprintf(stderr,"  fichero: %s\n",lpath);
                    
                    TagLib_File *file = taglib_file_new(lpath);
                    if(file==NULL) continue;
                    
                    TagLib_Tag *tag = taglib_file_tag(file);
                    char * comment = taglib_tag_comment(tag);
                    fprintf(stderr,"comment  - \"%s\"\n", comment);
                    limpia_etiqueta(comment);
                    if(strcmp(comment, "FAV") == 0)   // tiene el artista
                        if(filler(buf, file_name, NULL, 0)!=0) return -ENOMEM;
                }
            }
            closedir(dir);
        }
    } else
        return -ENOENT;

    return 0;
}


int mp3_open(const char* path, struct fuse_file_info* fi) {

    struct estructura_mis_datos* mis_datos = (struct estructura_mis_datos*)fuse_get_context()->private_data;
    
    char lpath[512];
    char * filename= basename((char *)path);
    strcpy(lpath,mis_datos->path);
    strcat(lpath,"/");
    strcat(lpath,filename);

        /* open-only flags */
    //#define   O_RDONLY    0x0000      /* open for reading only */
    //#define   O_WRONLY    0x0001      /* open for writing only */
    //#define   O_RDWR      0x0002      /* open for reading and writing */
    //#define   O_ACCMODE   0x0003      /* mask for above modes */
    


    if (existe_fichero(filename, mis_datos->path)) // fichero
    {
        if ((fi->flags & 3) != O_RDONLY) return -EACCES;
        fi->fh=open(lpath,fi->flags);
        if(fi->fh<0) return -errno;

    }
    /*else if((strcmp(path, "/borrar")) == 0){
        if((fi->flags & O_ACCMODE) == 3) return -errno;
        fi->fh = open("/borrar", fi->flags);
        mis_datos->borradir_fh=fi->fh;
        fprintf(stderr, "fi->fh: %d", fi->fh);
        if(fi->flags < 0) return -errno;
        return 0;   
    }
    else
        return -ENOENT;
    */
    return 0;
}


int mp3_read(const char* path, char* buf, size_t size, off_t offset, struct fuse_file_info* fi) {
    
    //size_t len;
    //int i = fi->fh;
    //struct estructura_mis_datos* mis_datos = (struct estructura_mis_datos*)fuse_get_context()->private_data;

   // if(i != mis_datos->borradir_fh){
        return pread(fi->fh, buf, size, offset);

  /*  }else{

        len = (mis_datos->borradir_lenght);
        
        if (offset < len) {
            if (offset + size > len) size = len - offset;
            if(i == mis_datos->borradir_fh)
                memcpy(buf, mis_datos->txt_borradir + offset, size);
        
        } else
            size = 0;
       
    return size;
    }*/
}
/*
//Eliminar un directorio
int mp3_rmdir(const char* ruta) {

    fprintf(stderr, "ruta: %s\n",ruta );
    struct estructura_mis_datos* mis_datos = (struct estructura_mis_datos*)fuse_get_context()->private_data;
    char lpath[512];
    DIR *dir;
    struct dirent *dp;
    char * filename= basename((char *)ruta);
    fprintf(stderr, "filename: %s\n",filename );
    //strcpy(lpath,mis_datos->path);
    strcpy(lpath, "/home/kosta/mp3FUSE/punto_montaje");
    strcat(lpath,"/");
    strcat(lpath,filename);
    fprintf(stderr, "lpath: %s\n",lpath );
    dir = opendir(mis_datos->path);
    
    while ((dp=readdir(dir)) != NULL)
        if ( strcmp(dp->d_name, filename))  {
            unlink(dp->d_name);
            closedir(dir);
            free(dir);
            return 0;
        }
    closedir(dir);
    return 1;
}
*/
/*
int mp3_unlink(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf)
{
    int rv = mp3_rmdir(fpath);

    if (rv)
        perror(fpath);

    return rv;
}


int mp3_rmdir(const char * ruta){
    int flags = FTW_DEPTH | FTW_PHYS;
    return nftw(ruta, mp3_unlink, 64, flags);
}
*/


int mp3_release(const char *path, struct fuse_file_info *fi)
{
    return close(fi->fh);
}


int mp3_write(const char* path, const char* buf, size_t size, off_t offset,      //PROFESOR
                    struct fuse_file_info* fi)
{
    size_t len;
    int i = fi->fh;
    struct estructura_mis_datos* mis_datos = (struct estructura_mis_datos*)fuse_get_context()->private_data;
    if(i == mis_datos->borradir_fh)
    {   
        fprintf(stderr, "WRITE************\n");
        len = mis_datos->borradir_lenght;
        if (len==0) // no había nada reservar memoria
        {
            mis_datos->txt_borradir=malloc(offset+size+1); // uno más para añadir el caracter 0, por si es necesario para la comparación de cadenas
            if(mis_datos->txt_borradir==NULL) return -ENOMEM;
        }
        else if(offset+size > len) // necesitamos más espacio
        {
            char *punt = malloc(offset+size+1);
            if(punt==NULL) return -ENOMEM;
            memcpy(punt, mis_datos->txt_borradir, len);
            free(mis_datos->txt_borradir);
            mis_datos->txt_borradir=punt;
        }

        if(offset>len) memset(mis_datos->txt_borradir + len, ' ', offset-len); // relleno entre len y offset, no sé si será necesario

        memcpy(mis_datos->txt_borradir + offset, buf, size);
        if(offset+size > len) // hemos agrandado el contenido del fichero
        {
            *(mis_datos->txt_borradir + offset+size) = '\0'; // añado el caracter 0 al final, por si es necesario para la comparación de cadenas
            mis_datos->borradir_lenght=offset+size;
        }
        return size;
    }
    else
        return -ENOENT; // por aquí no debe pasar porque no dejara abrir un fichero diferente a /busca para escritura
    
}


int mp3_truncate(const char *path, off_t newsize)       
{
    struct estructura_mis_datos* mis_datos = (struct estructura_mis_datos*)fuse_get_context()->private_data;

    if (strcmp(path, "/borrar") == 0)
    {
        if(newsize==0) free(mis_datos->txt_borradir);
        mis_datos->borradir_lenght = newsize;
        return 0;
    }
    else
        return -EACCES;
    
}


struct fuse_operations basic_oper = {
    .getattr = mp3_getattr,
    .readdir = mp3_readdir,
    .open = mp3_open,
    .read = mp3_read,
    .release = mp3_release,
    .truncate = mp3_truncate,
    .write = mp3_write
};


int main(int argc, char *argv[])
{
    struct estructura_mis_datos* mis_datos;
    mis_datos = malloc(sizeof(struct estructura_mis_datos));
    int res;
    struct stat fileStat;
    taglib_set_strings_unicode(0);
    struct fuse_args args = FUSE_ARGS_INIT(argc, argv);
    // análisis parámetros de entrada
    if ((args.argc < 3) || (args.argv[args.argc - 2][0] == '-') || (args.argv[args.argc - 1][0] == '-'))
    {
        perror("Parametros insuficientes");
        exit(-1);
    }

    //  mis_datos->fichero_inicial = strdup(argv[argc - 2]); // fichero original
    mis_datos->path = realpath(args.argv[args.argc-2], NULL);
    if (mis_datos->path==NULL)
    {
        fprintf(stderr,"ERROR el directorio no existe\n");
        exit(-1);
    }
    args.argv[args.argc - 2] = args.argv[args.argc - 1];
    args.argv[args.argc - 1] = NULL;
    args.argc--;

    // leer metadatos del fichero
    fprintf(stderr,"real path: %s\n",mis_datos->path);

    stat(mis_datos->path, &fileStat);
    mis_datos->st_uid= fileStat.st_uid;
    mis_datos->st_gid= fileStat.st_gid;
    mis_datos->st_atime = fileStat.st_atime;
    mis_datos->st_ctime = fileStat.st_ctime;
    mis_datos->st_mtime = fileStat.st_mtime;
 
    fprintf(stderr,"Leyendo ficheros...\n");

    mis_datos->lista_art = rellenar_lista_artistas(mis_datos->path);
    mis_datos->lista_album = rellenar_lista_album(mis_datos->path);
    mis_datos->lista_genero = rellenar_lista_genero(mis_datos->path);

    fprintf(stderr,"ficheros indexados: %s\n",mis_datos->path);

    Artista aux_art=mis_datos->lista_art;
    while(aux_art) {
        fprintf(stderr," artista: %s\n",aux_art->tag);
        aux_art=aux_art->sig;
    }

    
    Album aux_alb=mis_datos->lista_album;
    while(aux_alb){
        fprintf(stderr, " album: %s\n",aux_alb->tag);
        aux_alb=aux_alb->sig;
    }

    Genero aux_gen=mis_datos->lista_genero;
    while(aux_gen){
        fprintf(stderr, " genero: %s\n",aux_gen->tag);
        aux_gen=aux_gen->sig;
    }

    res=fuse_main(args.argc, args.argv, &basic_oper, mis_datos);
    fuse_opt_free_args(&args);

    

    return res;
}