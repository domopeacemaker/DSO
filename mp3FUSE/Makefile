# ejemplo de uso FUSE para DSO 0
# hemos instalado el paquete de desarrollo de FUSE con:
# sudo apt-get install fuse-dev
# hemos instalado el paquete de desarrollo de EXIF con:
# sudo apt-get install libexif-dev

fuse_flags= -g -D_FILE_OFFSET_BITS=64 -lfuse -pthread
exif_flags= -l tag_c

.PHONY : mount umount test compila

compila : pruebaMP3

pruebaMP3 : pruebaMP3.c
	gcc -o $@ $^ ${fuse_flags} ${exif_flags}
	mkdir -p punto_montaje

mount : 
	./pruebaMP3 musica punto_montaje
debug :
	./pruebaMP3 -d musica punto_montaje 
umount :
	fusermount -u punto_montaje

